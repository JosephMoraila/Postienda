#pragma once
#include <wx/wx.h>
#include "constants/HASH/HASH.hpp"

class AskEnterPasswordFrame : public wxDialog {
public:
	AskEnterPasswordFrame(wxWindow* parent, const wxString& title = _("Enter password"));
	void AplicarTema(bool oscuro);

private:
	bool temaOscuro = false; ///< Current theme state (true = dark, false = light).
	void Widgets();
	wxPanel* mainPanel = nullptr; ///< Main panel containing all widgets.
	wxTextCtrl* textInputPassword = nullptr;
	wxStaticText* messageOnEnter = nullptr;

	HASH::PasswordHash loadUserPassword();
	void OnPasswordEnter(wxCommandEvent& event);
};
