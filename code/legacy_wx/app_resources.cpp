/////////////////////////////////////////////////////////////////////////////
// Name:        
// Purpose:     
// Author:      Eric Oulashin
// Modified by: 
// Created:     28/03/2008 23:08:10
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

#include "app_resources.h"

////@begin XPM images
////@end XPM images

/*!
 * Resource functions
 */

////@begin AppResources resource functions

/*!
 * Menubar creation function for ID_MENUBAR
 */

wxMenuBar* AppResources::CreateMenuMenubar()
{
   wxMenuBar* menuBar = new wxMenuBar;
   return menuBar;
}

////@end AppResources resource functions

/*!
 * Get bitmap resources
 */

wxBitmap AppResources::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin AppResources bitmap retrieval
   wxUnusedVar(name);
   return wxNullBitmap;
////@end AppResources bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon AppResources::GetIconResource( const wxString& name )
{
    wxUnusedVar(name);
    // Icon retrieval
////@begin AppResources icon retrieval
   wxUnusedVar(name);
   return wxNullIcon;
////@end AppResources icon retrieval
}
