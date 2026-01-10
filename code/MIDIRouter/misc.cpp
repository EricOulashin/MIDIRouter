// This file defines some misc. functions for the MIDI Router project.
#pragma warning (disable: 4996) // "localtime function unsafe" message

#include "misc.h"
#include "wx/msgdlg.h"

void ModalMessageBox(wxWindow *pParentWin, const wxString& pMessage, const wxString& pCaption, long pStyle) {
   wxMessageDialog dlg(pParentWin, pMessage, pCaption, pStyle);
   dlg.ShowModal();
}

void ModalRtErrorBox(wxWindow *pParentWin, RtError& pRtError) {
   wxMessageDialog dlg(pParentWin, _T(pRtError.getMessageString()), _T("Error"), wxOK | wxICON_ERROR);
   dlg.ShowModal();
}

wxString appModeToString(modes pMode) {
   wxString retval;

   switch (pMode) {
      case eLocal:
         retval = "eLocal";
         break;
      case eClient:
         retval = "eClient";
         break;
      case eServer:
         retval = "eServer";
         break;
   }

   return(retval);
}