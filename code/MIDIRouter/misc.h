#ifndef __MISC_H__
#define __MISC_H__
// This file defines some misc. functions for the MIDI Router application.

#include <string>
#include "wx/string.h"
#include "wx/window.h"
#include "RtError.h"
#include "modes.h"

// Displays a modal message box.
//
// Parameters:
//  pParentWin: A pointer to the parent window.  Can be NULL.
//  pMessage: The message to display in the window
//  pCaption: The caption to display in the window
//  pStyle: The style flag for the window.  Defaults to wxOK.
void ModalMessageBox(wxWindow *pParentWin, const wxString& pMessage, const wxString& pCaption = _T("Message box"), long pStyle = wxOK);

// Displays a modal message box with an error message from an RtError object.
//
// Parameters:
//  pParentWin: A pointer  to the parent window.  Can be NULL.
//  pRtError: An RtError object that contains an error message to display
void ModalRtErrorBox(wxWindow *pParentWin, RtError& pRtError);

// Returns a string version of a mode value.
//
// Parameters:
//  pMode: A value from the modes enumeration
//
// Return: A string version of the mode value
wxString appModeToString(modes pMode);

#endif