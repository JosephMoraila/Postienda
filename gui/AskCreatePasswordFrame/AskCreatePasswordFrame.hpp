#pragma once
#include <wx/wx.h>

class AskCreatePasswordFrame : public wxDialog {
public:
	AskCreatePasswordFrame(wxWindow* parent, const wxString& title = _("Create password"));
	void AplicarTema(bool oscuro);

private:
	bool temaOscuro = false; ///< Current theme state (true = dark, false = light).
	void Widgets();
	wxPanel* mainPanel = nullptr; ///< Main panel containing all widgets.
	wxTextCtrl* textInputPassword = nullptr;
	wxTextCtrl* textInputConfirmPassword = nullptr;
	wxStaticText* messageOnEnter = nullptr;

	void OnPasswordEnter(wxCommandEvent& event);

	/**
	 * @brief Saves the user password to a .dat file
	 * \param saltHex: Salt hex to store to the file
	 * \param hashHex Hash password to store to file
	 */
	void saveUserPassword(const std::string& saltHex, const std::string& hashHex);
};
