#ifndef __MIDIThread_H__
#define __MIDIThread_H__
/* This file declares the MIDIThread class, which handles the processing of MIDI messages
 * as a separate thread.  The class derives from wxThread.
 */

#include <wx/thread.h>

class MIDIThread : public wxThread {
   public:
      // Constructor
      //
      // Parameters:
      //  pKind: The type of thread - Detached (default) or joinable
      explicit MIDIThread(wxThreadKind pKind = wxTHREAD_DETACHED);

      // Destructor
      ~MIDIThread();

      // Entry point for the thread - Starts a loop that routes MIDI messages from the
      // MIDI In port to the MIDI Out port
      wxThread::ExitCode Entry();

   private:
      // Runs the local mode loop
      void LocalMode();

      // Runs the client mode loop
      void ClientMode();
};

#endif