#include "gui/TicketFrame/PrintMenu/PrintMenu.hpp"
#include "utils/window/WindowUtils.h"
#include <wx/file.h>
#include "constants/JSON/Ticket/PRINTER_SETTINGS.hpp"
#include "print/EscPos/EscPosDictionary.hpp"
#include <wx/clipbrd.h>
#include <wx/tipwin.h>
#include "constants/TICKET/TICKET_PATH.hpp"
#include "print/Print_Utils_Getters.hpp"
#include "constants/MESSAGES_ADVICE/ERROR/PRINTING/PRINTING_ERRORS.hpp"
using namespace PRINTING_ERROR_MESSAGES;

PrintMenu::PrintMenu(wxWindow* parent, const wxString& title)
    : wxDialog(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize)
{
    AplicarIconoPrincipal(this);
    Widgets();
    this->Bind(wxEVT_CLOSE_WINDOW, &PrintMenu::OnCloseSave, this);
}

void PrintMenu::Widgets() {
    panel = new wxPanel(this, wxID_ANY);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

#ifdef _WIN32
    GetPrinterNamesWindows();
#elif __linux__
    GetPrinterNamesLinux();
#endif

    const wxString NO_PRINTERS_LABEL = _("No printers were detected");
    if (this->printerNames.IsEmpty()) this->printerNames.Add(NO_PRINTERS_LABEL);

    mainSizer->Add(new wxStaticText(panel, wxID_ANY, _("Select a printer where the tickets will be printed:")), 0, wxALL, 5);

    m_printerChoice = new wxChoice(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, printerNames);
    mainSizer->Add(m_printerChoice, 0, wxALL | wxEXPAND, 5);

    // --- Cargar impresora previamente guardada ---
    if (wxFileExists(PRINT_SETTINGS_PATH)) savedPrinter = PrintGetters::GetSelectedPrinter();

    // Si existe la impresora guardada, seleccionarla automÃÂÃÂ¡ticamente
    int index = m_printerChoice->FindString(savedPrinter); //Retorna el ÃÂÃÂ­ndice del elemento que coincide con la cadena dada, o wxNOT_FOUND si no se encuentra
    if (index != wxNOT_FOUND) m_printerChoice->SetSelection(index); //Si la impresora guardada estÃÂÃÂ¡ en la lista, seleccionarla

    // --- Evento para guardar automÃÂÃÂ¡ticamente al cambiar ---
    m_printerChoice->Bind(wxEVT_CHOICE, [&](wxCommandEvent& event) {
        wxString selected = m_printerChoice->GetStringSelection();
        savedPrinter = selected;
        if (!selected.IsEmpty() && selected != NO_PRINTERS_LABEL) SaveSettingsPrinterName();
        });

    checkboxPrintTicket = new wxCheckBox(panel, wxID_ANY, _("Print receipt when making purchase"));
    checkboxPrintTicket->Bind(wxEVT_CHECKBOX, &PrintMenu::SaveSettingsPrinterCheckbox, this);
    if (wxFileExists(PRINT_SETTINGS_PATH)) LoadSettingsPrinterCheckbox();
    checkboxPrintTicket->Bind(wxEVT_ENTER_WINDOW, [this](wxMouseEvent& event) { OnWidgetEnter(event, temaOscuro); });
    checkboxPrintTicket->Bind(wxEVT_LEAVE_WINDOW, [this](wxMouseEvent& event) { OnWidgetLeave(event, temaOscuro); });
    mainSizer->Add(checkboxPrintTicket, 0, wxALL | wxALIGN_CENTER, 5);

    wxButton* btnTry = new wxButton(panel, wxID_ANY, _("Test print"), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    btnTry->Bind(wxEVT_BUTTON, &PrintMenu::OnTryTickets, this);
    btnTry->Bind(wxEVT_ENTER_WINDOW, [this](wxMouseEvent& event) {OnButtonEnter(event, temaOscuro); });
    btnTry->Bind(wxEVT_LEAVE_WINDOW, [this](wxMouseEvent& event) {OnButtonLeave(event, temaOscuro); });
    mainSizer->Add(btnTry, 0, wxALL | wxALIGN_CENTER, 5);
    
#ifdef __linux__
    mainSizer->Add(new wxStaticText(panel, wxID_ANY, _("If the ticket is not being printed, maybe you need\nto enable your user to use line printers on terminal.\nTry this command: ")), 0, wxALL, 5);
    wxTextCtrl* labelTryCommand = new wxTextCtrl(panel,wxID_ANY,"sudo usermod -a -G lp $USER",wxDefaultPosition,wxDefaultSize,wxTE_READONLY | wxBORDER_NONE | wxTE_CENTRE);
    mainSizer->Add(labelTryCommand, 0, wxALL | wxEXPAND, 5);
    wxButton* btnCopyCommand = new wxButton(panel, wxID_ANY, _("Copy command"), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    btnCopyCommand->Bind(wxEVT_ENTER_WINDOW, [this](wxMouseEvent& event) {OnWidgetEnter(event, temaOscuro); });
    btnCopyCommand->Bind(wxEVT_LEAVE_WINDOW, [this](wxMouseEvent& event) {OnWidgetLeave(event, temaOscuro); });
    btnCopyCommand->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event){
        if (wxTheClipboard->Open()){
            wxString textCommand = "sudo usermod -a -G lp $USER";
            wxTheClipboard->SetData(new wxTextDataObject(textCommand));
            wxTheClipboard->Close();
            wxTipWindow* tip = new wxTipWindow(this, _("Copied to clipboard"));
        }
    });
    mainSizer->Add(btnCopyCommand, 0, wxALL | wxALIGN_CENTER, 5);
#endif
    panel->SetSizer(mainSizer);
    mainSizer->Fit(this);
}

void PrintMenu::OnTryTickets(wxCommandEvent& event) {
    if (!m_printerChoice) return;
    const wxString NO_PRINTERS_LABEL = _("No printers were detected");

    wxString selectedPrinter = m_printerChoice->GetStringSelection();
    if (selectedPrinter.IsEmpty() || selectedPrinter == NO_PRINTERS_LABEL) {
        wxMessageBox(_("Please select a valid printer."), _("Selection Error"), wxICON_ERROR);
        return;
    }
    char mode = PrintGetters::GetMode();
    #ifdef _WIN32
    if (mode == 0) ImprimirCanvasPruebaWindows(selectedPrinter.wc_str());
    #elif __linux__
    if (mode == 0) wxMessageBox(_("No canvas available by the moment. Select EscPos"), _("No available"), wxOK | wxICON_WARNING);
    #endif
    else if (mode == 1) PrintEscPosPrueba(selectedPrinter.wc_str());
    else wxMessageBox(ERROR_GETTING_MODE_MESSAGE, "Error", wxOK | wxICON_ERROR);
}

void PrintMenu::AplicarTema(bool oscuro) {
    this->temaOscuro = oscuro;
    ApplyTheme(panel, oscuro);
    ApplyTheme(m_printerChoice, oscuro);
    Refresh();
    Update();
}