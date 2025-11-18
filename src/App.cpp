#include <wx/wx.h>
#include <wx/intl.h>
#include <wx/filefn.h>
#include <locale.h>
#include <fstream>
#include "src/App.h"
#include "constants/APPNAME/APPNAME.h"
#include "third_party/json.hpp"
#include "constants/JSON/Language/LanguageJson.hpp"
#include "utils/CreateFileFolder.hpp"
#include "constants/FOLDERS/SETTINGS/SETTINGS_FOLDER.hpp"
#include "constants/FOLDERS/LOCALE/LOCALE_FOLDER.hpp"
#ifdef _WIN32
#include <windows.h>
#endif


using json = nlohmann::json;

wxIMPLEMENT_APP(App);

bool App::OnInit() {
    //Verify if an instance already exists
#ifdef _WIN32
    HWND hWnd = FindWindowA(nullptr, APPNAME.c_str());
    if (hWnd != nullptr)
    {
        ShowWindow(hWnd, SW_RESTORE);
        SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0,SWP_NOMOVE | SWP_NOSIZE);
        SetForegroundWindow(hWnd);
        return false; // Cerrar esta instancia
    }
#endif
    m_checker = new wxSingleInstanceChecker(APPNAME);
    if (m_checker->IsAnotherRunning()) {
        FocusMainWindow();
        return false; // Silenciar, no mostrar nada
    }
    

    CreateFolder::CreateSettingsFolder();
    ::setlocale(LC_ALL, "");

wxLanguage langToTry = wxLANGUAGE_DEFAULT;
bool localeLoaded = false;

// --------------------
// Leer idioma del JSON
// --------------------
std::string jsonPath = GetJsonLanguagePath();
if (wxFileExists(wxString::FromUTF8(jsonPath))) {
    try {
        std::ifstream file(jsonPath);
        json langData;
        file >> langData;
        file.close();

        wxString lang = wxString::FromUTF8(langData.value("language", "Default").c_str());
        InitLanguage(lang, langToTry);
    }
    catch (...) {
        langToTry = wxLANGUAGE_DEFAULT;
    }
}

// --------------------------------------
// Intentar solo UNA inicialización
// --------------------------------------
if (!m_locale.Init(langToTry)) {
    // idioma elegido no existe → intentamos inglés
    if (!m_locale.Init(wxLANGUAGE_ENGLISH)) {
        // inglés tampoco existe → fallback a texto del programa
        // NO usar más m_locale
        goto fallback_no_locale;
    }
}

// --------------------------------------
// Configurar rutas y cargar catálogo
// --------------------------------------
{
    wxString folders = wxString::Format("%s%s",
        SETTINGS_FOLDER_PATH,
        LOCALE_FOLDER_PATH
    );

    m_locale.AddCatalogLookupPathPrefix(folders);

    localeLoaded = m_locale.AddCatalog("messages");
}

// --------------------------------------
// Si no carga catálogo, usar fallback
// --------------------------------------
if (!localeLoaded) {
fallback_no_locale:
    // No usar ningún catálogo → textos por defecto del código
    // No hay nada más que hacer
    ;
}

    wxInitAllImageHandlers();

    // Mostrar ventana principal
    m_mainFrame = new MainFrame(APPNAME);
    m_mainFrame->Maximize(true);
    m_mainFrame->Center();
    m_mainFrame->Show();

    return true;
}


void App::InitLanguage(wxString& lang, wxLanguage& langToTry) {

    wxString spanish = wxString(L"Español");

    if (lang == spanish) langToTry = wxLANGUAGE_SPANISH;
    else if (lang == "English" || lang == "en" || lang == "en_US") langToTry = wxLANGUAGE_ENGLISH;
    else langToTry = wxLANGUAGE_DEFAULT;
}

void App::FocusMainWindow() {
    if (m_mainFrame) {
        m_mainFrame->Raise();               
        m_mainFrame->RequestUserAttention();
    }
}