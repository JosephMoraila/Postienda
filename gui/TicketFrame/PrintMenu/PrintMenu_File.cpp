#include "gui/TicketFrame/PrintMenu/PrintMenu.hpp"
#include "constants/JSON/Ticket/PRINTER_SETTINGS.hpp"
#include "constants/TICKET/TICKET_PATH.hpp"
#include <wx/mstream.h>  
#include <wx/zipstrm.h>      // Para wxZipOutputStream y wxZipInputStream
#include <wx/wfstream.h>     // Para wxFileOutputStream y wxFileInputStream
#include <wx/sstream.h>      // Para wxStringOutputStream
#include "third_party/json.hpp"
#include <fstream>

using json = nlohmann::json;

void PrintMenu::OnCloseSave(wxCloseEvent& event) {
    std::string printerName = std::string(savedPrinter.mb_str());
    bool isChecked = checkboxPrintTicket->IsChecked();

    // Archivo donde guardaremos la configuraciÃÂ³n
    std::string filename = PRINT_SETTINGS_PATH;

    // Cargar contenido existente
    json j;
    std::ifstream infile(filename);
    if (infile.is_open()) {
        try {
            infile >> j;
        }
        catch (...) {
            // Si hay error al leer JSON, iniciamos uno vacÃÂ­o
            j = json::object();
        }
        infile.close();
    }

    j["printer"] = printerName;
    j["isPrint"] = isChecked;

    // Guardar de nuevo el JSON
    std::ofstream outfile(filename);
    if (outfile.is_open()) {
        outfile << j.dump(4); // 4 espacios de indentaciÃÂ³n para legibilidad
        outfile.close();
    }
    event.Skip(); // Permite que la ventana realmente se cierre
}

void PrintMenu::SaveSettingsPrinterName()
{
    // Convertir wxString a std::string
    std::string printerName = std::string(savedPrinter.mb_str());


    // Archivo donde guardaremos la configuraciÃÂ³n
    std::string filename = PRINT_SETTINGS_PATH;

    // Cargar contenido existente
    json j;
    std::ifstream infile(filename);
    if (infile.is_open()) {
        try {
            infile >> j;
        }
        catch (...) {
            // Si hay error al leer JSON, iniciamos uno vacÃÂ­o
            j = json::object();
        }
        infile.close();
    }

    // Guardar el valor del printer en un campo especÃÂ­fico
    j["printer"] = printerName;

    // Guardar de nuevo el JSON
    std::ofstream outfile(filename);
    if (outfile.is_open()) {
        outfile << j.dump(4); // 4 espacios de indentaciÃÂ³n para legibilidad
        outfile.close();
    }
}

void PrintMenu::SaveSettingsPrinterCheckbox(wxCommandEvent& event) {
    bool isChecked = event.IsChecked();
    // Archivo donde guardaremos la configuraciÃÂ³n
    std::string filename = PRINT_SETTINGS_PATH;

    // Cargar contenido existente
    json j;
    std::ifstream infile(filename);
    if (infile.is_open()) {
        try {
            infile >> j;
        }
        catch (...) {
            // Si hay error al leer JSON, iniciamos uno vacÃÂ­o
            j = json::object();
        }
        infile.close();
    }

    // Guardar el valor del printer en un campo especÃÂ­fico
    j["isPrint"] = isChecked;

    // Guardar de nuevo el JSON
    std::ofstream outfile(filename);
    if (outfile.is_open()) {
        outfile << j.dump(4); // 4 espacios de indentaciÃÂ³n para legibilidad
        outfile.close();
    }
}

void PrintMenu::LoadSettingsPrinterCheckbox() {
    std::string filename = PRINT_SETTINGS_PATH;
    std::ifstream infile(filename);
    if (!infile.is_open()) checkboxPrintTicket->SetValue(false);
    try {
        json j;
        infile >> j;
        infile.close();

        // Verificar si existe el campo "printer"
        if (j.contains("isPrint") && j["isPrint"].is_boolean()) {
            bool isPrint = j["isPrint"];
            checkboxPrintTicket->SetValue(isPrint);
        }
        else checkboxPrintTicket->SetValue(false); // No hay campo "isPrinte"
    }
    catch (...) {
        // Error al leer JSON
        checkboxPrintTicket->SetValue(false);
    }
}


