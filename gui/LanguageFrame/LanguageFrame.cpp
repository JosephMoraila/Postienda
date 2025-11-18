#include <wx/wx.h>
#include "gui/LanguageFrame/LanguageFrame.hpp"

// ---------------- LanguageFrame.cpp ----------------
LanguageFrame::LanguageFrame(wxWindow* parent, const wxString& title) : wxDialog(parent, wxID_ANY, title, wxDefaultPosition, wxSize(300, 200))
{
	Widgets();
}