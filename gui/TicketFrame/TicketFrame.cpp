#include "TicketFrame.hpp"
#include <wx/filedlg.h>
#include <wx/image.h>
#include "utils/window/WindowUtils.h"
#include "gui/TicketFrame/Canvas/InputTextModal/InputTextModal.hpp"
#include "gui/TicketFrame/PrintMenu/PrintMenu.hpp"
#include "third_party/json.hpp"
#include "constants/TICKET/TICKET_PATH.hpp"


using json = nlohmann::json;

// -------------------- TicketFrame --------------------
TicketFrame::TicketFrame(wxWindow* parent) : wxFrame(parent, wxID_ANY, "Ticket", wxDefaultPosition, wxSize(1000, 700))
{
    Widgets();
    Bind(wxEVT_SIZE, &TicketFrame::OnSize, this);
    AdjustCanvasScroll();
    OnLoad();
    Bind(wxEVT_CLOSE_WINDOW, [this](wxCloseEvent& event) {
        if (!canvas->SaveToFile()) { 
            wxMessageBox(_("Error saving canvas ticket"), "Error", wxOK | wxICON_ERROR);
            event.Skip();
            return;
        }
        if (!SaveEscPosTicket()) {
            wxMessageBox(_("Error saving EscPos ticket"), "Error", wxOK | wxICON_ERROR);
            event.Skip();
            return;
        }
        event.Skip(); // Asegura que el evento se propague para cerrar la ventana
		});
}

void TicketFrame::Widgets() {
    mainPanel = new wxPanel(this); // Crear panel principal
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* dimensionSizer = new wxBoxSizer(wxHORIZONTAL);

    wxButton* btnAddText = new wxButton(mainPanel, wxID_ANY, _("Add text"), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    btnAddText->Bind(wxEVT_ENTER_WINDOW, [this](wxMouseEvent& event) {OnButtonEnter(event, temaOscuro); });
	btnAddText->Bind(wxEVT_LEAVE_WINDOW, [this](wxMouseEvent& event) {OnButtonLeave(event, temaOscuro); });
    btnAddText->Bind(wxEVT_BUTTON, &TicketFrame::OnAddText, this);
    wxButton* btnAddImage = new wxButton(mainPanel, wxID_ANY, _("Add image"), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	btnAddImage->Bind(wxEVT_BUTTON, &TicketFrame::OnAddImage, this);
	btnAddImage->Bind(wxEVT_ENTER_WINDOW, [this](wxMouseEvent& event) {OnButtonEnter(event, temaOscuro); });
	btnAddImage->Bind(wxEVT_LEAVE_WINDOW, [this](wxMouseEvent& event) {OnButtonLeave(event, temaOscuro); });
    wxButton* btnAddInfoPurchase = new wxButton(mainPanel, wxID_ANY, _("Add purchase info"), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	btnAddInfoPurchase->Bind(wxEVT_BUTTON, &TicketFrame::OnAddPurchaseInfo, this);
    btnAddInfoPurchase->Bind(wxEVT_ENTER_WINDOW, [this](wxMouseEvent& event) {OnButtonEnter(event, temaOscuro); });
    btnAddInfoPurchase->Bind(wxEVT_LEAVE_WINDOW, [this](wxMouseEvent& event) {OnButtonLeave(event, temaOscuro); });
    btnAddInfoPurchase->SetToolTip(_("Add purchase information.\nThis can only be added once."));
	wxButton* btnSave = new wxButton(mainPanel, wxID_ANY, _("Save"), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	btnSave->Bind(wxEVT_BUTTON, &TicketFrame::OnSave, this);
	btnSave->Bind(wxEVT_ENTER_WINDOW, [this](wxMouseEvent& event) {OnButtonEnter(event, temaOscuro); });
	btnSave->Bind(wxEVT_LEAVE_WINDOW, [this](wxMouseEvent& event) {OnButtonLeave(event, temaOscuro); });
	wxButton* btnTry = new wxButton(mainPanel, wxID_ANY, _("Settings"), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	btnTry->Bind(wxEVT_BUTTON, &TicketFrame::OnSettings, this);
    btnTry->Bind(wxEVT_ENTER_WINDOW, [this](wxMouseEvent& event) {OnButtonEnter(event, temaOscuro); });
    btnTry->Bind(wxEVT_LEAVE_WINDOW, [this](wxMouseEvent& event) {OnButtonLeave(event, temaOscuro); });
    buttonChangeMode = new wxButton(mainPanel, wxID_ANY, _("Change to EscPos"), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    buttonChangeMode->Bind(wxEVT_BUTTON, &TicketFrame::OnChangeToMode, this);
    buttonChangeMode->Bind(wxEVT_ENTER_WINDOW, [this](wxMouseEvent& event) {OnButtonEnter(event, temaOscuro); });
    buttonChangeMode->Bind(wxEVT_LEAVE_WINDOW, [this](wxMouseEvent& event) {OnButtonLeave(event, temaOscuro); });

    // Controles de dimensiones
    dimensionSizer->Add(new wxStaticText(mainPanel, wxID_ANY, _("Width (mm):")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    widthCtrl = new wxSpinCtrl(mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(70, -1));
    widthCtrl->SetRange(20, 200); // Rango tÃÂÃÂ­pico para impresoras tÃÂÃÂ©rmicas
    widthCtrl->SetValue(widthCanvas); // Ancho estÃÂÃÂ¡ndar comÃÂÃÂºn (58mm)

    dimensionSizer->Add(widthCtrl, 0, wxRIGHT, 10);
    dimensionSizer->Add(new wxStaticText(mainPanel, wxID_ANY, _("Height (mm):")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    heightCtrl = new wxSpinCtrl(mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(70, -1));
    heightCtrl->SetRange(20, 2000);
    heightCtrl->SetValue(heightCanvas);
    dimensionSizer->Add(heightCtrl, 0);

    buttonSizer->Add(btnAddText, 0, wxALL, 5);
    buttonSizer->Add(btnAddImage, 0, wxALL, 5);
    buttonSizer->Add(btnAddInfoPurchase, 0, wxALL, 5);
	buttonSizer->Add(btnSave, 0, wxALL, 5);
	buttonSizer->Add(btnTry, 0, wxALL, 5);
    buttonSizer->Add(buttonChangeMode, 0, wxALL, 5);
    mainSizer->Add(buttonSizer, 0, wxALIGN_LEFT);
    mainSizer->Add(dimensionSizer, 0, wxALL, 5);

    // Manejadores de eventos para los controles de dimensiones
    widthCtrl->Bind(wxEVT_SPINCTRL, [this](wxSpinEvent& evt) {
        int widthMM = evt.GetValue(); 
        this->widthCanvas = widthMM;
        int widthPixels = (widthMM * 8); // 8 pixels por mm (203 DPI)
        canvas->SetMaxSize(wxSize(widthPixels, canvas->GetMaxSize().GetHeight()));
        canvas->SetMinSize(wxSize(widthPixels, canvas->GetMinSize().GetHeight()));
        canvas->SetVirtualSize(widthPixels, canvas->GetVirtualSize().GetHeight());
        if (richInput) {
            wxSize currentSize = richInput->GetSize();
            richInput->SetMinSize(wxSize(widthPixels, currentSize.GetHeight()));
            richInput->SetMaxSize(wxSize(widthPixels, currentSize.GetHeight()));
            richInput->SetSize(widthPixels, currentSize.GetHeight()); // actualizar tamaÃÂÃÂ±o actual
        }
        Layout();
        AlignCenterRichInput(richInput);
        AdjustCanvasScroll();
        });

    heightCtrl->Bind(wxEVT_SPINCTRL, [this](wxSpinEvent& evt) {
        int heightMM = evt.GetValue();
        this->heightCanvas = heightMM;
        int heightPixels = (heightMM * 8); // 8 pixels por mm (203 DPI)
        canvas->SetMaxSize(wxSize(canvas->GetMaxSize().GetWidth(), heightPixels));
        canvas->SetMinSize(wxSize(canvas->GetMinSize().GetWidth(), heightPixels));
        canvas->SetVirtualSize(canvas->GetVirtualSize().GetWidth(), heightPixels);
        if (richInput) {
            wxSize currentSize = richInput->GetSize();
            richInput->SetMinSize(wxSize(currentSize.GetWidth(), heightPixels));
            richInput->SetMaxSize(wxSize(currentSize.GetWidth(), heightPixels));
            richInput->SetSize(currentSize.GetWidth(), heightPixels); // actualizar tamaÃÂÃÂ±o actual
        }
        Layout();
        AdjustCanvasScroll();
        });

    int anchoInicialPx = widthCanvas * 8; // 464 px para 58mm
    int alturaInicialPx = heightCanvas * 8; 

    canvas = new Canvas(this, mainPanel); // Cambiar parent a mainPanel
    canvas->SetMinSize(wxSize(anchoInicialPx, alturaInicialPx));
    canvas->SetMaxSize(wxSize(anchoInicialPx, alturaInicialPx));
    canvas->SetVirtualSize(anchoInicialPx, alturaInicialPx); // Asegura el tamaÃÂÃÂ±o virtual inicial
    canvas->SetBackgroundColour(*wxWHITE);

    richInput = new wxRichTextCtrl(mainPanel, wxID_ANY, wxEmptyString,wxDefaultPosition, wxDefaultSize,wxTE_MULTILINE | wxVSCROLL | wxHSCROLL | wxTE_WORDWRAP);
    richInput->Hide();
    wxTextAttr attr;
    attr.SetFont(wxFont(23, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    richInput->SetDefaultStyle(attr);
    richInput->SetBasicStyle(attr);
    richInput->SetMinSize(wxSize(anchoInicialPx, alturaInicialPx));
    richInput->SetMaxSize(wxSize(anchoInicialPx, alturaInicialPx));
    richInput->SetVirtualSize(anchoInicialPx, alturaInicialPx); // Asegura el tamaÃÂÃÂ±o virtual inicial
    AlignCenterRichInput(richInput);

    mainSizer->Add(canvas, 1, wxEXPAND | wxALL, 10);
    mainSizer->Add(richInput, 1, wxEXPAND | wxALL, 10);
    AdjustCanvasScroll();

    mainPanel->SetSizer(mainSizer);

    // Sizer para el frame
    wxBoxSizer* frameSizer = new wxBoxSizer(wxVERTICAL);
    frameSizer->Add(mainPanel, 1, wxEXPAND);
    SetSizer(frameSizer);
}

void TicketFrame::OnAddText(wxCommandEvent&) {
    TextDialog dlg(this, _("Add text"));
    dlg.AplicarTema(temaOscuro);
    if (dlg.ShowModal() == wxID_OK) {
        wxString text = dlg.GetText();
		bool isUnderInfo = dlg.GetIsUnderInfo();
        if (!text.IsEmpty()) { 
            canvas->AddText(text,isUnderInfo);
        }
    }
}

void TicketFrame::OnAddImage(wxCommandEvent&) {
    wxFileDialog openFileDialog(this, _("Select Image"), "", "", _("Image files (*.png;*.jpg;*.jpeg;*.bmp;*.webp)|*.png;*.jpg;*.jpeg;*.bmp;*.webp"), wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (openFileDialog.ShowModal() == wxID_CANCEL) return; // El usuario cancelÃÂÃÂ³ el diÃÂÃÂ¡logo
    wxString filePath = openFileDialog.GetPath();
    wxImage image;
    if (!image.LoadFile(filePath)) {
        wxMessageBox(_("Image could not be loaded."), "Error", wxOK | wxICON_ERROR);
        return;
    }
    if (!modoEscPos) {
        wxBitmap bitmap(image);
        canvas->AddImage(bitmap);
    }
    else {
        int anchoMax = richInput->GetSize().GetWidth() - 60; // ancho mÃÂÃÂ¡ximo disponible
        int anchoDeseado = anchoMax / 2; // la mitad del ancho disponible
        double escala = static_cast<double>(anchoDeseado) / image.GetWidth();
        // Redimensionar la imagen manteniendo la proporciÃÂÃÂ³n
        wxImage imgEscalada = image.Scale(static_cast<int>(image.GetWidth() * escala),static_cast<int>(image.GetHeight() * escala),wxIMAGE_QUALITY_HIGH);
        richInput->WriteImage(imgEscalada);
        richInput->Refresh();
    }
}

void TicketFrame::OnAddPurchaseInfo(wxCommandEvent&) {
    if (!modoEscPos) {
        if (alreadyExistsPurchaseInfo) {
            wxMessageBox(_("Purchase information is already on the receipt.\nIt can only be added once."), "Info", wxOK | wxICON_INFORMATION);
            return;
        }
        canvas->AddPurchaseInfo();
        this->alreadyExistsPurchaseInfo = true;
    }
    else {
        wxString purchaseInfo = _("\n[PURCHASE_INFO_STARTS]\n"
            "Sunday, 9/noviembre/2025\n16:14:30\n"
            "Bread 1234 $99\nMeat 4321 $199\n"
                "Total: $288\nPaid: $300\nChange: $12\nAttended by: Jose\n[PURCHASE_INFO_FINISHES]\n");
        wxString contenido = richInput->GetValue();
        bool existWord = contenido.Contains(purchaseInfo);
        if (existWord) alreadyExistsPurchaseInfoTextInut = true;
        if (existWord && alreadyExistsPurchaseInfoTextInut) {
            wxMessageBox(wxString::FromUTF8(_("Purchase information is already on the receipt.\nIt can only be added once..")), "Info", wxOK | wxICON_INFORMATION);
            return;
        }
        richInput->WriteText(purchaseInfo);
        wxMessageBox(wxString::FromUTF8(_("The purchase information has been added to the receipt.\nPlease note that this is just a preview of how it will look, as the actual receipt may become longer when a purchase includes many products, so the real size may be larger than what you are currently editing.")), "Info", wxOK | wxICON_INFORMATION);
    }
}

void TicketFrame::AplicarTema(bool oscuro) {
    temaOscuro = oscuro;
	ApplyTheme(mainPanel, oscuro);
	canvas->temaOscuro = oscuro;
    Refresh();
    Update();
}

void TicketFrame::AdjustCanvasScroll() {
    // Obtener el tamaÃÂÃÂ±o del ÃÂÃÂ¡rea cliente del TicketFrame (sin incluir bordes/decoraciones)
    wxSize frameSize = GetClientSize();

    // Obtener el tamaÃÂÃÂ±o virtual del canvas (ÃÂÃÂ¡rea total de contenido)
    wxSize virtualSize = canvas->GetVirtualSize();

    // Obtener el tamaÃÂÃÂ±o actual visible del canvas
    wxSize canvasSize = canvas->GetSize();

    // Calcular si necesitamos scrollbars
    bool needHorizontalScroll = virtualSize.GetWidth() > canvasSize.GetWidth();
    bool needVerticalScroll = virtualSize.GetHeight() > canvasSize.GetHeight();

    // Configurar la visibilidad de los scrollbars
    canvas->ShowScrollbars(needHorizontalScroll ? wxSHOW_SB_DEFAULT : wxSHOW_SB_NEVER,needVerticalScroll ? wxSHOW_SB_DEFAULT : wxSHOW_SB_NEVER);
}

void TicketFrame::OnSave(wxCommandEvent& event) {
    bool sucess = canvas->SaveToFile();
    if (!sucess) {
        wxMessageBox(("Error saving canvas ticket"), "Error", wxOK | wxICON_ERROR);
        return;
    }
    if(!SaveEscPosTicket()) wxMessageBox(_("Error saving EscPos ticket"), "Error", wxOK | wxICON_ERROR);
    //MostrarVisorContenido(richInput);

	wxMessageBox(_("Ticket saved successfully.\nIf you want to test the ticket, click the \"Settings\" > \"Test print\" button."), _("Success"), wxOK | wxICON_INFORMATION);
}

void TicketFrame::OnLoad() {
    if (!canvas->LoadFromFile()) wxMessageBox(_("Error loading ticket"), "Error", wxOK | wxICON_ERROR);
    AdjustCanvasScroll();
}


void TicketFrame::OnSize(wxSizeEvent& event) {
    event.Skip();
    CallAfter([this]() { // CallAfter para asegurar que se llame despues de procesar todos los eventos
        AdjustCanvasScroll();
        });
}

void TicketFrame::OnSettings(wxCommandEvent& event) {
    PrintMenu dlg(this);
    dlg.AplicarTema(temaOscuro);
    if (dlg.ShowModal() == wxID_OK) {

    }
}

void TicketFrame::OnChangeToMode(wxCommandEvent& event) {
    modoEscPos = !modoEscPos; // Alternar modo
    ChangeMode();
}

void TicketFrame::ChangeMode() {
    if (modoEscPos) {
        // Cambiar a modo texto (EscPos)
        canvas->Hide();
        richInput->Show();
        buttonChangeMode->SetLabel(_("Change Canvas"));
        richInput->SetFocus();
    }
    else {
        // Cambiar a modo canvas
        richInput->Hide();
        canvas->Show();
        buttonChangeMode->SetLabel(_("Change to EscPos"));
        canvas->SetFocus();
    }

    Layout(); // Reorganiza el sizer
}

bool TicketFrame::SaveEscPosTicket() {
    try {
        // LEER el ZIP existente y cargar todo su contenido en memoria
        std::map<wxString, std::vector<char>> entradasAnteriores;
        json root;
        bool foundJson = false;

        {  // Scope para cerrar los streams automÃÂÃÂ¡ticamente
            wxFileInputStream fileIn(TICKET_PATH);
            if (!fileIn.IsOk()) {
                wxMessageBox(_("Ticket file could not be opened"), "Error", wxOK | wxICON_ERROR);
                return false;
            }

            wxZipInputStream zipIn(fileIn);
            std::unique_ptr<wxZipEntry> entry;

            // Leer TODAS las entradas del ZIP
            while (entry.reset(zipIn.GetNextEntry()), entry != nullptr) {
                wxString nombreEntrada = entry->GetName();
                size_t size = entry->GetSize();

                if (size > 0) {
                    std::vector<char> buffer(size);
                    zipIn.Read(buffer.data(), size);

                    if (nombreEntrada == "data.json") {
                        buffer.push_back('\0');
                        std::string jsonContent(buffer.data());
                        root = json::parse(jsonContent);
                        foundJson = true;
                    }
                    else {
                        // Solo guardar entradas que NO sean imÃÂÃÂ¡genes de EscPos
                        // (las imÃÂÃÂ¡genes de Canvas empiezan con "image_", las de EscPos con "imageEscPos_")
                        if (!nombreEntrada.StartsWith("imageEscPos_")) entradasAnteriores[nombreEntrada] = buffer;
                        // Si empieza con "imageEscPos_", se descarta (se reemplazarÃÂÃÂ¡ con las nuevas)
                    }
                }
            }
        } // fileIn y zipIn se cierran aquÃÂÃÂ­ automÃÂÃÂ¡ticamente

        if (!foundJson) {
            wxMessageBox(wxString::FromUTF8(_("No data.json found in file")), "Error", wxOK | wxICON_ERROR);
            return false;
        }

        // PROCESAR nuevo contenido del RichInput
        std::vector<ContenidoLinea> contentRichInput = ExtraerContenidoRichInput(richInput);
        json arrayEscPos = json::array();
        unsigned int imageCounter = 0;

        // Mapa para guardar las nuevas imÃÂÃÂ¡genes
        std::map<wxString, std::vector<char>> nuevasImagenes;

        for (const ContenidoLinea& linea : contentRichInput) {
            if (linea.type == ContenidoLinea::IMAGE) {
                wxString imageName = wxString::Format("imageEscPos_%d.png", imageCounter++);

                // Crear objeto JSON para la imagen
                json imageObj;
                imageObj["type"] = linea.type;
                imageObj["file"] = std::string(imageName.utf8_str());
                imageObj["line"] = linea.numero;
                arrayEscPos.push_back(imageObj);

                // Guardar imagen en memoria
                wxMemoryOutputStream memStream;
                if (!linea.imagen.SaveFile(memStream, wxBITMAP_TYPE_PNG)) {
                    wxMessageBox(_("Error saving image in memory"), "Error", wxOK | wxICON_ERROR);
                    continue;
                }

                // Copiar datos del stream a un vector
                size_t imgSize = memStream.GetSize();
                std::vector<char> imgData(imgSize);
                memcpy(imgData.data(), memStream.GetOutputStreamBuffer()->GetBufferStart(), imgSize);

                nuevasImagenes[imageName] = imgData;
            }
            else if (linea.type == ContenidoLinea::PURCHASE_INFO) {
                // Guardar en array separado de PURCHASE_INFO
                json infoObj;
                infoObj["content"] = std::string(linea.texto.utf8_str());
                infoObj["line"] = linea.numero;
                infoObj["type"] = linea.type;
                arrayEscPos.push_back(infoObj);
            }
            else { // ContenidoLinea::TEXT
                // Es texto normal
                json textObj;
                textObj["type"] = linea.type;
                textObj["content"] = std::string(linea.texto.utf8_str());
                textObj["line"] = linea.numero;
                arrayEscPos.push_back(textObj);
            }
        }

        // AGREGAR los arrays al JSON
        root["itemsEscPos"] = arrayEscPos;

        // 4. CREAR el nuevo ZIP con TODO el contenido
        wxFileOutputStream fileOut(TICKET_PATH);
        if (!fileOut.IsOk()) {
            wxMessageBox(_("The ticket file could not be written"), "Error", wxOK | wxICON_ERROR);
            return false;
        }

        wxZipOutputStream zipOut(fileOut);

        // Escribir el data.json modificado
        std::string jsonModificado = root.dump(4);
        zipOut.PutNextEntry("data.json");
        zipOut.Write(jsonModificado.c_str(), jsonModificado.length());
        zipOut.CloseEntry();

        // Copiar todas las entradas ANTERIORES 
        // (esto incluye data.json antiguo e imÃÂÃÂ¡genes del Canvas, pero NO las imÃÂÃÂ¡genes viejas de EscPos)
        for (const auto& entrada : entradasAnteriores) {
            zipOut.PutNextEntry(entrada.first);
            zipOut.Write(entrada.second.data(), entrada.second.size());
            zipOut.CloseEntry();
        }

        // Agregar las NUEVAS imÃÂÃÂ¡genes de EscPos
        for (const auto& imagen : nuevasImagenes) {
            zipOut.PutNextEntry(imagen.first);
            zipOut.Write(imagen.second.data(), imagen.second.size());
            zipOut.CloseEntry();
        }

        zipOut.Close();
        fileOut.Close();

        return true;
    }
    catch (const json::exception& e) {
        wxMessageBox(wxString::Format("Error JSON: %s", e.what()),
            "Error", wxOK | wxICON_ERROR);
        return false;
    }
    catch (const std::exception& e) {
        wxMessageBox(wxString::Format("Exception: %s", e.what()),
            "Error", wxOK | wxICON_ERROR);
        return false;
    }
    catch (...) {
        wxMessageBox("Unkonwn error saving ticket", "Error", wxOK | wxICON_ERROR);
        return false;
    }
}
