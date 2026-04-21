#ifndef _MIDIROUTERAPP_H_
#define _MIDIROUTERAPP_H_
/////////////////////////////////////////////////////////////////////////////
// Name:        midirouterapp.h
// Purpose:     
// Author:      Eric Oulashin
// Modified by: 
// Created:     28/03/2008 23:03:10
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

/* The following platforms/APIs are supported with the following preprocessor flags.  One
 * of these preprocessor flags must be used when compiling this project in order for the
 * correct platform-specific code to be compiled in:
 * - Win32: __WINDOWS_MM__
 * - Mac OS X: __MACOSX_CORE__
 * - Linux with Alsa API: __LINUX_ALSASEQ__
 * - IRIX MD API: __IRIX_MD__
 */

/*!
 * Includes
 */

////@begin includes
#include "wx/image.h"
#include "mainframe.h"
////@end includes
#include <wx/app.h>
#include "modes.h" // Mode enumeration
#include "RtMidi.h"

/*!
 * Forward declarations
 */

////@begin forward declarations
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
////@end control identifiers

/*!
 * MIDIRouterApp class declaration
 */

class MIDIRouterApp: public wxApp
{    
    DECLARE_CLASS( MIDIRouterApp )
    DECLARE_EVENT_TABLE()

public:
    /// Constructor
    MIDIRouterApp();

    void Init();

    /// Initialises the application
    virtual bool OnInit();

    /// Called on exit
    virtual int OnExit();

////@begin MIDIRouterApp event handler declarations

////@end MIDIRouterApp event handler declarations

////@begin MIDIRouterApp member function declarations

////@end MIDIRouterApp member function declarations

////@begin MIDIRouterApp member variables
////@end MIDIRouterApp member variables
   MainFrame *mMainWindow; // The main GUI window
   modes mMode;            // The application mode (local, client, server)
   RtMidiIn *mMIDIIn;      // MIDI input interface
   RtMidiOut *mMIDIOut;    // MIDI output interface
   bool mMIDILoopContinue; // For controlling thread loops/MIDI functionality
   bool mServerConnected;  // Will be set to true if already connected to a client in server mode
   bool mClientLocalToo;   // Whether or not to also send MIDI to local MIDI Out when in client mode
};

/*!
 * Application instance declaration 
 */

////@begin declare app
DECLARE_APP(MIDIRouterApp)
////@end declare app

#endif