#include "gui/LanguageFrame/LanguageFrame.hpp"
#include "utils/window/WindowUtils.h"
#include <json.hpp>
#include <wx/filefn.h>
#include <fstream>

using json = nlohmann::json;

wxString LanguageFrame::GetSelectedLanguage() const {
    wxString selectedLanguage = "Default";
    std::string jsonPath = GetJsonLanguagePath();

    if (wxFileExists(wxString::FromUTF8(jsonPath.c_str()))) {
        try {
            std::ifstream file(jsonPath);
            json langData;
            file >> langData;
            file.close();

            std::string langUtf8 = langData.value("language", "Default");

            wxString lang = wxString::FromUTF8(langUtf8.c_str());

            wxString spanish = wxString(L"Español");

            if (lang == spanish) {
                selectedLanguage = L"Español";
            }
            else if (lang == "English") {
                selectedLanguage = "English";
            }
            else {
                selectedLanguage = "Default";
            }
        }
        catch (...) {
            selectedLanguage = "Default";
        }
    }

    return selectedLanguage;
}


void LanguageFrame::Widgets() {
    mainPanel = new wxPanel(this, wxID_ANY);

    wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

    wxStaticText* label = new wxStaticText(mainPanel, wxID_ANY, _("Select your language:"));

    this->languages.Add("Default");
    this->languages.Add("English");
    this->languages.Add(L"Español");

    wxString selectedLang = GetSelectedLanguage();

    languagePicker = new wxChoice(mainPanel, wxID_ANY, wxDefaultPosition, wxSize(150, -1), languages);

    int selIndex = languages.Index(selectedLang);
    if (selIndex != wxNOT_FOUND) languagePicker->SetSelection(selIndex);
    else languagePicker->SetSelection(0); // Default if not found

    wxButton* okButton = new wxButton(mainPanel, wxID_ANY, _("Accept"), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    okButton->Bind(wxEVT_ENTER_WINDOW, [this](wxMouseEvent& event) { OnWidgetEnter(event, temaOscuro); });
    okButton->Bind(wxEVT_LEAVE_WINDOW, [this](wxMouseEvent& event) { OnWidgetLeave(event, temaOscuro); });
    okButton->Bind(wxEVT_BUTTON, &LanguageFrame::OnAcccept, this);

    vbox->Add(label, 0, wxALL | wxALIGN_CENTER, 10);
    vbox->Add(languagePicker, 0, wxALL | wxALIGN_CENTER, 10);
    vbox->Add(okButton, 0, wxALL | wxALIGN_CENTER, 10);

    mainPanel->SetSizer(vbox);
    this->Centre();
}


void LanguageFrame::AplicarTema(bool oscuro) {
	temaOscuro = oscuro;
	ApplyTheme(mainPanel, temaOscuro);
}