#include "gui/AskEnterPasswordFrame/AskEnterPasswordFrame.hpp"

AskEnterPasswordFrame::AskEnterPasswordFrame(wxWindow* parent, const wxString& title) : wxDialog(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize) {
	Widgets();
}

