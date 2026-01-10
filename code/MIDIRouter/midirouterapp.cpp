/////////////////////////////////////////////////////////////////////////////
// Name:        midirouterapp.cpp
// Purpose:     
// Author:      Eric Oulashin
// Modified by: 
// Created:     28/03/2008 23:03:10
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
////@end includes

#include "midirouterapp.h"
#include "wx/msgdlg.h"
#include "wx/string.h"
#include "misc.h"
#include "sleep.h"

////@begin XPM images
////@end XPM images

// Global application object pointer (can be accessed in other source files)
MIDIRouterApp *gApp(NULL);


/*!
 * Application instance implementation
 */

////@begin implement app
IMPLEMENT_APP( MIDIRouterApp )
////@end implement app


/*!
 * MIDIRouterApp type definition
 */

IMPLEMENT_CLASS( MIDIRouterApp, wxApp )


/*!
 * MIDIRouterApp event table definition
 */

BEGIN_EVENT_TABLE( MIDIRouterApp, wxApp )

////@begin MIDIRouterApp event table entries
////@end MIDIRouterApp event table entries

END_EVENT_TABLE()


/*!
 * Constructor for MIDIRouterApp
 */

MIDIRouterApp::MIDIRouterApp()
   : mMode(eLocal),
     mMIDIIn(NULL),
     mMIDIOut(NULL),
     mMIDILoopContinue(false),
     mServerConnected(false),
     mClientLocalToo(false)
{
   gApp = this;
   Init();
}


/*!
 * Member initialisation
 */

void MIDIRouterApp::Init() {
////@begin MIDIRouterApp member initialisation
////@end MIDIRouterApp member initialisation
}

/*!
 * Initialisation for MIDIRouterApp
 */

bool MIDIRouterApp::OnInit() {    
	// Remove the comment markers above and below this block
	// to make permanent changes to the code.
/*
#if wxUSE_XPM
	wxImage::AddHandler(new wxXPMHandler);
#endif
#if wxUSE_LIBPNG
	wxImage::AddHandler(new wxPNGHandler);
#endif
#if wxUSE_LIBJPEG
	wxImage::AddHandler(new wxJPEGHandler);
#endif
#if wxUSE_GIF
	wxImage::AddHandler(new wxGIFHandler);
#endif
   */
   // Set up mMIDIIn and mMIDIOut.  The constructors may throw RtError exceptions, so that's why
   // they are pointers allocated with new (so the exceptions can be caught when they are created).
   try {
      mMIDIIn = new RtMidiIn();
   }
   catch (RtError& pError) {
      ModalRtErrorBox(NULL, pError);
   }
   try {
      mMIDIOut = new RtMidiOut();
   }
   catch (RtError& pError) {
      ModalRtErrorBox(NULL, pError);
   }

   // If there is at least 1 MIDI input or output port, then create & show
   // the main window.
   bool retval = true;
   if ((mMIDIIn->getPortCount() > 0) || (mMIDIOut->getPortCount() > 0)) {
	   MainFrame *mainWindow = new MainFrame(NULL);
	   mainWindow->Show(true);
      SetTopWindow(mainWindow); // Tell the app this is the main window
   }
   else {
      ModalMessageBox(NULL, _T("There are no MIDI ports available.  The program will now exit."),
                      _T("Warning"), wxOK | wxICON_EXCLAMATION);
      retval = false;
   }

   return(retval);
}


/*!
 * Cleanup for MIDIRouterApp
 */

int MIDIRouterApp::OnExit() {
   // Make sure any running MIDI thread is stopped.
   mMIDILoopContinue = false;
   SLEEP(2); // Wait to make sure the thread is (hopefully) stopped
   // Make sure the MIDI ports are closed and the memory used by the RtMidi objects
   // is freed.
   if (mMIDIIn != NULL) {
      try {
         mMIDIIn->closePort();
      }
      catch (RtError& WXUNUSED(pError)) {
      }

      delete mMIDIIn;
   }
   if (mMIDIOut != NULL) {
      try {
         mMIDIOut->closePort();
      }
      catch (RtError& WXUNUSED(pError)) {
      }

      delete mMIDIOut;
   }
////@begin MIDIRouterApp cleanup
	return wxApp::OnExit();
////@end MIDIRouterApp cleanup
}

