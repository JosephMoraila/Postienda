#include "gui/AskEnterPasswordFrame/AskEnterPasswordFrame.hpp"
#include "utils/window/WindowUtils.h"

void AskEnterPasswordFrame::Widgets() {
    mainPanel = new wxPanel(this, wxID_ANY);
    wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

    wxStaticText* label = new wxStaticText(mainPanel, wxID_ANY, _("Enter password:"));

    textInputPassword = new wxTextCtrl(mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(30, -1), wxTE_PASSWORD | wxTE_PROCESS_ENTER);
    textInputPassword->Bind(wxEVT_TEXT_ENTER, &AskEnterPasswordFrame::OnPasswordEnter, this);

    wxButton* okButton = new wxButton(mainPanel, wxID_ANY, _("Accept"), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    okButton->Bind(wxEVT_ENTER_WINDOW, [this](wxMouseEvent& event) { OnWidgetEnter(event, temaOscuro); });
    okButton->Bind(wxEVT_LEAVE_WINDOW, [this](wxMouseEvent& event) { OnWidgetLeave(event, temaOscuro); });
    okButton->Bind(wxEVT_BUTTON, &AskEnterPasswordFrame::OnPasswordEnter, this);

    messageOnEnter = new wxStaticText(mainPanel, wxID_ANY, "");

    vbox->Add(label, 0, wxALL | wxALIGN_CENTER, 10);
    vbox->Add(textInputPassword, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);
    vbox->Add(messageOnEnter, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);
    vbox->Add(okButton, 0, wxALL | wxALIGN_CENTER, 10);

    mainPanel->SetSizer(vbox);
    vbox->Fit(mainPanel);             // Ajusta el panel al tamaño del sizer
    this->SetSize(wxSize(500, 300));
    this->Centre();
}

void AskEnterPasswordFrame::AplicarTema(bool oscuro) {
    temaOscuro = oscuro;
    ApplyTheme(mainPanel, temaOscuro);
}