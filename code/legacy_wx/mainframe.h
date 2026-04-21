#ifndef _MAINFRAME_H_
#define _MAINFRAME_H_
/////////////////////////////////////////////////////////////////////////////
// Name:        mainframe.h
// Purpose:     
// Author:      Eric Oulashin
// Modified by: 
// Created:     28/03/2008 23:03:31
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

/*!
 * Includes
 */

////@begin includes
#include "wx/frame.h"
#include "wx/valtext.h"
#include "wx/statusbr.h"
////@end includes
#include "modes.h" // Mode enumeration
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/radiobut.h>
#include <wx/socket.h>
#include <wx/checkbox.h>

/*!
 * Forward declarations
 */

////@begin forward declarations
class wxStatusBar;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_MAINFRAME 10000
#define ID_PANEL1 10001
#define ID_RADIO_LOCAL 10004
#define ID_RADIO_SERVER 10006
#define ID_RADIO_CLIENT 10005
#define ID_CHECKBOX_LOCAL_TOO 10014
#define ID_BUTTON_GO 10009
#define ID_BUTTON_STOP 10013
#define ID_CHOICE_MIDI_IN 10007
#define ID_CHOICE_MIDI_OUT 10008
#define ID_TEXT_SERVER 10011
#define ID_TEXT_PORT 10012
#define ID_FILE_EXIT 10003
#define ID_HELP_ABOUT 10002
#define ID_STATUSBAR 10010
#define SYMBOL_MAINFRAME_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxMINIMIZE_BOX|wxCLOSE_BOX|wxFRAME_EX_METAL
#define SYMBOL_MAINFRAME_TITLE _("MIDI Router")
#define SYMBOL_MAINFRAME_IDNAME ID_MAINFRAME
#define SYMBOL_MAINFRAME_SIZE wxSize(435, 270)
#define SYMBOL_MAINFRAME_POSITION wxDefaultPosition
////@end control identifiers
// Socket-related identifiers
#define SERVER_ID 11002
#define SERVER_SOCKET_ID 11000
#define CLIENT_SOCKET_ID 11001

/*!
 * MainFrame class declaration
 */

class MainFrame: public wxFrame
{    
    DECLARE_CLASS( MainFrame )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    MainFrame();
    MainFrame( wxWindow* parent, wxWindowID id = SYMBOL_MAINFRAME_IDNAME, const wxString& caption = SYMBOL_MAINFRAME_TITLE, const wxPoint& pos = SYMBOL_MAINFRAME_POSITION, const wxSize& size = SYMBOL_MAINFRAME_SIZE, long style = SYMBOL_MAINFRAME_STYLE );

    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_MAINFRAME_IDNAME, const wxString& caption = SYMBOL_MAINFRAME_TITLE, const wxPoint& pos = SYMBOL_MAINFRAME_POSITION, const wxSize& size = SYMBOL_MAINFRAME_SIZE, long style = SYMBOL_MAINFRAME_STYLE );

    /// Destructor
    ~MainFrame();

    // Returns a pointer to the MIDI In wxChoice object
    wxChoice* GetMIDIInChoiceBox() const;

    // Returns a pointer to the MIDI Out wxChoice object
    wxChoice* GetMIDIOutChoiceBox() const;

    // Returns a pointer to the client socket
    wxSocketClient* GetClientSocket() const;

    // Enables or disables the 'Go' button
    //
    // Parameters:
    //  pEnable: If true, the button will be enabled; otherwise, the button
    //           will be disabled.
    void EnableGoButton(bool pEnable);

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin MainFrame event handler declarations

////@end MainFrame event handler declarations

////@begin MainFrame member function declarations

   /// Retrieves bitmap resources
   wxBitmap GetBitmapResource( const wxString& name );

   /// Retrieves icon resources
   wxIcon GetIconResource( const wxString& name );
////@end MainFrame member function declarations

   /// Should we show tooltips?
   static bool ShowToolTips();
    
   // Event function for when the user clicks File > Exit: Closes the application
   void OnFileExit(wxCommandEvent& WXUNUSED(event));

   // Evenft function for when the user clicks Help > About: Shows the about box
   void OnHelpAbout(wxCommandEvent& WXUNUSED(event));

   // Event function for when the user clicks the 'Local' radio button
   void OnClickLocal(wxCommandEvent& WXUNUSED(event));
   
   // Event function for when the user clicks the 'Client' radio button
   void OnClickClient(wxCommandEvent& WXUNUSED(event));
   
   // Event function for when the user clicks the 'Server' radio button
   void OnClickServer(wxCommandEvent& WXUNUSED(event));

   // Event function for when the user selects a MIDI In port
   void OnSelectMIDIIn(wxCommandEvent& WXUNUSED(event));

   // Event function for when the user selects a MIDI Out port
   void OnSelectMIDIOut(wxCommandEvent& WXUNUSED(event));

   // Event function for when the user clicks the Go button - In local mode, this routes
   // from the MIDI In port to the MIDI Out port.  In client mode, this connects & sends
   // MIDI to the server.  In host mode, this waits for a connection and sends incoming
   // MIDI data to the MIDI Out port.
   void OnGoButton(wxCommandEvent& WXUNUSED(event));

   // Event function for when the user clicks the Stop button - Stops the currently-running
   // secondary thread.
   void OnStopButton(wxCommandEvent& WXUNUSED(event));

   // Server event handler for incoming connections
   void OnServerEvent(wxSocketEvent& event);

   // Socket event handler for the server socket
   void OnServerSocketEvent(wxSocketEvent& event);

   // Socket event handler for the client socket
   void OnClientSocketEvent(wxSocketEvent& event);

   // Event function for when the user clicks the "Local too" checkbox in client mode
   void OnClientLocalTooChkbox(wxCommandEvent& WXUNUSED(event));

   // Populates the MIDI input selection control
   void PopulateMIDIIn();

   // Populates the MIDI output selection control
   void PopulateMIDIOut();

private:
////@begin MainFrame member variables
   wxPanel* mPanel;
   wxRadioButton* mRadioLocal;
   wxRadioButton* mRadioServer;
   wxRadioButton* mRadioClient;
   wxCheckBox* mChkBoxLocalToo;
   wxButton* mGoButton;
   wxButton* mStopButton;
   wxStaticText* mStaticTextMIDIIn;
   wxChoice* mChoiceMIDIIn;
   wxStaticText* mStaticTextMIDIOut;
   wxChoice* mChoiceMIDIOut;
   wxStaticBoxSizer* mConnectionSettingsSizer;
   wxStaticText* mStaticTextServer;
   wxStaticText* mStaticTextPort;
   wxTextCtrl* mTextCtrlServer;
   wxTextCtrl* mTextCtrlPort;
   wxStatusBar* mStatusBar;
////@end MainFrame member variables
   wxSocketServer *mServerSocket;
   wxSocketClient *mClientSocket;

   // Changes the GUI mode.  This only affects which controls are shown
   // in the GUI; this does not actually start the server or client thread.
   //
   // Parameters:
   //  pMode: The mode - can be one of the following:
   //         eLocal: Local mode (no network connection)
   //         eClient: Client mode (connect & transmit to server)
   //         eServer: Server mode (receive MIDI from client)
   void ChangeGUIMode(modes pMode);

   // Stops the current application mode (local, client, server)
   void StopCurrentMode();

   // Enables or disables the server input and server static text control.
   void EnableServerInput(bool pEnable);

   // Enables or disables the server port input and server static text control.
   void EnablePortInput(bool pEnable);

   // Enables or disables the MIDI In input and server static text control.
   void EnableMIDIInInput(bool pEnable);

   // Enables or disables the MIDI Out input and server static text control.
   void EnableMIDIOutInput(bool pEnable);

   // Enables & disables the appropriate GUI elements for local mode.
   void SetLocalModeGUI();

   // Enables & disables the appropriate GUI elements for server mode.
   void SetServerModeGUI();

   // Enables & disables the appropriate GUI elements for client mode.
   void SetClientModeGUI();
};

#endif
    // _MAINFRAME_H_
