#include "gui/TicketFrame/Canvas/InputTextModal/InputTextModal.hpp"
#include "utils/window/WindowUtils.h"

TextDialog::TextDialog(wxWindow* parent, const wxString& title) : wxDialog(parent, wxID_ANY, title, wxDefaultPosition, wxSize(1000, 800), wxRESIZE_BORDER)
{
    // Panel principal
    panel = new wxPanel(this, wxID_ANY);
    mainSizer = new wxBoxSizer(wxVERTICAL);

    // Campo de texto multilinea con tamaÃÂ±o explÃÂ­cito
    textCtrl = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(500, 600), wxTE_MULTILINE | wxTE_RICH2);
    mainSizer->Add(textCtrl, 1, wxEXPAND | wxALL, 10);

	checkboxUnderInfo = new wxCheckBox(panel, wxID_ANY, _("Position below purchase information"));
	checkboxUnderInfo->Bind(wxEVT_ENTER_WINDOW, [this](wxMouseEvent& event) { OnWidgetEnter(event, temaOscuro); });
	checkboxUnderInfo->Bind(wxEVT_LEAVE_WINDOW, [this](wxMouseEvent& event) { OnWidgetLeave(event, temaOscuro); });
    checkboxUnderInfo->SetValue(false);
    checkboxUnderInfo->SetToolTip(_("Activate to place text in the position it is in, but it will be placed below the purchase receipt.\nSince the purchase information has a variable size."));
	mainSizer->Add(checkboxUnderInfo, 0, wxALL, 5);

    // Panel para los botones
    buttonPanel = new wxPanel(panel, wxID_ANY);
    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);

    wxButton* btnOk = new wxButton(buttonPanel, wxID_OK, _("Accept"), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    wxButton* buttonCancel = new wxButton(buttonPanel, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);

    btnOk->Bind(wxEVT_ENTER_WINDOW, [this](wxMouseEvent& event) { OnButtonEnter(event, temaOscuro); });
    btnOk->Bind(wxEVT_LEAVE_WINDOW, [this](wxMouseEvent& event) { OnButtonLeave(event, temaOscuro); });
    buttonCancel->Bind(wxEVT_ENTER_WINDOW, [this](wxMouseEvent& event) { OnButtonEnter(event, temaOscuro); });
    buttonCancel->Bind(wxEVT_LEAVE_WINDOW, [this](wxMouseEvent& event) { OnButtonLeave(event, temaOscuro); });

    buttonSizer->Add(btnOk, 0, wxALL, 5);
    buttonSizer->Add(buttonCancel, 0, wxALL, 5);

    buttonPanel->SetSizer(buttonSizer);
    mainSizer->Add(buttonPanel, 0, wxALIGN_CENTER | wxALL, 10);

    panel->SetSizer(mainSizer);

    // Sizer principal del diÃÂ¡logo
    wxBoxSizer* dialogSizer = new wxBoxSizer(wxVERTICAL);
    dialogSizer->Add(panel, 1, wxEXPAND | wxALL, 0);
    SetSizer(dialogSizer);
	SetMinSize(wxSize(100, 500)); // Ancho mÃÂ­nimo para evitar que el diÃÂ¡logo sea demasiado estrecho
    SetSize(wxSize(300, 600)); //TamaÃÂ±o para impresora termica de 58 mm
}

void TextDialog::AplicarTema(bool oscuro) {
    temaOscuro = oscuro;

    ApplyTheme(this, oscuro);       // Aplica tema al diÃÂ¡logo y a todos los hijos (incluido panel y botones)
    ApplyTheme(buttonPanel, oscuro);
    ApplyTheme(textCtrl, oscuro);
	ApplyTheme(checkboxUnderInfo, oscuro);
    Refresh();
    Update();
}

wxString TextDialog::GetText() const {
    wxString text = textCtrl->GetValue();
    // Este WHILE se repite hasta que no haya mÃÂ¡s \n al inicio
    while (text.StartsWith("\n")) {
        text = text.Mid(1);  // Quita UN \n, luego vuelve a verificar
    }
    // Este WHILE se repite hasta que no haya mÃÂ¡s \n al final
    while (text.EndsWith("\n")) {
        text.RemoveLast();  // Quita UN \n, luego vuelve a verificar
    }
    return text;
}

void TextDialog::SetText(wxString texto) const {
    textCtrl->SetValue(texto);
}

bool TextDialog::GetIsUnderInfo() const {
    return checkboxUnderInfo->GetValue();
}

void TextDialog::SetIsUnderInfo(bool underInfo) {
    checkboxUnderInfo->SetValue(underInfo);
}