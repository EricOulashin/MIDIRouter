/* This file defines the functions for the MIDIThread class, which performs the loop
 * that processes MIDI events.  The loop will be different depending on the mode of
 * the application (local, client, server).
 */

#include "MIDIThread.h"
#include "midirouterapp.h"
#include "mainframe.h"
#include "misc.h"
#include "sleep.h"
#include "MIDIMessage.h"
#include <vector>
using std::vector;

// Global app and main frame pointers
extern MIDIRouterApp *gApp;
extern MainFrame *gMainFrame;

////////////////////
// Public methods //
////////////////////

MIDIThread::MIDIThread(wxThreadKind pKind)
   : wxThread(pKind)
{
}

MIDIThread::~MIDIThread() {
}

wxThread::ExitCode MIDIThread::Entry() {
   switch (gApp->mMode) {
      case eLocal:
         LocalMode();
         break;
      case eClient:
         ClientMode();
         break;
      case eServer:
         // Note: There is no server mode loop in this class, since the
         // socket stuff is event-driven; The MainFrame class has the
         // server logic.
         break;
   } // End switch

   return 0;
}

/////////////////////
// Private methods //
/////////////////////

void MIDIThread::LocalMode() {
   try {
      RtMidiIn *MIDIIn = gApp->mMIDIIn;
      RtMidiOut *MIDIOut = gApp->mMIDIOut;
      gMainFrame->GetStatusBar()->SetStatusText(_T("Running local mode.."), 0);
      // Make sure the MIDI ports are open
      MIDIIn->openPort(gMainFrame->GetMIDIInChoiceBox()->GetSelection());
      MIDIOut->openPort(gMainFrame->GetMIDIOutChoiceBox()->GetSelection());
      vector<unsigned char> message;
      // Don't ignore sysex, timing, or active sensing messages.
      MIDIIn->ignoreTypes(false, false, false);
      // Note: The app's mMIDILoopContinue member gets set to false when the
      // user clicks the "Stop" button on the GUI.
      while (gApp->mMIDILoopContinue) {
         MIDIIn->getMessage(&message);
         if (message.size() > 0) {
            MIDIOut->sendMessage(&message);
         }
         // Sleeping for 10 milliseconds significantly reduces CPU load.
         SLEEP(10);
      }
      // Close the ports
      MIDIOut->closePort();
      MIDIIn->closePort();
   }
   catch (RtError& pError) {
      ModalRtErrorBox(NULL, pError);
   }
   catch (const std::exception& pExc) {
      ModalMessageBox(gMainFrame, _T(pExc.what()), _T("Error"), wxOK | wxICON_ERROR);
   }

   // Update the GUI's statusbar to let the user know this thread is done.
   gMainFrame->GetStatusBar()->SetStatusText(_T("Local mode stopped."));
   // Re-enable the main frame's Go button
   gMainFrame->EnableGoButton(true);
}

void MIDIThread::ClientMode() {
   try {
      RtMidiIn *MIDIIn = gApp->mMIDIIn;
      RtMidiOut *MIDIOut = gApp->mMIDIOut;
      wxSocketClient *clientSocket = gMainFrame->GetClientSocket();

      // Make sure the MIDI In port is open
      MIDIIn->openPort(gMainFrame->GetMIDIInChoiceBox()->GetSelection());
      // If the user wants to also output to local MIDI Out, make sure the
      // MIDI Out port is open too.
      if (gApp->mClientLocalToo) {
         MIDIOut->openPort(gMainFrame->GetMIDIOutChoiceBox()->GetSelection());
      }

      // Ignore incoming sysex, timing, and active sensing messages due to their
      // relative high data rates.
      MIDIIn->ignoreTypes(true, true, true);

      MIDIMessage iMIDIMessage;
      vector<unsigned char> msgVector; // Will contain the message from MIDIIn
      msgVector.reserve(3); // MIDI messages contain up to 3 bytes
      while (gApp->mMIDILoopContinue) {
         MIDIIn->getMessage(&msgVector);
         if (msgVector.size() > 0) {
            // If the user wants to also output to local MIDI Out, then do it.
            if (gApp->mClientLocalToo) {
               MIDIOut->sendMessage(&msgVector);
            }

            // Set the iMIDIMessage data members and send it over the socket.
            iMIDIMessage.numBytes = (unsigned char)(msgVector.size());
            for (unsigned int i = 0; i < (unsigned int)iMIDIMessage.numBytes; ++i) {
               iMIDIMessage.message[i] = msgVector[i];
            }
            clientSocket->Write(&iMIDIMessage, sizeof(iMIDIMessage));
         }
         // Sleeping for 10 milliseconds significantly reduces CPU load.
         SLEEP(10);
      }
      // Close the MIDI port(s)
      MIDIIn->closePort();
      if (gApp->mClientLocalToo) {
         MIDIOut->closePort();
      }
   }
   catch (RtError& pError) {
      ModalRtErrorBox(NULL, pError);
   }
   catch (const std::exception& pExc) {
      ModalMessageBox(gMainFrame, _T(pExc.what()), _T("Error"), wxOK | wxICON_ERROR);
   }
}