#include "gui/AskCreatePasswordFrame/AskCreatePasswordFrame.hpp"
#include "utils/window/WindowUtils.h"

AskCreatePasswordFrame::AskCreatePasswordFrame(wxWindow* parent, const wxString& title) : wxDialog(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize) {
	AplicarIconoPrincipal(this);
	Widgets();
}
