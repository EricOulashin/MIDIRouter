/////////////////////////////////////////////////////////////////////////////
// Name:        mainframe.cpp
// Purpose:     
// Author:      Eric Oulashin
// Modified by: 
// Created:     28/03/2008 23:03:31
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include <wx/menu.h>
#include <wx/aboutdlg.h>
#include <wx/icon.h>
#include "mainframe.h"
#include "midirouterapp.h"
#include "misc.h"
#include "MIDIThread.h"
#include "MIDIMessage.h"
#include <vector>
using std::vector;

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
////@end includes

////@begin XPM images
////@end XPM images

// Global application object pointer
extern MIDIRouterApp *gApp;

// Global main frame pointer
MainFrame *gMainFrame(NULL);

/*!
 * MainFrame type definition
 */

IMPLEMENT_CLASS( MainFrame, wxFrame )


/*!
 * MainFrame event table definition
 */

BEGIN_EVENT_TABLE( MainFrame, wxFrame )

////@begin MainFrame event table entries
////@end MainFrame event table entries
   // File menu functions
   EVT_MENU(ID_FILE_EXIT, MainFrame::OnFileExit)
   // Help menu functions
   EVT_MENU(ID_HELP_ABOUT, MainFrame::OnHelpAbout)
   // Local/client/server radio button functions
   EVT_RADIOBUTTON(ID_RADIO_LOCAL, MainFrame::OnClickLocal)
   EVT_RADIOBUTTON(ID_RADIO_CLIENT, MainFrame::OnClickClient)
   EVT_RADIOBUTTON(ID_RADIO_SERVER, MainFrame::OnClickServer)
   // MIDI In & Out selection functions
   EVT_CHOICE(ID_CHOICE_MIDI_IN, MainFrame::OnSelectMIDIIn)
   EVT_CHOICE(ID_CHOICE_MIDI_OUT, MainFrame::OnSelectMIDIOut)
   // Buttons
   EVT_BUTTON(ID_BUTTON_GO, MainFrame::OnGoButton)
   EVT_BUTTON(ID_BUTTON_STOP, MainFrame::OnStopButton)
   // Server socket events
   EVT_SOCKET(SERVER_ID, MainFrame::OnServerEvent)
   EVT_SOCKET(SERVER_SOCKET_ID, MainFrame::OnServerSocketEvent)
   // Client socket event
   EVT_SOCKET(CLIENT_SOCKET_ID, MainFrame::OnClientSocketEvent)
   // "Local too" checkbox
   EVT_CHECKBOX(ID_CHECKBOX_LOCAL_TOO, MainFrame::OnClientLocalTooChkbox)

END_EVENT_TABLE()


/*!
 * MainFrame constructors
 */

MainFrame::MainFrame()
   : mPanel(NULL),
     mGoButton(NULL),
     mStaticTextMIDIIn(NULL),
     mChoiceMIDIIn(NULL),
     mStaticTextMIDIOut(NULL),
     mChoiceMIDIOut(NULL),
     mConnectionSettingsSizer(NULL),
     mStaticTextServer(NULL),
     mTextCtrlServer(NULL),
     mTextCtrlPort(NULL),
     mStatusBar(NULL),
     mServerSocket(NULL),
     mClientSocket(NULL)
{
   gMainFrame = this;
}

MainFrame::MainFrame(wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style)
   : mPanel(NULL),
     mGoButton(NULL),
     mStaticTextMIDIIn(NULL),
     mChoiceMIDIIn(NULL),
     mStaticTextMIDIOut(NULL),
     mChoiceMIDIOut(NULL),
     mConnectionSettingsSizer(NULL),
     mStaticTextServer(NULL),
     mTextCtrlServer(NULL),
     mTextCtrlPort(NULL),
     mStatusBar(NULL),
     mServerSocket(NULL),
     mClientSocket(NULL)
{
   gMainFrame = this;
   Create( parent, id, caption, pos, size, style );
}


/*!
 * MainFrame creator
 */

bool MainFrame::Create(wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style) {
////@begin MainFrame creation
   wxFrame::Create( parent, id, caption, pos, size, style );

   CreateControls();
   Centre();
////@end MainFrame creation
    return true;
}


/*!
 * MainFrame destructor
 */

MainFrame::~MainFrame() {
////@begin MainFrame destruction
////@end MainFrame destruction
}

wxChoice* MainFrame::GetMIDIInChoiceBox() const {
   return(mChoiceMIDIIn);
}

wxChoice* MainFrame::GetMIDIOutChoiceBox() const {
   return(mChoiceMIDIOut);
}

wxSocketClient* MainFrame::GetClientSocket() const {
   return(mClientSocket);
}

void MainFrame::EnableGoButton(bool pEnable) {
   mGoButton->Enable(pEnable);
}

/*!
 * Member initialisation
 */

void MainFrame::Init() {
////@begin MainFrame member initialisation
   mPanel = NULL;
   mRadioLocal = NULL;
   mRadioServer = NULL;
   mRadioClient = NULL;
   mChkBoxLocalToo = NULL;
   mGoButton = NULL;
   mStopButton = NULL;
   mStaticTextMIDIIn = NULL;
   mChoiceMIDIIn = NULL;
   mStaticTextMIDIOut = NULL;
   mChoiceMIDIOut = NULL;
   mConnectionSettingsSizer = NULL;
   mStaticTextServer = NULL;
   mStaticTextPort = NULL;
   mTextCtrlServer = NULL;
   mTextCtrlPort = NULL;
   mStatusBar = NULL;
////@end MainFrame member initialisation
   mServerSocket = NULL;
   gMainFrame = this;
}


/*!
 * Control creation for MainFrame
 */

void MainFrame::CreateControls()
{    
////@begin MainFrame content construction
   MainFrame* itemFrame1 = this;

   wxMenuBar* menuBar = new wxMenuBar;
   wxMenu* itemMenu26 = new wxMenu;
   itemMenu26->Append(ID_FILE_EXIT, _("&Exit"), _T(""), wxITEM_NORMAL);
   menuBar->Append(itemMenu26, _("&File"));
   wxMenu* itemMenu28 = new wxMenu;
   itemMenu28->Append(ID_HELP_ABOUT, _("&About"), _T(""), wxITEM_NORMAL);
   menuBar->Append(itemMenu28, _("&Help"));
   itemFrame1->SetMenuBar(menuBar);

   mPanel = new wxPanel( itemFrame1, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );

   wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxVERTICAL);
   mPanel->SetSizer(itemBoxSizer3);

   wxStaticBox* itemStaticBoxSizer4Static = new wxStaticBox(mPanel, wxID_ANY, _("Mode"));
   wxStaticBoxSizer* itemStaticBoxSizer4 = new wxStaticBoxSizer(itemStaticBoxSizer4Static, wxHORIZONTAL);
   itemBoxSizer3->Add(itemStaticBoxSizer4, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

   mRadioLocal = new wxRadioButton( mPanel, ID_RADIO_LOCAL, _("Local"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
   mRadioLocal->SetValue(true);
   itemStaticBoxSizer4->Add(mRadioLocal, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

   mRadioServer = new wxRadioButton( mPanel, ID_RADIO_SERVER, _("Server"), wxDefaultPosition, wxDefaultSize, 0 );
   mRadioServer->SetValue(false);
   itemStaticBoxSizer4->Add(mRadioServer, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

   mRadioClient = new wxRadioButton( mPanel, ID_RADIO_CLIENT, _("Client"), wxDefaultPosition, wxDefaultSize, 0 );
   mRadioClient->SetValue(false);
   itemStaticBoxSizer4->Add(mRadioClient, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

   mChkBoxLocalToo = new wxCheckBox( mPanel, ID_CHECKBOX_LOCAL_TOO, _("Local too"), wxDefaultPosition, wxDefaultSize, 0 );
   mChkBoxLocalToo->SetValue(false);
   mChkBoxLocalToo->Enable(false);
   itemStaticBoxSizer4->Add(mChkBoxLocalToo, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

   mGoButton = new wxButton( mPanel, ID_BUTTON_GO, _("Start"), wxDefaultPosition, wxDefaultSize, 0 );
   itemStaticBoxSizer4->Add(mGoButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

   mStopButton = new wxButton( mPanel, ID_BUTTON_STOP, _("Stop"), wxDefaultPosition, wxDefaultSize, 0 );
   itemStaticBoxSizer4->Add(mStopButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

   wxBoxSizer* itemBoxSizer11 = new wxBoxSizer(wxHORIZONTAL);
   itemBoxSizer3->Add(itemBoxSizer11, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

   wxBoxSizer* itemBoxSizer12 = new wxBoxSizer(wxVERTICAL);
   itemBoxSizer11->Add(itemBoxSizer12, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

   mStaticTextMIDIIn = new wxStaticText( mPanel, wxID_STATIC, _("MIDI In"), wxDefaultPosition, wxDefaultSize, 0 );
   itemBoxSizer12->Add(mStaticTextMIDIIn, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

   wxArrayString mChoiceMIDIInStrings;
   mChoiceMIDIIn = new wxChoice( mPanel, ID_CHOICE_MIDI_IN, wxDefaultPosition, wxDefaultSize, mChoiceMIDIInStrings, 0 );
   itemBoxSizer12->Add(mChoiceMIDIIn, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

   mStaticTextMIDIOut = new wxStaticText( mPanel, wxID_STATIC, _("MIDI Out"), wxDefaultPosition, wxDefaultSize, 0 );
   itemBoxSizer12->Add(mStaticTextMIDIOut, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

   wxArrayString mChoiceMIDIOutStrings;
   mChoiceMIDIOut = new wxChoice( mPanel, ID_CHOICE_MIDI_OUT, wxDefaultPosition, wxDefaultSize, mChoiceMIDIOutStrings, 0 );
   itemBoxSizer12->Add(mChoiceMIDIOut, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

   wxStaticBox* itemStaticBoxSizer17Static = new wxStaticBox(mPanel, wxID_ANY, _("Connection settings"));
   mConnectionSettingsSizer = new wxStaticBoxSizer(itemStaticBoxSizer17Static, wxVERTICAL);
   itemBoxSizer11->Add(mConnectionSettingsSizer, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

   wxBoxSizer* itemBoxSizer18 = new wxBoxSizer(wxHORIZONTAL);
   mConnectionSettingsSizer->Add(itemBoxSizer18, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

   wxBoxSizer* itemBoxSizer19 = new wxBoxSizer(wxVERTICAL);
   itemBoxSizer18->Add(itemBoxSizer19, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

   mStaticTextServer = new wxStaticText( mPanel, wxID_STATIC, _("Server"), wxDefaultPosition, wxDefaultSize, 0 );
   itemBoxSizer19->Add(mStaticTextServer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

   mStaticTextPort = new wxStaticText( mPanel, wxID_STATIC, _("Port"), wxDefaultPosition, wxDefaultSize, 0 );
   itemBoxSizer19->Add(mStaticTextPort, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

   wxBoxSizer* itemBoxSizer22 = new wxBoxSizer(wxVERTICAL);
   itemBoxSizer18->Add(itemBoxSizer22, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

   mTextCtrlServer = new wxTextCtrl( mPanel, ID_TEXT_SERVER, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
   mTextCtrlServer->SetHelpText(_("Enter the name or IP address of the server"));
   if (MainFrame::ShowToolTips())
      mTextCtrlServer->SetToolTip(_("Enter the name or IP address of the server"));
   itemBoxSizer22->Add(mTextCtrlServer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

   mTextCtrlPort = new wxTextCtrl( mPanel, ID_TEXT_PORT, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
   mTextCtrlPort->SetHelpText(_("Enter the port of the server"));
   if (MainFrame::ShowToolTips())
      mTextCtrlPort->SetToolTip(_("Enter the port of the server"));
   itemBoxSizer22->Add(mTextCtrlPort, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

   mStatusBar = new wxStatusBar( itemFrame1, ID_STATUSBAR, wxST_SIZEGRIP|wxNO_BORDER );
   mStatusBar->SetFieldsCount(1);
   itemFrame1->SetStatusBar(mStatusBar);

   // Set validators
   mTextCtrlPort->SetValidator( wxTextValidator(wxFILTER_NUMERIC) );
////@end MainFrame content construction

   // If compiling for Windows, include the app icon from resources in the dialog box.
   #ifdef __WINDOWS_MM__
   SetIcon(wxIcon(_T("WXAPPICON"), wxBITMAP_TYPE_ICO_RESOURCE));
   #endif

   // Set the minimum size of this frame.  435x270 is big enough to fit all the controls
   // in Windows, so I'm calling this the minimum size.
   SetMinSize(wxSize(435, 270));

   // Disable the server & port inputs, since this program starts up in local mode by default.
   EnableServerInput(false);
   EnablePortInput(false);

   // Populate the MIDI In & Out choice controls
   PopulateMIDIIn();
   PopulateMIDIOut();
   // If there's at least 1 MIDI port in each of the MIDI port controls,
   // then set the selection to the first one in the list.
   if (mChoiceMIDIIn->GetCount() > 0) {
      mChoiceMIDIIn->SetSelection(0);
   }
   if (mChoiceMIDIOut->GetCount() > 0) {
      mChoiceMIDIOut->SetSelection(0);
   }

   // Default the server port to 2112 (TODO: Read this from a configuration file)
   mTextCtrlPort->SetValue(_T("2112"));

   /* There are 4 main scenarios this application needs to take into account:
    * - The PC has both MIDI In and Out ports; the app can operate in local, client, or server mode
    * - The PC only has a MIDI In port; the app can only operate in client mode
    * - The PC has MIDI Out but no MIDI In ports; the app can only operate in server mode
    * - The PC has no MIDI ports..  Just exit the app.
    * The last 3 require special action.
    */
   // MIDI In ports exist, but there are no MIDI Out ports - Disable local and server mode
   // radio buttons
   if ((gApp->mMIDIIn->getPortCount() > 0) && (gApp->mMIDIOut->getPortCount() == 0)) {
      ModalMessageBox(this, _T("There are no MIDI output ports; enabling client mode only."),
                      _T("Note"));
      // Set the 'client' mode radio button selected, since it will be the only option
      mRadioClient->SetValue(true);
      mRadioLocal->Enable(false);
      mRadioServer->Enable(false);
      ChangeGUIMode(eClient);
   }
   // MIDI Out ports exist, but there are no MIDI In ports - Disable local and client mode
   // radio buttons
   else if ((gApp->mMIDIIn->getPortCount() == 0) && (gApp->mMIDIOut->getPortCount() > 0)) {
      ModalMessageBox(this, _T("There are no MIDI input ports; enabling server mode only."),
                      _T("Note"));
      // Set the 'host' mode radio button selected, since it will be the only option
      mRadioServer->SetValue(true);
      mRadioLocal->Enable(false);
      mRadioClient->Enable(false);
      ChangeGUIMode(eServer);
   }
   // No MIDI ports at all..  Just exit the app.  Note: This case is already taken
   // care of in the MIDIRouterApp class, but this is here for good measure.
   else if ((gApp->mMIDIIn->getPortCount() == 0) && (gApp->mMIDIOut->getPortCount() == 0)) {
      ModalMessageBox(this, _T("There are no MIDI ports available.  The program will now exit."),
                      _T("Warning"), wxOK | wxICON_EXCLAMATION);
      Close(true);
   }
}


/*!
 * Should we show tooltips?
 */

bool MainFrame::ShowToolTips() {
    return true;
}

void MainFrame::OnFileExit(wxCommandEvent& WXUNUSED(event)) {
	Close(true);
}

void MainFrame::OnHelpAbout(wxCommandEvent& WXUNUSED(event)) {
   wxAboutDialogInfo info;
   info.SetName(_T("MIDI Router"));
   info.SetVersion(_T("1.0 beta"));
   info.SetDescription(_T("Routes MIDI messages from a MIDI In port to a MIDI Out port, or over a network."));
   info.SetCopyright(_T("Copyright (C) 2008 Eric Oulashin <oulashin@hotmail.com>"));
   wxArrayString developers;
   developers.Add(_T("Eric Oulashin"));
   info.SetDevelopers(developers);
   // If compiling for Windows, include the app icon from resources in the dialog box.
   #ifdef __WINDOWS_MM__
   info.SetIcon(wxIcon(_T("WXAPPICON"), wxBITMAP_TYPE_ICO_RESOURCE));
   #endif

   ::wxAboutBox(info);
}

void MainFrame::OnClickLocal(wxCommandEvent& WXUNUSED(event)) {
   ChangeGUIMode(eLocal);
}
   
void MainFrame::OnClickClient(wxCommandEvent& WXUNUSED(event)) {
   ChangeGUIMode(eClient);
}
   
void MainFrame::OnClickServer(wxCommandEvent& WXUNUSED(event)) {
   ChangeGUIMode(eServer);
}

void MainFrame::OnSelectMIDIIn(wxCommandEvent& WXUNUSED(event)) {
   try {
      // If we're currently processing MIDI, then close the current MIDI In port and open
      // the new port.
      if (gApp->mMIDILoopContinue) {
         gApp->mMIDIIn->closePort();
         gApp->mMIDIIn->openPort(mChoiceMIDIIn->GetSelection());
      }
   }
   catch (RtError& pError) {
      ModalRtErrorBox(this, pError);
   }
}

void MainFrame::OnSelectMIDIOut(wxCommandEvent& WXUNUSED(event)) {
   try {
      // If we're currently processing MIDI, then close the current MIDI In port and open
      // the new port.
      if (gApp->mMIDILoopContinue) {
         gApp->mMIDIOut->closePort();
         gApp->mMIDIOut->openPort(mChoiceMIDIIn->GetSelection());
      }
   }
   catch (RtError& pError) {
      ModalRtErrorBox(this, pError);
   }
}

void MainFrame::OnGoButton(wxCommandEvent& WXUNUSED(event)) {
   // Set the app's mMIDILoopContinue member to true so that the the thread loop will run
   gApp->mMIDILoopContinue = true;
   // Disable some critical GUI elements so that the user can't change things while
   // the app is processing MIDI.
   mGoButton->Enable(false);
   EnableServerInput(false);
   EnablePortInput(false);
   EnableMIDIInInput(false);
   EnableMIDIOutInput(false);
   switch (gApp->mMode) {
      case eLocal:
         {
            // Create & run a MIDIThread object for local mode (note: it knows the app mode via the global
            // variable gApp).
            MIDIThread *iThread = new MIDIThread;
            wxThreadError error = iThread->Create();
            if (error == wxTHREAD_NO_ERROR) {
               iThread->Run();
            }
            else {
               delete iThread; // Free the thread object's memory
               // Generate an error message and display it
               wxString errorMsg = _T("Unable to start local mode: ");
               if (error == wxTHREAD_NO_RESOURCE) {
                  errorMsg += _T("Insufficient resources are available to create a new thread.");
               }
               else if (error == wxTHREAD_RUNNING) {
                  // The thread is already running??  This shouldn't happen, but this is here for
                  // the sake of completeness.
                  errorMsg += _T("The thread is already running.");
               }
               wxMessageBox(errorMsg, _T("Error"), wxCENTRE|wxOK|wxICON_ERROR, this);
            }
         }
         break;
      case eServer:
         {
            try {
               // Open the chosen MIDI Out port
               gApp->mMIDIOut->openPort(mChoiceMIDIOut->GetSelection());
               // Get the port from the input box on the GUI
               wxString portStr = mTextCtrlPort->GetValue();
               unsigned long port = 0;
               portStr.ToULong(&port); // This is the closest numeric type conversion
               // Create the address - Defaults to localhost:0 initially
               wxIPV4address addr;
               addr.Service((unsigned short)port);
               mServerSocket = new wxSocketServer(addr);
               if (mServerSocket->Ok()) {
                  // Set up the event handler and subscribe to connection events
                  mServerSocket->SetEventHandler(*this, SERVER_ID);
                  mServerSocket->SetNotify(wxSOCKET_CONNECTION_FLAG);
                  mServerSocket->Notify(true);
                  mStatusBar->SetStatusText(_T("Waiting for client on port ") + portStr + _T("..."));
               }
               else {
                  ModalMessageBox(this, _T("Server unable to listen on port ") + portStr, _T("Error"),
                                  wxOK | wxICON_EXCLAMATION);
                  // Re-enable the Go button so the user can click it again, and make sure the GUI
                  // is set up correctly.
                  mGoButton->Enable(true);
                  SetServerModeGUI();
                  gApp->mMIDIOut->closePort();
               }
            }
            catch (RtError& pError) {
               ModalRtErrorBox(this, pError);
               // Re-enable the Go button so the user can click it again, and make sure the GUI
               // is set up correctly.
               mGoButton->Enable(true);
               SetServerModeGUI();
            }
         }
         break;
      case eClient:
         {
            // Get the host & port from the inputs on the GUI
            wxString hostStr = mTextCtrlServer->GetValue();
            wxString portStr = mTextCtrlPort->GetValue();
            // We need to convert the port to an integer for use with a wxIPV4address
            // object.
            unsigned long port = 0;
            portStr.ToULong(&port); // This is the closest numeric type conversion
            // Create the address - Defaults to localhost:0 initially
            wxIPV4address addr;
            addr.Hostname(hostStr);
            addr.Service((unsigned short)port);

            // Set up the client socket and connect to the specified host
            // using the specified port
            mClientSocket = new wxSocketClient();
            // Set up the event handler and subscribe to most events
            mClientSocket->SetEventHandler(*this, CLIENT_SOCKET_ID);
            mClientSocket->SetNotify(wxSOCKET_CONNECTION_FLAG |
                                     wxSOCKET_INPUT_FLAG |
                                     wxSOCKET_LOST_FLAG);
            mClientSocket->Notify(true);
            mStatusBar->SetStatusText(_T("Connecting to the server..."));
            mClientSocket->Connect(addr, true); // Blocking connect (blocks the GUI)
            //mClientSocket->Connect(addr, false); // Non-blocking connect (don't block the GUI)
            //mClientSocket->WaitOnConnect(10);    // Wait for 10 seconds to connect
            if (mClientSocket->IsConnected()) {
               mStatusBar->SetStatusText(_T("Connected to the server."));
               // Create & run a MIDIThread object for client mode (note: it knows the app mode via the global
               // variable gApp).
               MIDIThread *iThread = new MIDIThread;
               wxThreadError error = iThread->Create();
               if (error == wxTHREAD_NO_ERROR) {
                  iThread->Run();
               }
               else {
                  delete iThread; // Free the thread object's memory
                  // Generate an error message and display it
                  wxString errorMsg = _T("Unable to start client mode: ");
                  if (error == wxTHREAD_NO_RESOURCE) {
                     errorMsg += _T("Insufficient resources are available to create a new thread.");
                  }
                  else if (error == wxTHREAD_RUNNING) {
                     // The thread is already running??  This shouldn't happen, but this is here for
                     // the sake of completeness.
                     errorMsg += _T("The thread is already running.");
                  }
                  wxMessageBox(errorMsg, _T("Error"), wxCENTRE|wxOK|wxICON_ERROR, this);
               }
            }
            else {
               // Uh-oh, we couldn't connect
               // Close & destroy the client socket
               mClientSocket->Notify(false);
               mClientSocket->Close();
               delete mClientSocket;
               mClientSocket = NULL;
               mStatusBar->SetStatusText(_T(""));
               ModalMessageBox(this, _T("Unable to connect to the server"), _T("Error"),
                               wxOK | wxICON_ERROR);
               // Re-enable the Go button so the user can click it again, and make sure the GUI
               // is set up correctly.
               mGoButton->Enable(true);
               SetClientModeGUI();
            }
         }
         break;
   }
}

void MainFrame::OnStopButton(wxCommandEvent& WXUNUSED(event)) {
   StopCurrentMode();
}

void MainFrame::OnServerEvent(wxSocketEvent& event) {
   // Accept new connection if there is one in the pending
   // connections queue, else exit. We use Accept(false) for
   // non-blocking accept (although if we got here, there
   // should ALWAYS be a pending connection).

   if (!(gApp->mServerConnected)) {
      wxSocketBase *sock = mServerSocket->Accept(false);
      if (sock != NULL) {
         sock->SetEventHandler(*this, SERVER_SOCKET_ID);
         sock->SetNotify(wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
         sock->Notify(true);
         gApp->mServerConnected = true;
         mStatusBar->SetStatusText(_T("Client is connected."));
      }
      else {
         ModalMessageBox(this, _T("Error: couldn't accept a connection"), _T("Error"), wxOK | wxICON_ERROR);
      }
   }
}

void MainFrame::OnServerSocketEvent(wxSocketEvent& event) {
   // We'll need the socket from the wxSocketEvent object
   wxSocketBase *sock = event.GetSocket();
   // The app object's mMIDILoopContinue member controls the continuation
   // of the current mode, so if it's true, then go ahead and process the event.
   if (gApp->mMIDILoopContinue) {
      switch(event.GetSocketEvent())
      {
         case wxSOCKET_INPUT:
         {
            // We disable input events, so that the code doesn't trigger
            // wxSocketEvent again.
            sock->SetNotify(wxSOCKET_LOST_FLAG);

            // Read the data from the socket
            // These variables are declared static so that it doesn't get
            // constructed & destructed every time this function is called,
            // which will be many, many times.
            static RtMidiOut *MIDIOut = gApp->mMIDIOut;
            static vector<unsigned char> msgVector; // Will be sent to MIDIOut
            static MIDIMessage iMIDIMessage;
            sock->Read(&iMIDIMessage, sizeof(iMIDIMessage));
            // Insert the MIDI message data into msgVector, and send the vector
            // to gApp->mMIDIOut.
            msgVector.clear();
            for (unsigned int i = 0; i < (unsigned int)iMIDIMessage.numBytes; ++i) {
               msgVector.push_back(iMIDIMessage.message[i]);
            }
            MIDIOut->sendMessage(&msgVector);

            // Enable input events again.
            sock->SetNotify(wxSOCKET_LOST_FLAG | wxSOCKET_INPUT_FLAG);
            break;
         }
         case wxSOCKET_LOST:
         {
            // Destroy() should be used instead of delete wherever possible,
            // due to the fact that wxSocket uses 'delayed events' (see the
            // documentation for wxPostEvent) and we don't want an event to
            // arrive to the event handler (the frame, here) after the socket
            // has been deleted. Also, we might be doing some other thing with
            // the socket at the same time; for example, we might be in the
            // middle of a test or something. Destroy() takes care of all
            // this for us.
            sock->Destroy();
            // Stop the current app mode
            StopCurrentMode();
            break;
         }
         default:
            break;
      }
   }
   else {
      // The app's mMIDILoopContinue member has been set false, which means
      // the user has chosen to stop server mode.  So, destroy the socket and
      // stop server mode.
      sock->Destroy();
      gApp->mMIDIOut->closePort();
      StopCurrentMode();
   }
}

void MainFrame::OnClientSocketEvent(wxSocketEvent& event) {
   switch (event.GetSocketEvent()) {
      case wxSOCKET_INPUT:  // Input on socket..  We're not expecting any input, so nothing to do here.
         break;
      case wxSOCKET_CONNECTION: // Connection established..  We already tried to wait, so nothing to do here.
         break;
      case wxSOCKET_LOST:
         ModalMessageBox(this, _T("Connection to the server was lost"), _T("Error"),
                         wxOK | wxICON_ERROR);
         StopCurrentMode();
         break;
   }
}

void MainFrame::OnClientLocalTooChkbox(wxCommandEvent& event) {
   gApp->mClientLocalToo = mChkBoxLocalToo->GetValue();
   // When in client mode, show or hide the MIDI Out seletion box according
   // to the app's mClientLocalToo variable; otherwise, make sure they're
   // hidden.
   if (gApp->mMode == eClient) {
      EnableMIDIOutInput(gApp->mClientLocalToo);
   }
   else {
      EnableMIDIOutInput(false);
   }
}

void MainFrame::PopulateMIDIIn() {
   // Clear the list in mChoiceMIDIIn and re-populate it
   mChoiceMIDIIn->Clear();
   RtMidiIn *MIDIIn = gApp->mMIDIIn;
   try {
      // Insert the MIDI port names into the control
      unsigned numPorts = MIDIIn->getPortCount();
      for (unsigned i = 0; i < numPorts; ++i) {
         mChoiceMIDIIn->Append(_T(MIDIIn->getPortName(i).c_str()));
      }

      // Call Layout() on mPanel() to make sure the input is wide enough to
      // show the MIDI port names
      mPanel->Layout();
   }
   catch (RtError &pError) {
      ModalRtErrorBox(NULL, pError);
   }
}

void MainFrame::PopulateMIDIOut() {
   // Clear the list in mChoiceMIDIOut and re-populate it
   mChoiceMIDIOut->Clear();
   RtMidiOut *MIDIOut = gApp->mMIDIOut;
   try {
      unsigned numPorts = MIDIOut->getPortCount();
      for (unsigned i = 0; i < numPorts; ++i) {
         mChoiceMIDIOut->Append(_T(MIDIOut->getPortName(i).c_str()));
      }


      // Call Layout() on mPanel() to make sure the input is wide enough to
      // show the MIDI port names
      mPanel->Layout();
   }
   catch (RtError &pError) {
      ModalRtErrorBox(NULL, pError);
   }
}

/*!
 * Get bitmap resources
 */

wxBitmap MainFrame::GetBitmapResource( const wxString& name ) {
    // Bitmap retrieval
////@begin MainFrame bitmap retrieval
   wxUnusedVar(name);
   return wxNullBitmap;
////@end MainFrame bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon MainFrame::GetIconResource( const wxString& name ) {
    // Icon retrieval
////@begin MainFrame icon retrieval
   wxUnusedVar(name);
   return wxNullIcon;
////@end MainFrame icon retrieval
}

void MainFrame::ChangeGUIMode(modes pMode) {
   StopCurrentMode();

   gApp->mMode = pMode;
   switch (pMode) {
      case eLocal: // Local mode: Hide network connection settings, show MIDI In & MIDI Out choice boxes
         SetLocalModeGUI();
         break;
      case eClient: // Client mode: Change go button text, enable network settings settings, show MIDI In choice box, hide MIDI Out choice box
         SetClientModeGUI();
         break;
      case eServer: // Server mode: Change go button text, enable network settings settings, but hide server input, show MIDI Out choice box hide MIDI In choice box
         SetServerModeGUI();
         break;
   }
}

void MainFrame::StopCurrentMode() {
   // By setting the app's mMIDILoopContinue to false, the thread loop will stop.
   gApp->mMIDILoopContinue = false;
   // Based on the app mode, stop the proper thread/action, and enable/disable the appropriate
   // GUI elements.
   switch (gApp->mMode) {
      case eLocal: // Local mode
         SetLocalModeGUI();
         break;
      case eServer: // Server mode - Stop mServerSocket from listening
         // mServerSocket shouldn't be NULL, but check just in case
         if (mServerSocket != NULL) {
            mServerSocket->Notify(false);
            mServerSocket->Close();
            delete mServerSocket;
            mServerSocket = NULL;
            gApp->mServerConnected = false;
         }
         mStatusBar->SetStatusText(_T("Server mode stopped."));
         SetServerModeGUI();
         break;
      case eClient: // Client mode
         // mClientSocket shouldn't be NULL, but check just in case
         if (mClientSocket != NULL) {
            mClientSocket->Notify(false);
            mClientSocket->Close();
            delete mClientSocket;
            mClientSocket = NULL;
         }
         mStatusBar->SetStatusText(_T("Client mode stopped."));
         SetClientModeGUI();
         break;
   }

   mGoButton->Enable(true);

   // Make sure the MIDI ports are closed
   try {
      gApp->mMIDIIn->closePort();
      gApp->mMIDIOut->closePort();
   }
   catch (RtError& WXUNUSED(pError)) {
   }
}

void MainFrame::EnableServerInput(bool pEnable) {
   mTextCtrlServer->Enable(pEnable);
   mStaticTextServer->Enable(pEnable);
}

void MainFrame::EnablePortInput(bool pEnable) {
   mTextCtrlPort->Enable(pEnable);
   mStaticTextPort->Enable(pEnable);
}

void MainFrame::EnableMIDIInInput(bool pEnable) {
   mChoiceMIDIIn->Enable(pEnable);
   mStaticTextMIDIIn->Enable(pEnable);
}

void MainFrame::EnableMIDIOutInput(bool pEnable) {
   mChoiceMIDIOut->Enable(pEnable);
   mStaticTextMIDIOut->Enable(pEnable);
}

void MainFrame::SetLocalModeGUI() {
   mGoButton->SetLabel(_T("Start"));
   EnableServerInput(false);
   EnablePortInput(false);
   EnableMIDIInInput(true);
   EnableMIDIOutInput(true);
   mChkBoxLocalToo->Enable(false);
}

void MainFrame::SetServerModeGUI() {
   mGoButton->SetLabel(_T("Host"));
   EnableServerInput(false);
   EnablePortInput(true);
   EnableMIDIInInput(false);
   EnableMIDIOutInput(true);
   mChkBoxLocalToo->Enable(false);
}

void MainFrame::SetClientModeGUI() {
   mGoButton->SetLabel(_T("Connect"));
   EnableServerInput(true);
   EnablePortInput(true);
   EnableMIDIInInput(true);
   EnableMIDIOutInput(gApp->mClientLocalToo);
   mChkBoxLocalToo->Enable(true);
}