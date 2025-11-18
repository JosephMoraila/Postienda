#include "gui/TicketFrame/Canvas/Canvas.hpp"
#include <wx/file.h>         // wxFile
#include <wx/mstream.h>      // wxMemoryOutputStream, wxMemoryInputStream
#include <wx/display.h>
#include "constants/TICKET/TICKET_PATH.hpp"
#include "gui/TicketFrame/TicketFrame.hpp"
#include <wx/zipstrm.h>      // Para wxZipOutputStream y wxZipInputStream
#include <wx/wfstream.h>     // Para wxFileOutputStream y wxFileInputStream
#include <wx/sstream.h>      // Para wxStringOutputStream
#include <json.hpp>
#include "constants/MESSAGES_ADVICE/WARNING/WARNING_MESSAGES.hpp"
using namespace WARNING_MESSAGES;

using json = nlohmann::json;

wxBEGIN_EVENT_TABLE(Canvas, wxPanel)
EVT_PAINT(Canvas::OnPaint)
EVT_LEFT_DOWN(Canvas::OnLeftDown)
EVT_LEFT_UP(Canvas::OnLeftUp)
EVT_LEFT_DCLICK(Canvas::OnDoubleLeftClick)
EVT_MOTION(Canvas::OnMouseMove)
EVT_KEY_DOWN(Canvas::OnKeyDown)
wxEND_EVENT_TABLE()

Canvas::Canvas(TicketFrame* parentFrame, wxWindow* parent) : wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxSize(464, 800)) {
    this->parentFrame = parentFrame;
    SetBackgroundColour(*wxWHITE);
    SetFocus(); // Permite que el panel reciba eventos de teclado
    SetScrollRate(10, 10);
}

void Canvas::AddText(const wxString& text, bool isUnderInfo) {
    CanvasItem item;
    item.type = CanvasItem::TEXT;
    item.text = text;
	item.textWithFormat = text;
	item.underInfo = isUnderInfo;
    item.pos = wxPoint(50, 50);
    items.push_back(item);
    SetFocus(); // Recuperar el foco despuÃÂÃÂ©s de agregar texto
    Refresh();
}

void Canvas::AddImage(const wxBitmap& bmp) {
    CanvasItem item;
    item.type = CanvasItem::IMAGE;
    item.bmp = bmp;
    item.originalImage = bmp.ConvertToImage();
    item.pos = wxPoint(50, 50);

    // Obtener dimensiones originales
    int originalWidth = bmp.GetWidth();
    int originalHeight = bmp.GetHeight();

    // Definir tamaÃÂÃÂ±o mÃÂÃÂ¡ximo (ajusta estos valores segÃÂÃÂºn tu necesidad)
    const int MAX_WIDTH = 400;   // Ancho mÃÂÃÂ¡ximo en pÃÂÃÂ­xeles
    const int MAX_HEIGHT = 400;  // Alto mÃÂÃÂ¡ximo en pÃÂÃÂ­xeles

    int newWidth = originalWidth;
    int newHeight = originalHeight;

    // Calcular si necesita redimensionarse
    if (originalWidth > MAX_WIDTH || originalHeight > MAX_HEIGHT) {
        // Calcular ratio de escalado manteniendo proporciÃÂÃÂ³n
        double scaleWidth = (double)MAX_WIDTH / originalWidth;
        double scaleHeight = (double)MAX_HEIGHT / originalHeight;

        // Usar el menor ratio para que quepa dentro de los lÃÂÃÂ­mites
        double scale = std::min(scaleWidth, scaleHeight);

        newWidth = (int)(originalWidth * scale);
        newHeight = (int)(originalHeight * scale);

        // Redimensionar la imagen original para impresiÃÂÃÂ³n
        item.originalImage = item.originalImage.Scale(newWidth, newHeight, wxIMAGE_QUALITY_HIGH);

        // Redimensionar el bitmap para visualizaciÃÂÃÂ³n
        wxImage resizedImg = bmp.ConvertToImage();
        resizedImg = resizedImg.Scale(newWidth, newHeight, wxIMAGE_QUALITY_HIGH);
        item.bmp = wxBitmap(resizedImg);
    }

    item.imgWidth = newWidth;
    item.imgHeight = newHeight;

    items.push_back(item);
    SetFocus(); // Recuperar el foco despuÃÂÃÂ©s de agregar imagen
    Refresh();
}
void Canvas::AddPurchaseInfo() {
	CanvasItem item;
	item.type = CanvasItem::PURCHASE_INFO;
    wxString textInfo = _("Sunday, 9/november/2025\n16:14:30\n"
         "Bread    1234    $99\nMeat  4321    $199\n"
         "Total: $288\nPaid: $300\nChange: $12\nAttended by: Jose");
    item.text = textInfo;
	item.textWithFormat = textInfo;
	item.pos = wxPoint(50, 80);
	items.push_back(item);
	SetFocus(); 
	Refresh();
	wxMessageBox(_("Purchase information has been added to the receipt. Please note that this is an example of how it will appear, as the actual receipt may be longer when purchasing many items, so the actual size may be longer than what you are editing."), "Info", wxOK | wxICON_INFORMATION);
}

bool Canvas::SaveToFile() {
    try {
        wxFileOutputStream fileOut(TICKET_PATH);
        if (!fileOut.IsOk()) {
            wxMessageBox(_("The ticket file could not be created"), "Error", wxOK | wxICON_ERROR);
            return false;
        }
        wxZipOutputStream zipOut(fileOut);
        wxDisplay display;
        wxSize ppi = display.GetPPI();
        json root;
        root["version"] = "1.0";
        root["canvas_dpi_x"] = ppi.x;
        root["canvas_dpi_y"] = ppi.y;
        root["canvas_width"] = parentFrame->widthCanvas;
        root["canvas_height"] = parentFrame->heightCanvas;
        root["escpos"] = parentFrame->modoEscPos;

        json itemsArrayCanvas = json::array();
        int imageCounter = 0;
        for (const CanvasItem& item : items) {
            json jsonItem;
            jsonItem["type"] = static_cast<int>(item.type);
            jsonItem["pos_x"] = item.pos.x;
            jsonItem["pos_y"] = item.pos.y;
            jsonItem["font_size"] = item.fontSize;

            if (item.type == CanvasItem::TEXT) {
                jsonItem["text"] = std::string(item.text.ToUTF8().data());
                jsonItem["under_info"] = item.underInfo;
                jsonItem["textRightWidth"] = item.textRightWidth;
                jsonItem["textWithFormat"] = std::string(item.textWithFormat.ToUTF8().data());
            }
            else if (item.type == CanvasItem::PURCHASE_INFO) {
                jsonItem["text"] = std::string(item.text.ToUTF8().data());
                jsonItem["is_purchase_info"] = true;
                jsonItem["textRightWidth"] = item.textRightWidth;
                jsonItem["textWithFormat"] = std::string(item.textWithFormat.ToUTF8().data());
            }
            else if (item.type == CanvasItem::IMAGE) {
                jsonItem["img_width"] = item.imgWidth;
                jsonItem["img_height"] = item.imgHeight;
                wxString imageName = wxString::Format("image_%d.png", imageCounter++);
                jsonItem["image_file"] = std::string(imageName.ToUTF8().data());

                wxMemoryOutputStream memStream;
                if (!item.originalImage.SaveFile(memStream, wxBITMAP_TYPE_PNG)) {
                    wxMessageBox(_("Error saving image in memory"), "Error", wxOK | wxICON_ERROR);
                    continue;
                }
                zipOut.PutNextEntry(imageName);
                zipOut.Write(memStream.GetOutputStreamBuffer()->GetBufferStart(), memStream.GetSize());
                zipOut.CloseEntry();
            }

            itemsArrayCanvas.push_back(jsonItem);
        }
        root["itemsCanvas"] = itemsArrayCanvas;

        std::string jsonStr = root.dump(2, ' ', false, json::error_handler_t::replace);
        zipOut.PutNextEntry("data.json");
        zipOut.Write(jsonStr.data(), jsonStr.size());
        zipOut.CloseEntry();

        zipOut.Close();
        fileOut.Close();

        return true;
    }
    catch (const json::exception& e) {
        wxMessageBox(wxString::Format(_("Error saving JSON: %s"), e.what()),"Error", wxOK | wxICON_ERROR);
        return false;
    }
    catch (const std::exception& e) {
        wxMessageBox(wxString::Format(_("Saving error: %s"), e.what()),"Error", wxOK | wxICON_ERROR);
        return false;
    }
}

bool Canvas::LoadFromFile() {
    try {
        if (!wxFile::Exists(TICKET_PATH)) {
            wxMessageBox(_("No ticket was found."), "Info", wxOK | wxICON_INFORMATION);
            return true;
        }

        wxFileInputStream fileIn(TICKET_PATH);
        if (!fileIn.IsOk()) {
            wxMessageBox(_("File ticket could not be opened"), "Error", wxOK | wxICON_ERROR);
            return false;
        }
		wxZipInputStream zipIn(fileIn); // Crear el stream ZIP de entrada
        // Buscar y leer el archivo data.json
        std::string jsonContent;
		wxZipEntry* entry = nullptr; // Puntero para las entradas del ZIP
		//Mientras haya archivos en el ZIP tomamos la siguiente
        while ((entry = zipIn.GetNextEntry()) != nullptr) {
            if (entry->GetName() == "data.json") {
                // Leer todo el contenido del JSON
				size_t size = entry->GetSize(); // Obtener el tamaÃÂÃÂ±o del archivo JSON
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
            wxMessageBox(_("No data.json was found in the file"), "Error", wxOK | wxICON_ERROR);
            return false;
        }

        // Parsear el JSON
        json root = json::parse(jsonContent);

        // Verificar versiÃÂÃÂ³n (para futuras compatibilidades)
        if (root.contains("version")) {
            std::string version = root["version"];
            // AquÃÂÃÂ­ puedes manejar diferentes versiones si es necesario
        } 

        // Leer dimensiones del canvas
        uint32_t widthCanvas = root["canvas_width"];
        uint32_t heightCanvas = root["canvas_height"];

        parentFrame->widthCanvas = widthCanvas;
        parentFrame->widthCtrl->SetValue(widthCanvas);
        parentFrame->heightCanvas = heightCanvas;
        parentFrame->heightCtrl->SetValue(heightCanvas);

        parentFrame->modoEscPos = root["escpos"];
        parentFrame->ChangeMode();

        // Convertir a pÃÂÃÂ­xeles y redimensionar
        int widthPixels = widthCanvas * 8;
        int heightPixels = heightCanvas * 8;

        SetMaxSize(wxSize(widthPixels, heightPixels));
        SetMinSize(wxSize(widthPixels, heightPixels));
        SetVirtualSize(widthPixels, heightPixels);

        //Establecer tambien el tamaÃÂÃÂ±o del input para que sea igual al del canvas
        parentFrame->richInput->SetMaxSize(wxSize(widthPixels, heightPixels));
        parentFrame->richInput->SetMinSize(wxSize(widthPixels, heightPixels));
        parentFrame->richInput->SetSize(widthPixels, heightPixels);
        parentFrame->Layout();

        // Limpiar items existentes
        items.clear();
        parentFrame->alreadyExistsPurchaseInfo = false;

        // Leer items del canvas
        if (!root.contains("itemsCanvas") || !root["itemsCanvas"].is_array()) {
            wxMessageBox(_("Invalid items format"), "Error", wxOK | wxICON_ERROR);
            return false;
        }

        json itemsArray = root["itemsCanvas"];

        for (const auto& jsonItem : itemsArray) {
            CanvasItem item;
            item.type = static_cast<CanvasItem::Type>(jsonItem["type"].get<int>());
            item.pos.x = jsonItem["pos_x"];
            item.pos.y = jsonItem["pos_y"];
            item.fontSize = jsonItem["font_size"];

            if (item.type == CanvasItem::TEXT) {
                item.text = wxString::FromUTF8(jsonItem["text"].get<std::string>());
                item.underInfo = jsonItem["under_info"];
                item.textRightWidth = jsonItem["textRightWidth"]; 
                item.textWithFormat = wxString::FromUTF8(jsonItem["textWithFormat"].get<std::string>());
            }
            else if (item.type == CanvasItem::PURCHASE_INFO) {
                item.text = wxString::FromUTF8(jsonItem["text"].get<std::string>());
				if (jsonItem.contains("is_purchase_info") && jsonItem["is_purchase_info"]) { //Si el item contiene is_purchase_info y es true
                    parentFrame->alreadyExistsPurchaseInfo = true;
                    item.textRightWidth = jsonItem["textRightWidth"];
                    item.textWithFormat = wxString::FromUTF8(jsonItem["textWithFormat"].get<std::string>());
                }
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
						size_t size = imgEntry->GetSize(); // Obtener el tamaÃÂÃÂ±o de la imagen
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
                    wxMessageBox(wxString::Format(wxString::FromUTF8(_("Image was not found: %s")), imageFile), WARNING, wxOK | wxICON_WARNING);
                    continue;
                }
            }
            items.push_back(item);
        }

        //Restablecer del EscPos
        if (!root.contains("itemsEscPos") || !root["itemsEscPos"].is_array()) wxMessageBox(wxString::FromUTF8(_("Invalid EscPos Format")), "Error", wxOK | wxICON_ERROR);
        else {
            json itemsEscPos = root["itemsEscPos"];

            // Volver al inicio del archivo para leer las imÃÂÃÂ¡genes del ZIP
            fileIn.SeekI(0);
            wxZipInputStream zipInImages(fileIn);
            std::unique_ptr<wxZipEntry> entryImg;

            // Cargar imÃÂÃÂ¡genes EscPos del ZIP a memoria
            std::map<wxString, std::vector<char>> imagenesEscPos;
            while (entryImg.reset(zipInImages.GetNextEntry()), entryImg != nullptr) {
                wxString nombre = entryImg->GetName();
                if (nombre.StartsWith("imageEscPos_")) {
                    size_t size = entryImg->GetSize();
                    if (size > 0) {
                        std::vector<char> buffer(size);
                        zipInImages.Read(buffer.data(), size);
                        imagenesEscPos[nombre] = std::move(buffer);
                    }
                }
            }

            // Limpiar y reconstruir el richInput
            parentFrame->richInput->Freeze();
            parentFrame->richInput->Clear();

            int lastLine = -1; // Para saber cuÃÂÃÂ¡ntos saltos de lÃÂÃÂ­nea agregar

            for (const auto& item : itemsEscPos) {
                int tipo = item.value("type", 0);
                int linea = item.value("line", 0);

                // Si hay lÃÂÃÂ­neas vacÃÂÃÂ­as entre el ÃÂÃÂºltimo elemento y el actual, agregarlas
                if (lastLine >= 0 && linea > lastLine + 1) {
                    int emptyLines = linea - lastLine - 1;
                    for (int i = 0; i < emptyLines; ++i)
                        parentFrame->richInput->Newline();
                }

                switch (tipo) {
                case 0: { // TEXTO NORMAL
                    wxString texto = wxString::FromUTF8(item.value("content", "").c_str());
                    parentFrame->richInput->WriteText(texto + "\n");
                    break;
                }

                case 1: { // IMAGEN
                    std::string fileName = item.value("file", "");
                    wxString wxFileName = wxString::FromUTF8(fileName.c_str());

                    auto it = imagenesEscPos.find(wxFileName);
                    if (it != imagenesEscPos.end()) {
                        wxMemoryInputStream memIn(it->second.data(), it->second.size());
                        wxImage img(memIn, wxBITMAP_TYPE_PNG);
                        if (img.IsOk()) {
                            if (img.GetWidth() > 400) img = img.Scale(400, (400 * img.GetHeight()) / img.GetWidth(), wxIMAGE_QUALITY_HIGH);
                            parentFrame->richInput->WriteImage(img);
                            parentFrame->richInput->Newline();
                        }
                    }
                    else wxLogWarning(_("ESC/POS image not found in ZIP: %s"), wxFileName);
                    break;
                }

                case 2: { // PURCHASE INFO
                    wxString texto = wxString::FromUTF8(item.value("content", "").c_str());
                    parentFrame->richInput->BeginBold();
                    parentFrame->richInput->WriteText(texto + "\n");
                    parentFrame->richInput->EndBold();
                    break;
                }

                default:
                    wxLogWarning(_("Unknown item format: %d"), tipo);
                    break;
                }

                lastLine = linea;
            }
            parentFrame->richInput->Thaw();
            parentFrame->richInput->Layout();
            parentFrame->richInput->Refresh();
        }

        Refresh();
        parentFrame->AlignCenterRichInput(parentFrame->richInput);
        parentFrame->AdjustCanvasScroll();
        return true;
    }
    catch (const json::exception& e) {
        wxMessageBox(wxString::Format(_("Error JSON reading ticket: %s"), e.what()),"Error", wxOK | wxICON_ERROR);
        return false;
    }
    catch (const std::exception& e) {
        wxMessageBox(wxString::Format(wxString::FromUTF8(_("Exception when reading the ticket: %s")), e.what()),"Error", wxOK | wxICON_ERROR);
        return false;
    }
    catch (...) {
        wxMessageBox(_("Unknown error reading ticket"), "Error", wxOK | wxICON_ERROR);
        return false;
    }
}
