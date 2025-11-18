#pragma once
#include <wx/wx.h>
#include "constants/JSON/Language/LanguageJson.hpp"

class LanguageFrame : public wxDialog
{
public:
		LanguageFrame(wxWindow* parent, const wxString& title = _("Language"));
		void AplicarTema(bool oscuro);
private:
	bool temaOscuro = false; ///< Current theme state (true = dark, false = light).
	void Widgets();
	wxPanel* mainPanel = nullptr; ///< Main panel containing all widgets.
	wxChoice* languagePicker = nullptr; ///< Choice widget for selecting language.
	wxArrayString languages; ///Lnaguages array to display available languages
	 /**
		 @brief  Get the selected language from JSON file
		 @retval  - Selected language, if error or json does not exist Default
	 **/
	wxString GetSelectedLanguage() const; 

	/// @name Bindings
	///@{

	 /**
		 @brief Save the selected language to json
		 @param event - Event when pressing button
	 **/
	void OnAcccept(wxCommandEvent& event); 

	///@}

};