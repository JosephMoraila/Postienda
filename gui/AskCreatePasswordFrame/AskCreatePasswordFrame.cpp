#include "gui/AskCreatePasswordFrame/AskCreatePasswordFrame.hpp"

AskCreatePasswordFrame::AskCreatePasswordFrame(wxWindow* parent, const wxString& title) : wxDialog(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize) {
	Widgets();
}
