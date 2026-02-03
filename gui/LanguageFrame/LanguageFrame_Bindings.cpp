#include "gui/LanguageFrame/LanguageFrame.hpp"
#include "src/App.h"
#include "third_party/json.hpp"
#include <wx/filefn.h> // Para wxMkdir, wxFileExists
#include <fstream>
#include "constants/JSON/Language/LanguageJson.hpp"
#include "constants/FOLDERS/SETTINGS/SETTINGS_FOLDER.hpp"
#include "constants/FOLDERS/LOCALE/LOCALE_FOLDER.hpp"

using json = nlohmann::json;

void LanguageFrame::OnAcccept(wxCommandEvent& event) {
    int sel = languagePicker->GetSelection();
    wxString lang = languages[sel];

    // Asegurar que exista el directorio locale/
    wxString Folder;
    Folder << SETTINGS_FOLDER_PATH() << wxFileName::GetPathSeparator() << LOCALE_FOLDER_PATH;
    if (!wxDirExists(Folder)) wxMkdir(Folder);

    std::string path = GetJsonLanguagePath();
    
    // Crear el JSON
    json langData;
    langData["language"] = std::string(lang.utf8_str());

    // Guardar en archivo locale/language.json
    std::ofstream file(path);
    if (file.is_open()) {
        file << langData.dump(4); // con indentación de 4 espacios
        file.close();
    }
    else {
        wxMessageBox(_("Could not write: ")+ wxString::FromUTF8(path.c_str()), _("Error"), wxOK | wxICON_ERROR, this);
        return;
    }

    wxMessageBox(_("The language change will take effect after restarting the application."),_("Information"),wxOK | wxICON_INFORMATION,this);

    this->EndModal(wxID_OK);
}
