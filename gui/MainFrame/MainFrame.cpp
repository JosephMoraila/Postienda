#include "gui/MainFrame/MainFrame.h"
#include "gui/UsuariosFrame/UsuariosFrame.h"
#include <wx/wx.h>
#include <wx/textfile.h> 
#include <wx/grid.h>
#include "gui/ProductosFrame/ProductosFrame.h"
#include "constants/JSON/DarkMode/DarkMode.h"
#include "constants/JSON/Session/Session.h"
#include "constants/APPNAME/APPNAME.h"
#include "utils/ValidateStringInput.h"
#include "utils/FuenteUtils.h"
#include "utils/GetFromFile.h"
#include <fstream>
#include <json.hpp>
#include "utils/window/WindowUtils.h"
using json = nlohmann::json;


MainFrame::MainFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title)
{
    MenuBar();
    Widgets();
	CreateJSONTemaOscuro(); // Carga el estado del tema (o crea el archivo), tiene que ir primero porque crea o le el JSON que contendrÃÂÃÂ¡ si estamos en blanco o negro
    AplicarTemaActual();    // Aplica el tema segÃÂÃÂºn el valor leÃÂÃÂ­do, tiene que ir despues de funciÃÂÃÂ³n CreateJSONTemaOscuro para que se aplique el tema al iniciar la ventana
    // Bind resize event
    Bind(wxEVT_SIZE, &MainFrame::OnSize, this);
    // Ajustar columnas despuÃÂÃÂ©s de que la ventana estÃÂÃÂ© completamente creada

    GetActualUserFromJSON();
    AjustarColumnasListCtrl();
    CartHasProducts();
    Bind(wxEVT_CLOSE_WINDOW, &MainFrame::OnClose, this);
}


std::vector<size_t> MainFrame::ReturnSelectedItemsControlList() {
    std::vector<size_t> selectedIds;
    long selected = -1; // -1 significa que no hay selecciÃÂÃÂ³n inicial
    while (true) {
        selected = listaProductos->GetNextItem(selected, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        if (selected == -1) break; // ya no hay mÃÂÃÂ¡s seleccionados
        wxString idStr = listaProductos->GetItemText(selected, 3); // Obtener ID (columna 3)
        unsigned long tempId = 0;
        if (idStr.ToULong(&tempId)) {
            size_t id = static_cast<size_t>(tempId);
            selectedIds.push_back(id);
        }
    }
    return selectedIds;
}


void MainFrame::AplicarTema(bool oscuro) {
    temaOscuro = oscuro;

    // Aplicar tema a todos los widgets del MainFrame
    ApplyTheme(this, oscuro);
	ApplyTheme(mainPanel, oscuro);

    // Aplicar tema a las ventanas hijas si existen
    if (productosVentana) productosVentana->AplicarTema(oscuro);
    if (usuariosVentana) usuariosVentana->AplicarTema(oscuro);
    if (ticketVentana) ticketVentana->AplicarTema(oscuro);
    if (infoProductsVentana) infoProductsVentana->AplicarTema(oscuro);
    if (previousPurchasesVentana) {
        previousPurchasesVentana->AplicarTema(oscuro);
        //Apply theme to previousPurchasesVentana children windows
        for (auto& [id, windowPtr] : previousPurchasesVentana->purchaseWindows) {
            if (windowPtr) windowPtr->AplicarTema(oscuro); 
        }
    }
	if (LanguageWindow) LanguageWindow->AplicarTema(oscuro);
    Refresh();
    Update();
}

void MainFrame::CreateJSONTemaOscuro() {
    std::ifstream archivoIn(JSON_DARKMODE_PATH);
    json configJson;

    if (archivoIn && archivoIn >> configJson &&
        configJson.contains("modo_oscuro") && configJson["modo_oscuro"].is_boolean())
    {
        // leer preferencia del usuario
        temaOscuro = configJson["modo_oscuro"];

        // leer si el JSON ya contenÃÂÃÂ­a la preferencia "usar_modo_sistema"
        if (configJson.contains("usar_modo_sistema") && configJson["usar_modo_sistema"].is_boolean()) {
            usarModoSistema = configJson["usar_modo_sistema"];
        }
        else {
            // por defecto no seguir sistema
            usarModoSistema = false;
            configJson["usar_modo_sistema"] = usarModoSistema;
        }

        // aseguramos que el estado actual del sistema quede guardado

        std::ofstream archivoOut(JSON_DARKMODE_PATH);
        archivoOut << std::setw(4) << configJson << std::endl;
    }
    else {
        // archivo no existe o mal formado -> inicializar por defecto
        temaOscuro = false;
        usarModoSistema = false;
        json nuevoJson = {
            {"modo_oscuro", temaOscuro},
            {"usar_modo_sistema", usarModoSistema},
        };
        std::ofstream archivoOut(JSON_DARKMODE_PATH);
        archivoOut << std::setw(4) << nuevoJson << std::endl;
    }
}

void MainFrame::AplicarTemaActual()
{
    bool temaEfectivo = false; // false = claro por defecto
    std::ifstream archivoIn(JSON_DARKMODE_PATH);

    if (archivoIn.is_open()) {
        try {
            json jsonThemeMode;
            archivoIn >> jsonThemeMode;

            if (jsonThemeMode.contains("modo_oscuro") && jsonThemeMode["modo_oscuro"].is_boolean() &&
                jsonThemeMode.contains("usar_modo_sistema") && jsonThemeMode["usar_modo_sistema"].is_boolean())
            {
                bool usarSistema = jsonThemeMode["usar_modo_sistema"].get<bool>();

                if (usarSistema) {
                    // Leer del sistema
                    temaEfectivo = IsDarkModeSystem();
                }
                else {
                    // Usar valor guardado en JSON
                    temaEfectivo = jsonThemeMode["modo_oscuro"].get<bool>();
                }
            }
        }
        catch (...) {
            temaEfectivo = false; // fallback
        }
    }

    // Guardar estado actual
    temaOscuro = temaEfectivo;

    // Aplicar a todos los widgets
    AplicarTema(temaEfectivo);
    ActualizarMenuApariencia();
}

void MainFrame::GuardarConfiguracion() {
    json configJson = {
        {"modo_oscuro", temaOscuro},
        {"usar_modo_sistema", usarModoSistema},
    };
    std::ofstream archivoOut(JSON_DARKMODE_PATH);
    archivoOut << std::setw(4) << configJson << std::endl;
}


void MainFrame::GetActualUserFromJSON() {
    wxString usuario = getUserFromJSON<wxString>();
    if (usuario != "Ninguno") {
		this->SetTitle(APPNAME + " <" + usuario + ">");
    }
}

bool MainFrame::IsDarkModeSystem() {
#ifdef _WIN32
    return IsDarkModeSystemWindows();
#elif __linux__
    return IsDarkModeSystemLinux();
#else
    return false;
#endif
}

#ifdef _WIN32
bool MainFrame::IsDarkModeSystemWindows()
{
    // DWORD = Double Word, entero sin signo de 32 bits
    DWORD value = 0;
    // Inicializamos value en 0 para evitar valores basura antes de leer del registro.

    DWORD size = sizeof(value);
    // TamaÃÂÃÂ±o del buffer para RegQueryValueEx. sizeof(value) = 4 bytes (DWORD).

    HKEY hKey;
    // Handle (puntero/identificador) de la clave del registro de Windows.

    // Abrir la clave de registro que contiene la configuraciÃÂÃÂ³n de tema
    if (RegOpenKeyEx(
        HKEY_CURRENT_USER, // rama del usuario actual
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", // ruta de la clave
        0,                  // opciones reservadas (siempre 0)
        KEY_READ,           // permisos de lectura
        &hKey               // puntero donde se almacenarÃÂÃÂ¡ el handle de la clave
    ) == ERROR_SUCCESS)   // ERROR_SUCCESS = 0 indica que la clave se abriÃÂÃÂ³ correctamente
    {
        // Leer el valor "AppsUseLightTheme" dentro de la clave
        RegQueryValueEx(
            hKey,             // handle de la clave abierta
            L"AppsUseLightTheme", // nombre del valor a leer
            nullptr,          // reservado, siempre nullptr
            nullptr,          // tipo de dato del valor (no nos interesa aquÃÂÃÂ­)
            (LPBYTE)&value,   // puntero a buffer donde se almacenarÃÂÃÂ¡ el valor (cast a LPBYTE)
            &size             // tamaÃÂÃÂ±o del buffer
        );

        // Cerrar la clave del registro para liberar recursos
        RegCloseKey(hKey);

        // Si el valor leÃÂÃÂ­do es 0 -> modo oscuro; si es 1 -> modo claro
        return value == 0;
    }
}


WXLRESULT MainFrame::MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
    // nMsg = mensaje que Windows envÃÂÃÂ­a a la ventana
    // wParam / lParam = parÃÂÃÂ¡metros adicionales dependientes del mensaje

    // Verifica si el mensaje recibido es WM_SETTINGCHANGE
    // WM_SETTINGCHANGE se envÃÂÃÂ­a cuando se cambian configuraciones del sistema (tema, color, etc.)
    if (nMsg == WM_SETTINGCHANGE)
    {
        // lParam contiene un puntero a una cadena con la secciÃÂÃÂ³n de la configuraciÃÂÃÂ³n que cambiÃÂÃÂ³
        LPCWSTR section = reinterpret_cast<LPCWSTR>(lParam);

        // Compara la secciÃÂÃÂ³n con "ImmersiveColorSet", que indica cambio de tema (claro/oscuro) en Windows 10/11
        if (section && wcscmp(section, L"ImmersiveColorSet") == 0)
        {
            // Solo aplicamos cambios si la app estÃÂÃÂ¡ configurada para seguir el modo del sistema
            if (usarModoSistema) {
                // Consulta si actualmente el sistema estÃÂÃÂ¡ en modo oscuro
                bool darkMode = IsDarkModeSystem();

                // Aplica el tema correspondiente a toda la aplicaciÃÂÃÂ³n
                AplicarTema(darkMode);
            }
        }
    }

    // Llamada al procedimiento de ventana base de wxWidgets para procesar mensajes estÃÂÃÂ¡ndar
    return wxFrame::MSWWindowProc(nMsg, wParam, lParam);
}
#endif

#if __linux__
bool MainFrame::IsDarkModeSystemLinux()
{
    wxArrayString output;

    // ===== GNOME, Cinnamon, MATE, Budgie =====
    long code = wxExecute("gsettings get org.gnome.desktop.interface color-scheme", output);
    if (code == 0 && !output.IsEmpty())
    {
        wxString v = output[0].Lower();
        if (v.Contains("dark"))
            return true;
        if (v.Contains("light"))
            return false;
    }

    output.Clear();
    code = wxExecute("gsettings get org.gnome.desktop.interface gtk-theme", output);
    if (code == 0 && !output.IsEmpty())
    {
        wxString theme = output[0].Lower();
        if (theme.Contains("dark"))
            return true;
    }

    // ===== KDE Plasma =====
    wxString kde = wxGetHomeDir() + "/.config/kdeglobals";
    if (wxFileExists(kde))
    {
        wxTextFile f(kde);
        if (f.Open())
        {
            for (size_t i = 0; i < f.GetLineCount(); i++)
            {
                wxString l = f[i].Lower();
                if (l.Contains("[colors:window]")) 
                {
                    // KDE usa valores RGB, buscar en esta sección
                    for (size_t j = i + 1; j < f.GetLineCount(); j++)
                    {
                        wxString line = f[j].Lower();
                        if (line.StartsWith("[")) break; // Nueva sección
                        if (line.Contains("backgroundnormal") || line.Contains("backcolor"))
                        {
                            // Colores oscuros típicamente < RGB(128, 128, 128)
                            if (line.Contains("20,20,20") || line.Contains("30,30,30") || 
                                line.Contains("25,25,25") || line.Contains("35,35,35"))
                                return true;
                        }
                    }
                }
            }
            f.Close();
        }
    }

    // ===== LXQt =====
    wxString lxqt = wxGetHomeDir() + "/.config/lxqt/session.conf";
    if (wxFileExists(lxqt))
    {
        wxTextFile f(lxqt);
        if (f.Open())
        {
            for (size_t i = 0; i < f.GetLineCount(); i++)
            {
                wxString l = f[i].Lower();
                if (l.Contains("theme") && l.Contains("dark"))
                    return true;
            }
            f.Close();
        }
    }

    return false;
}
#endif