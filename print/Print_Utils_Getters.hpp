#pragma once
#include "print/Canvas/CanvasItem/CanvasItem.hpp"
#include "constants/JSON/Ticket/PRINTER_SETTINGS.hpp"
#include "constants/TICKET/TICKET_PATH.hpp"
#include <wx/mstream.h>  
#include <wx/zipstrm.h>      // Para wxZipOutputStream y wxZipInputStream
#include <wx/wfstream.h>     // Para wxFileOutputStream y wxFileInputStream
#include <wx/sstream.h>      // Para wxStringOutputStream
#include "third_party/json.hpp"
#include <fstream>
#include <wx/wx.h>
#include "print/EscPos/LineStructure/LineStructure.hpp"
using json = nlohmann::json;

/**
    @namespace PrintGetters
    @brief Functions that get data from printer/ticket settings
**/
namespace PrintGetters {

    /**
    @brief  Gets if escpos mode or canvas mode
    @retval  - Returns 1 if escpos is true, 0 if canvas, or -1 if error
    **/
    inline char GetMode() {
        try {
            if (!wxFile::Exists(TICKET_PATH)) {
                wxMessageBox(_("No saved ticket was found."), "Info", wxOK | wxICON_INFORMATION);
                return -1;
            }

            wxFileInputStream fileIn(TICKET_PATH);
            if (!fileIn.IsOk()) {
                wxMessageBox(_("The ticket file could not be opened"), "Error", wxOK | wxICON_ERROR);
                return -1;
            }
            wxZipInputStream zipIn(fileIn); // Crear el stream ZIP de entrada
            // Buscar y leer el archivo data.json
            std::string jsonContent;
            wxZipEntry* entry = nullptr; // Puntero para las entradas del ZIP
            //Mientras haya archivos en el ZIP tomamos la siguiente
            while ((entry = zipIn.GetNextEntry()) != nullptr) {
                if (entry->GetName() == "data.json") {
                    // Leer todo el contenido del JSON
                    size_t size = entry->GetSize(); // Obtener el tamaÃ±o del archivo JSON
                    char* buffer = new char[size + 1]; //Creamos un buffer para leer el JSON, +1 para el null terminator que es necesario para convertir a string
                    zipIn.Read(buffer, size); // Leer los datos del JSON, el size indica cuantos bytes leer
                    buffer[size] = '\0'; //Asegurarse de que el buffer termine en null para convertir a string
                    jsonContent = std::string(buffer); // Convertir a std::string
                    delete[] buffer; // Liberar el buffer
                    delete entry; // Liberar la entrada del ZIP
                    break;
                }
                delete entry; // Liberar la entrada del ZIP si no es data.json
            }

            if (jsonContent.empty()) {
                wxMessageBox(_("No data.json file was found."), "Error", wxOK | wxICON_ERROR);
                return -1;
            }

            // Parsear el JSON
            json root = json::parse(jsonContent);

            if (!root.contains("escpos")) {
                wxMessageBox(wxString::Format(_("The mode could not be obtained")), "Error", wxOK | wxICON_ERROR);
                return -1;
            }

            bool escpos = root["escpos"];
            char mode = 0;
            mode = escpos ? 1 : 0;
            return mode;
        }
        catch (const json::exception& e) {
            wxMessageBox(wxString::Format("Error JSON reading ticket: %s", e.what()), "Error", wxOK | wxICON_ERROR);
            return -1;
        }
        catch (const std::exception& e) {
            wxMessageBox(wxString::Format("Exception when reading the ticket: %s", e.what()), "Error", wxOK | wxICON_ERROR);
            return -1;
        }
        catch (...) {
            wxMessageBox("Unknown error reading the ticket", "Error", wxOK | wxICON_ERROR);
            return -1;
        }
    }

    /**
        @brief Get if print ticket is active
        @retval  - If successful returns True if active, false if disabled or an error
    **/
    inline bool IsPrintTicket() {
        std::string filename = PRINT_SETTINGS_PATH;
        std::ifstream infile(filename);
        if (!infile.is_open()) {
            // Si no existe el archivo, devolver false
            return false;
        }

        try {
            json j;
            infile >> j;
            infile.close();

            // Verificar si existe el campo "printer"
            if (j.contains("isPrint") && j["isPrint"].is_boolean()) {
                bool isPrint = j["isPrint"];
                return isPrint;
            }
            else return false; // No hay campo "printer"   
        }
        catch (...) {
            // Error al leer JSON
            return false;
        }
    }


    /**
     @brief Loads the saved printer
     @retval  - Returns the saved printer, if not returns ""
    **/
    inline wxString GetSelectedPrinter()
    {
        std::string filename = PRINT_SETTINGS_PATH;
        std::ifstream infile(filename);
        if (!infile.is_open()) return wxString(""); // Si no existe el archivo, devolver cadena vacÃ­a
        
        try {
            json j;
            infile >> j;
            infile.close();

            // Verificar si existe el campo "printer"
            if (j.contains("printer") && j["printer"].is_string()) {
                std::string printerName = j["printer"];
                return wxString(printerName);
            }
            else return wxString(""); // No hay campo "printer"
            
        }
        catch (...) {
            // Error al leer JSON
            return wxString("");
        }
    }

    /**
     @brief  Get the width of the canvas or escpos
     @retval  - Canvas/EscPos width
    **/
    inline int GetWidth() {
        try {
            if (!wxFile::Exists(TICKET_PATH)) {
                wxMessageBox(_("No saved ticket was found."), "Info", wxOK | wxICON_INFORMATION);
                return -1;
            }
            wxFileInputStream fileIn(TICKET_PATH);
            if (!fileIn.IsOk()) {
                wxMessageBox(_("The ticket file could not be opened"), "Error", wxOK | wxICON_ERROR);
                return -1;
            }
            wxZipInputStream zipIn(fileIn); // Crear el stream ZIP de entrada
            // Buscar y leer el archivo data.json
            std::string jsonContent;
            wxZipEntry* entry = nullptr; // Puntero para las entradas del ZIP
            //Mientras haya archivos en el ZIP tomamos la siguiente
            while ((entry = zipIn.GetNextEntry()) != nullptr) {
                if (entry->GetName() == "data.json") {
                    // Leer todo el contenido del JSON
                    size_t size = entry->GetSize(); // Obtener el tamaÃ±o del archivo JSON
                    char* buffer = new char[size + 1]; //Creamos un buffer para leer el JSON, +1 para el null terminator que es necesario para convertir a string
                    zipIn.Read(buffer, size); // Leer los datos del JSON, el size indica cuantos bytes leer
                    buffer[size] = '\0'; //Asegurarse de que el buffer termine en null para convertir a string
                    jsonContent = std::string(buffer); // Convertir a std::string
                    delete[] buffer; // Liberar el buffer
                    delete entry; // Liberar la entrada del ZIP
                    break;
                }
                delete entry; // Liberar la entrada del ZIP si no es data.json
            }

            if (jsonContent.empty()) {
                wxMessageBox(_("No data.json file was found."), "Error", wxOK | wxICON_ERROR);
                return -1;
            }

            // Parsear el JSON
            json root = json::parse(jsonContent);

            if (!root.contains("canvas_width") || !root["canvas_width"].is_number()) {
                wxMessageBox(_("Invalid item format"), "Error", wxOK | wxICON_ERROR);
                return -1;
            }

            int width = root["canvas_width"]; //EscPos and Canvas width are the same
            return width;
        }
        catch (const json::exception& e) {
            wxMessageBox(wxString::Format("Error JSON reading ticket: %s", e.what()), "Error", wxOK | wxICON_ERROR);
            return -1;
        }
        catch (const std::exception& e) {
            wxMessageBox(wxString::Format("Exception when reading the ticket: %s", e.what()), "Error", wxOK | wxICON_ERROR);
            return -1;
        }
        catch (...) {
            wxMessageBox("Unknown error reading the ticket", "Error", wxOK | wxICON_ERROR);
            return -1;
        }
    }

    /**
     @brief  Loads the canvas items from file
     @retval  - Canvas items
    **/
    inline std::vector<CanvasItem> LoadCanvasFromFile() {
        std::vector <CanvasItem> items;
        try {
            if (!wxFile::Exists(TICKET_PATH)) {
                wxMessageBox("No saved ticket was found.", "Info", wxOK | wxICON_INFORMATION);
                return items;
            }
            wxFileInputStream fileIn(TICKET_PATH);
            if (!fileIn.IsOk()) {
                wxMessageBox("The ticket file could not be opened", "Error", wxOK | wxICON_ERROR);
                return items;
            }
            wxZipInputStream zipIn(fileIn); // Crear el stream ZIP de entrada
            // Buscar y leer el archivo data.json
            std::string jsonContent;
            wxZipEntry* entry = nullptr; // Puntero para las entradas del ZIP
            //Mientras haya archivos en el ZIP tomamos la siguiente
            while ((entry = zipIn.GetNextEntry()) != nullptr) {
                if (entry->GetName() == "data.json") {
                    // Leer todo el contenido del JSON
                    size_t size = entry->GetSize(); // Obtener el tamaÃ±o del archivo JSON
                    char* buffer = new char[size + 1]; //Creamos un buffer para leer el JSON, +1 para el null terminator que es necesario para convertir a string
                    zipIn.Read(buffer, size); // Leer los datos del JSON, el size indica cuantos bytes leer
                    buffer[size] = '\0'; //Asegurarse de que el buffer termine en null para convertir a string
                    jsonContent = std::string(buffer); // Convertir a std::string
                    delete[] buffer; // Liberar el buffer
                    delete entry; // Liberar la entrada del ZIP
                    break;
                }
                delete entry; // Liberar la entrada del ZIP si no es data.json
            }

            if (jsonContent.empty()) {
                wxMessageBox("No data.json file was found.", "Error", wxOK | wxICON_ERROR);
                return items;
            }

            // Parsear el JSON
            json root = json::parse(jsonContent);

            // Verificar versiÃ³n (para futuras compatibilidades)
            if (root.contains("version")) {
                std::string version = root["version"];
                // AquÃ­ puedes manejar diferentes versiones si es necesario
            }

            // Leer dimensiones del canvas
            uint32_t widthCanvas = root["canvas_width"];
            uint32_t heightCanvas = root["canvas_height"];

            // Convertir a pÃ­xeles y redimensionar
            int widthPixels = widthCanvas * 8;
            int heightPixels = heightCanvas * 8;

            // Limpiar items existentes
            items.clear();

            // Leer items
            if (!root.contains("itemsCanvas") || !root["itemsCanvas"].is_array()) {
                wxMessageBox("Invalid item format", "Error", wxOK | wxICON_ERROR);
                return items;
            }

            json itemsArrayCanvas = root["itemsCanvas"];

            for (const auto& jsonItem : itemsArrayCanvas) { //Iterar sobre cada item en el array
                CanvasItem item;
                // Leer propiedades comunes
                item.type = static_cast<CanvasItem::Type>(jsonItem["type"].get<int>());
                item.pos.x = jsonItem["pos_x"];
                item.pos.y = jsonItem["pos_y"];
                item.fontSize = jsonItem["font_size"];

                if (item.type == CanvasItem::TEXT) {
                    item.text = wxString::FromUTF8(jsonItem["text"].get<std::string>());
                    item.textWithFormat = wxString::FromUTF8(jsonItem["textWithFormat"].get<std::string>());
                    item.underInfo = jsonItem["under_info"];
                }
                else if (item.type == CanvasItem::PURCHASE_INFO) {
                    item.text = wxString::FromUTF8(jsonItem["text"].get<std::string>());
                    item.textWithFormat = wxString::FromUTF8(jsonItem["textWithFormat"].get<std::string>());
                    item.textRightWidth = jsonItem["textRightWidth"].get<int>();
                }
                else if (item.type == CanvasItem::IMAGE) {
                    item.imgWidth = jsonItem["img_width"];
                    item.imgHeight = jsonItem["img_height"];
                    std::string imageFile = jsonItem["image_file"];

                    // Volver a abrir el ZIP para leer la imagen
                    wxFileInputStream fileIn2(TICKET_PATH);
                    wxZipInputStream zipIn2(fileIn2);

                    wxZipEntry* imgEntry = nullptr;
                    bool imageFound = false;

                    while ((imgEntry = zipIn2.GetNextEntry()) != nullptr) {
                        if (imgEntry->GetName().ToStdString() == imageFile) {
                            // Leer la imagen en memoria
                            size_t size = imgEntry->GetSize(); // Obtener el tamaÃ±o de la imagen
                            char* buffer = new char[size]; // Crear un buffer de bytes para la imagen
                            zipIn2.Read(buffer, size); // Leer los datos de la imagen

                            wxMemoryInputStream memInput(buffer, size);// Crear un stream de entrada en memoria
                            item.originalImage.LoadFile(memInput, wxBITMAP_TYPE_PNG); // Cargar la imagen desde el stream de memoria
                            item.bmp = wxBitmap(item.originalImage); // Crear el bitmap desde la imagen cargada

                            delete[] buffer;
                            delete imgEntry;
                            imageFound = true;
                            break;
                        }
                        delete imgEntry;
                    }

                    if (!imageFound) {
                        wxMessageBox(wxString::Format(_("Image not found: %s"), imageFile), "Advertencia", wxOK | wxICON_WARNING);
                        continue;
                    }
                }
                items.push_back(item);
            }
            return items;
        }
        catch (const json::exception& e) {
            wxMessageBox(wxString::Format("Exception when reading the ticket: %s", e.what()), "Error", wxOK | wxICON_ERROR);
            return std::vector<CanvasItem>();
        }
        catch (const std::exception& e) {
            wxMessageBox(wxString::Format("Exception when reading the ticket: %s", e.what()), "Error", wxOK | wxICON_ERROR);
            return std::vector<CanvasItem>();
        }
        catch (...) {
            wxMessageBox("Unknown error reading the ticket", "Error", wxOK | wxICON_ERROR);
            return std::vector<CanvasItem>();
        }
    }

    /**
     @brief  Loads esc pos items from file
     @retval  - Vector of esc pos items loaded from file
     **/
    inline std::vector<ContenidoLinea> LoadEscPosFromFile() {
        std::vector <ContenidoLinea> items;
        try {
            if (!wxFile::Exists(TICKET_PATH)) {
                wxMessageBox(_("No saved ticket was found."), "Info", wxOK | wxICON_INFORMATION);
                return items;
            }
            wxFileInputStream fileIn(TICKET_PATH);
            if (!fileIn.IsOk()) {
                wxMessageBox(_("The ticket file could not be opened"), "Error", wxOK | wxICON_ERROR);
                return items;
            }
            wxZipInputStream zipIn(fileIn); // Crear el stream ZIP de entrada
            // Buscar y leer el archivo data.json
            std::string jsonContent;
            wxZipEntry* entry = nullptr; // Puntero para las entradas del ZIP
            //Mientras haya archivos en el ZIP tomamos la siguiente
            while ((entry = zipIn.GetNextEntry()) != nullptr) {
                if (entry->GetName() == "data.json") {
                    // Leer todo el contenido del JSON
                    size_t size = entry->GetSize(); // Obtener el tamaÃ±o del archivo JSON
                    char* buffer = new char[size + 1]; //Creamos un buffer para leer el JSON, +1 para el null terminator que es necesario para convertir a string
                    zipIn.Read(buffer, size); // Leer los datos del JSON, el size indica cuantos bytes leer
                    buffer[size] = '\0'; //Asegurarse de que el buffer termine en null para convertir a string
                    jsonContent = std::string(buffer); // Convertir a std::string
                    delete[] buffer; // Liberar el buffer
                    delete entry; // Liberar la entrada del ZIP
                    break;
                }
                delete entry; // Liberar la entrada del ZIP si no es data.json
            }

            if (jsonContent.empty()) {
                wxMessageBox(_("No data.json file was found."), "Error", wxOK | wxICON_ERROR);
                return items;
            }

            // Parsear el JSON
            json root = json::parse(jsonContent);

            if (!root.contains("itemsEscPos") || !root["itemsEscPos"].is_array()) {
                wxMessageBox("Invalid item format", "Error", wxOK | wxICON_ERROR);
                return items;
            }

            json itemsArrayEscPos = root["itemsEscPos"];

            for (const auto& jsonItem : itemsArrayEscPos) { //Iterar sobre cada item en el array
                ContenidoLinea item;
                // Leer propiedades comunes
                item.type = static_cast<ContenidoLinea::Type>(jsonItem["type"].get<int>());
                item.numero = static_cast<int>(jsonItem["line"].get<int>());
                item.esImagen = false;

                if (item.type == ContenidoLinea::TEXT) item.texto = wxString::FromUTF8(jsonItem["content"].get<std::string>());
                else if (item.type == ContenidoLinea::PURCHASE_INFO) item.texto = wxString::FromUTF8(jsonItem["content"].get<std::string>());
                else if (item.type == ContenidoLinea::IMAGE) {
                    item.esImagen = true;
                    std::string imageFile = jsonItem["file"];

                    // Volver a abrir el ZIP para leer la imagen
                    wxFileInputStream fileIn2(TICKET_PATH);
                    wxZipInputStream zipIn2(fileIn2);

                    wxZipEntry* imgEntry = nullptr;
                    bool imageFound = false;

                    while ((imgEntry = zipIn2.GetNextEntry()) != nullptr) {
                        if (imgEntry->GetName().ToStdString() == imageFile) {
                            // Leer la imagen en memoria
                            size_t size = imgEntry->GetSize(); // Obtener el tamaÃ±o de la imagen
                            char* buffer = new char[size]; // Crear un buffer de bytes para la imagen
                            zipIn2.Read(buffer, size); // Leer los datos de la imagen

                            wxMemoryInputStream memInput(buffer, size);// Crear un stream de entrada en memoria
                            item.imagen.LoadFile(memInput, wxBITMAP_TYPE_PNG); // Cargar la imagen desde el stream de memoria

                            delete[] buffer;
                            delete imgEntry;
                            imageFound = true;
                            break;
                        }
                        delete imgEntry;
                    }

                    if (!imageFound) {
                        wxMessageBox(wxString::Format(_("Image not found: %s"), imageFile), _("Warning"), wxOK | wxICON_WARNING);
                        continue;
                    }
                }
                items.push_back(item);
            }
            return items;
        }
        catch (const json::exception& e) {
            wxMessageBox(wxString::Format("Error JSON reading ticket: %s", e.what()), "Error", wxOK | wxICON_ERROR);
            return std::vector<ContenidoLinea>();
        }
        catch (const std::exception& e) {
            wxMessageBox(wxString::Format("Exception when reading the ticket: %s", e.what()), "Error", wxOK | wxICON_ERROR);
            return std::vector<ContenidoLinea>();
        }
        catch (...) {
            wxMessageBox("Unknown error reading the ticket", "Error", wxOK | wxICON_ERROR);
            return std::vector<ContenidoLinea>();
        }
    }

}