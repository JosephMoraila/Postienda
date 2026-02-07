#include "gui/AskEnterPasswordFrame/AskEnterPasswordFrame.hpp"
#include "utils/window/WindowUtils.h"

AskEnterPasswordFrame::AskEnterPasswordFrame(wxWindow* parent, const wxString& title) : wxDialog(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize) {
	AplicarIconoPrincipal(this);
	Widgets();
}

