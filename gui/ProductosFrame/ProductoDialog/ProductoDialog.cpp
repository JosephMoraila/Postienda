// ProductoDialog.cpp
#include "gui/ProductosFrame/ProductoDialog/ProductoDialog.h"
#include <wx/wx.h>
#include "utils/window/WindowUtils.h"
#include "constants/MESSAGES_ADVICE/WARNING/WARNING_MESSAGES.hpp"
using namespace WARNING_MESSAGES;

ProductoDialog::ProductoDialog(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, _("New product"), wxDefaultPosition, wxSize(350, 300)) {
    AplicarIconoPrincipal(this);
    Widgets();
}

void ProductoDialog::Widgets() {
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Campo Nombre
    mainSizer->Add(new wxStaticText(this, wxID_ANY, _("Name:")), 0, wxALL, 5);
    txtNombre = new wxTextCtrl(this, wxID_ANY);
    mainSizer->Add(txtNombre, 0, wxEXPAND | wxALL, 5);

    // Campo Precio
    precioLabel = new wxStaticText(this, wxID_ANY, _("Price:"));
    mainSizer->Add(precioLabel, 0, wxALL, 5);
    txtPrecio = new wxTextCtrl(this, wxID_ANY);
    txtPrecio->Bind(wxEVT_TEXT, [this](wxCommandEvent&) {
        FormatTextCtrlWithCommas(txtPrecio);
        });
    mainSizer->Add(txtPrecio, 0, wxEXPAND | wxALL, 5);

    // Etiqueta "Precio por kilo" (inicialmente oculta)


    // NUEVO: Campo CÃÂ³digo de Barras
    mainSizer->Add(new wxStaticText(this, wxID_ANY, _("Barcode (optional):")), 0, wxALL, 5);
    txtCodigoBarras = new wxTextCtrl(this, wxID_ANY);
    txtCodigoBarras->SetHint("Ex: 7501055363803");
    mainSizer->Add(txtCodigoBarras, 0, wxEXPAND | wxALL, 5);

    // Texto de ayuda
    wxStaticText* helpText = new wxStaticText(this, wxID_ANY, _("The barcode must contain only numbers and letters (maximum 20 characters)."));
    helpText->SetFont(helpText->GetFont().Smaller());
    helpText->SetForegroundColour(wxColour(100, 100, 100));
    mainSizer->Add(helpText, 0, wxALL | wxEXPAND, 5);

    // Checkbox para indicar si es por peso
    chkPorPeso = new wxCheckBox(this, wxID_ANY, _("Product by weight?"));
    chkPorPeso->SetValue(false); // Por defecto no es por peso
    // Evento cuando el mouse entra
    chkPorPeso->Bind(wxEVT_ENTER_WINDOW, [this](wxMouseEvent& event) {
        chkPorPeso->SetCursor(wxCursor(wxCURSOR_HAND)); // Cambia a mano
        event.Skip(); // Permite que otros eventos tambiÃÂ©n se procesen
        });

    // Evento cuando el mouse sale
    chkPorPeso->Bind(wxEVT_LEAVE_WINDOW, [this](wxMouseEvent& event) {
        chkPorPeso->SetCursor(wxCursor(wxCURSOR_ARROW)); // Cambia a cursor normal
        event.Skip();
        });
    mainSizer->Add(chkPorPeso, 0, wxALL, 5);

    // Evento para mostrar/ocultar la etiqueta
    chkPorPeso->Bind(wxEVT_CHECKBOX, [=, this](wxCommandEvent& event) {
        if (chkPorPeso->IsChecked()) {
            precioLabel->SetLabel(_("Price per kilo:"));
            wxFont font = precioLabel->GetFont();
            font.SetWeight(wxFONTWEIGHT_BOLD);
            precioLabel->SetFont(font);
        }
        else {
            precioLabel->SetLabel(_("Price:"));
            wxFont font = precioLabel->GetFont();
            font.SetWeight(wxFONTWEIGHT_NORMAL);  // quita la negrita
            precioLabel->SetFont(font);
        }


        mainSizer->Layout(); // Refresca el diseÃÂ±o
        mainSizer->Fit(this); // Redimensiona el diÃÂ¡logo segÃÂºn contenido
        });

    // Botones personalizados
    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    btnOk = new wxButton(this, wxID_OK, _("Accept"), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    btnCancel = new wxButton(this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);

    buttonSizer->Add(btnOk, 0, wxRIGHT, 10);
    buttonSizer->Add(btnCancel, 0);

    mainSizer->Add(buttonSizer, 0, wxALL | wxALIGN_RIGHT, 10);

    // Eventos
    Bind(wxEVT_BUTTON, &ProductoDialog::OnAceptar, this, wxID_OK);

    // Hover en botones
    btnOk->Bind(wxEVT_ENTER_WINDOW, &ProductoDialog::OnButtonEnter, this);
    btnOk->Bind(wxEVT_LEAVE_WINDOW, &ProductoDialog::OnButtonLeave, this);
    btnCancel->Bind(wxEVT_ENTER_WINDOW, &ProductoDialog::OnButtonEnter, this);
    btnCancel->Bind(wxEVT_LEAVE_WINDOW, &ProductoDialog::OnButtonLeave, this);

    SetSizer(mainSizer);
    Layout();
    Centre();
    mainSizer->Fit(this);
}

wxString ProductoDialog::GetNombre() const {
    return txtNombre->GetValue();
}

double ProductoDialog::GetPrecio() const {
    wxString raw = txtPrecio->GetValue();
    // Quitamos todas las comas
    raw.Replace(",", "");
    // Convertimos a double
    double precio = 0.0;
    raw.ToDouble(&precio);
    return precio;
}


wxString ProductoDialog::GetCodigoBarras() const {
	return txtCodigoBarras->GetValue().Trim().Trim(false); //Se pone dos veces para eliminar espacios al inicio y al final
}

bool ProductoDialog::EsPorPeso() const {
    return chkPorPeso->IsChecked();
}

void ProductoDialog::OnAceptar(wxCommandEvent& event) {
    // Validar nombre
    wxString nombre = txtNombre->GetValue().Trim(true).Trim(false);
	nombre.Replace("\t", " ");// Reemplazar tabulaciones (Espacios grandes) por un espacio
    while (nombre.Replace("  ", " ")) {}

    if (nombre.IsEmpty()) {
        wxMessageBox(_("The product name cannot be empty or contain only spaces."), WARNING, wxOK | wxICON_WARNING);
        return;
    }

    txtNombre->SetValue(nombre);

    // Validar precio
    wxString raw = txtPrecio->GetValue();
    raw.Replace(",", "");  // quitar comas
    double precio = 0.0;
    if (!raw.ToDouble(&precio) || precio < 0) {
        wxMessageBox(_("The price must be a number greater than or equal to zero."), WARNING, wxOK | wxICON_WARNING);
        return;
    }

    wxString codigoBarras = txtCodigoBarras->GetValue().Trim().Trim(false);

    if (!codigoBarras.IsEmpty()) {
        // Limpiar cÃÂ³digo de barras (solo nÃÂºmeros y letras)
        wxString codigoLimpio;
        for (size_t i = 0; i < codigoBarras.length(); i++) {
           // Extrae el carÃÂ¡cter actual de la vuelta
            wxChar c = codigoBarras[i];
			// Verifica si el carÃÂ¡cter es alfanumÃÂ©rico
            if (wxIsalnum(c)) {
                codigoLimpio += c;
            }
        }

        // Validar longitud
        if (codigoLimpio.length() > 20) {
            wxMessageBox(_("The barcode cannot have more than 20 characters."), INVALID_CODEBAR, wxOK | wxICON_WARNING);
            return;
        }

        // Validar que no estÃÂ© vacÃÂ­o despuÃÂ©s de limpiar
        if (codigoLimpio.IsEmpty()) {
            wxMessageBox(_("The barcode must contain at least one number or letter."), INVALID_CODEBAR, wxOK | wxICON_WARNING);
            return;
        }

        // Actualizar el campo con el cÃÂ³digo limpio
        txtCodigoBarras->SetValue(codigoLimpio);
    }

	//EndModal y wxID_OK para cerrar el diÃÂ¡logo y devolver el resultado
    EndModal(wxID_OK);
}

void ProductoDialog::AplicarTema(bool oscuro) {
    temaOscuro = oscuro;

	ApplyTheme(this, oscuro);

    Refresh();
    Update();
}

void ProductoDialog::OnButtonEnter(wxMouseEvent& event) {
    wxButton* btn = dynamic_cast<wxButton*>(event.GetEventObject());
    SetButtonHover(btn, true, temaOscuro);
    event.Skip();
}

void ProductoDialog::OnButtonLeave(wxMouseEvent& event) {
    wxButton* btn = dynamic_cast<wxButton*>(event.GetEventObject());
    btn->SetCursor(wxCursor(wxCURSOR_ARROW));
    SetButtonHover(btn, false, temaOscuro);
    event.Skip();
}
