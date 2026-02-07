#include <wx/wx.h>
#include "gui/LanguageFrame/LanguageFrame.hpp"
#include "utils/window/WindowUtils.h"

// ---------------- LanguageFrame.cpp ----------------
LanguageFrame::LanguageFrame(wxWindow* parent, const wxString& title) : wxDialog(parent, wxID_ANY, title, wxDefaultPosition, wxSize(300, 200))
{
	AplicarIconoPrincipal(this);
	Widgets();
}