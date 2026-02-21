#include "gui/ActionsFrame/ActionsFrame.hpp"
#include "utils/window/WindowUtils.h"
#include "utils/MathUtils.hpp"
#include <wx/statline.h>

void ActionsFrame::Widgets() {

    mainPanel = new wxPanel(this);

    // Sizer del frame
    wxBoxSizer* frameSizer = new wxBoxSizer(wxVERTICAL);
    frameSizer->Add(mainPanel, 1, wxEXPAND);
    SetSizer(frameSizer);

    // Sizer del panel
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    wxStaticText* textoDescription = new wxStaticText(mainPanel, wxID_ANY, _("Watch what happened between dates"));
    wxFont font = textoDescription->GetFont();
    font.SetPointSize(12);
    textoDescription->SetFont(font);
    mainSizer->Add(textoDescription, 0, wxEXPAND | wxALL, 10);

    buttonPanel = new wxPanel(mainPanel);
    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);

    wxButton* printButton = new wxButton(buttonPanel, wxID_ANY, _("Print"), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    printButton->Bind(wxEVT_ENTER_WINDOW, [this](wxMouseEvent& event) {OnWidgetEnter(event, temaOscuro); });
    printButton->Bind(wxEVT_LEAVE_WINDOW, [this](wxMouseEvent& event) {OnWidgetLeave(event, temaOscuro); });
    printButton->Bind(wxEVT_BUTTON, &ActionsFrame::OnPrintTicket, this);
    buttonSizer->Add(printButton, 0, wxALL, 5);

    grossCashLabel = new wxStaticText(buttonPanel, wxID_ANY, wxString::Format(_("Gross Cash: %s"), FormatFloatWithCommas(grossCash)));
    totalMoneyCardLabel = new wxStaticText(buttonPanel, wxID_ANY, wxString::Format(_("Card Income: %s"), FormatFloatWithCommas(totalMoneyCard)));
    manalAdditionsLabel = new wxStaticText(buttonPanel, wxID_ANY, wxString::Format(_("Manual Income: %s"), FormatFloatWithCommas(manualAdditions)));
    manalWithdrawalsLabel = new wxStaticText(buttonPanel, wxID_ANY, wxString::Format(_("Manual Withdrawals: %s"), FormatFloatWithCommas(manualWithdrawals)));
    returnsMoneyLabel = new wxStaticText(buttonPanel, wxID_ANY, wxString::Format(_("Returned: %s"), FormatFloatWithCommas(returnsMoney)));
    netCashLabel = new wxStaticText(buttonPanel, wxID_ANY, wxString::Format(_("Net Cash: %s"), FormatFloatWithCommas(netCash)));
    totalLabel = new wxStaticText(buttonPanel, wxID_ANY, wxString::Format(_("Total: %s"), FormatFloatWithCommas(total)));
    int spacing = FromDIP(40);   // Espacio adaptable
    int lineHeight = FromDIP(28);
    int lineWidth = FromDIP(2);

    buttonSizer->Add(totalMoneyCardLabel, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, spacing);
    // Línea separadora
    buttonSizer->Add(new wxStaticLine(buttonPanel, wxID_ANY, wxDefaultPosition, wxSize(lineWidth, lineHeight), wxLI_VERTICAL), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, spacing);
    buttonSizer->Add(manalAdditionsLabel, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, spacing);
    buttonSizer->Add(new wxStaticLine(buttonPanel, wxID_ANY, wxDefaultPosition, wxSize(lineWidth, lineHeight), wxLI_VERTICAL), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, spacing);
    buttonSizer->Add(manalWithdrawalsLabel, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, spacing);
    // Línea separadora
    buttonSizer->Add(new wxStaticLine(buttonPanel, wxID_ANY, wxDefaultPosition, wxSize(lineWidth, lineHeight), wxLI_VERTICAL), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, spacing);
    buttonSizer->Add(returnsMoneyLabel, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, spacing);
    buttonSizer->Add(new wxStaticLine(buttonPanel, wxID_ANY, wxDefaultPosition, wxSize(lineWidth, lineHeight), wxLI_VERTICAL), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, spacing);
    buttonSizer->Add(grossCashLabel, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, spacing);
    buttonSizer->Add(new wxStaticLine(buttonPanel, wxID_ANY, wxDefaultPosition, wxSize(lineWidth, lineHeight), wxLI_VERTICAL), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, spacing);
    buttonSizer->Add(netCashLabel, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, spacing);
    buttonSizer->Add(new wxStaticLine(buttonPanel, wxID_ANY, wxDefaultPosition, wxSize(lineWidth, lineHeight), wxLI_VERTICAL), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, spacing);
    // ---- Total ----
    buttonSizer->Add(totalLabel, 0, wxALIGN_CENTER_VERTICAL);

    buttonPanel->SetSizer(buttonSizer);
    mainSizer->Add(buttonPanel, 0, wxEXPAND | wxALL, 5);

    filtersPanel = new wxPanel(mainPanel);
    wxBoxSizer* filtersSizer = new wxBoxSizer(wxHORIZONTAL);

    wxButton* searchButton = new wxButton(filtersPanel, wxID_ANY, _("Search"), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    searchButton->Bind(wxEVT_ENTER_WINDOW, [this](wxMouseEvent& event) {OnWidgetEnter(event, temaOscuro); });
    searchButton->Bind(wxEVT_LEAVE_WINDOW, [this](wxMouseEvent& event) {OnWidgetLeave(event, temaOscuro); });
    searchButton->Bind(wxEVT_BUTTON, &ActionsFrame::OnSearch, this);
    filtersSizer->Add(searchButton, 0, wxALL, 5);

    wxStaticText* startDateLabel = new wxStaticText(filtersPanel, wxID_ANY, _("Start date:"));
    filtersSizer->Add(startDateLabel, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    startDatePicker = new wxDatePickerCtrl(filtersPanel, wxID_ANY, wxDefaultDateTime, wxDefaultPosition, wxDefaultSize, wxDP_DROPDOWN | wxDP_SHOWCENTURY);
    filtersSizer->Add(startDatePicker, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

    wxDateTime zeroStartTime = wxDateTime::Now(); zeroStartTime.SetHour(0); zeroStartTime.SetMinute(0); zeroStartTime.SetSecond(0);//We set to 00:00 to the start time by default
    wxStaticText* startLabel = new wxStaticText(filtersPanel, wxID_ANY, _("Start time:"));
    filtersSizer->Add(startLabel, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    startTimePicker = new wxTimePickerCtrl(filtersPanel, wxID_ANY, zeroStartTime);
    filtersSizer->Add(startTimePicker, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

    wxStaticText* endDateLabel = new wxStaticText(filtersPanel, wxID_ANY, _("End date:"));
    filtersSizer->Add(endDateLabel, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    endDatePicker = new wxDatePickerCtrl(filtersPanel, wxID_ANY, wxDefaultDateTime, wxDefaultPosition, wxDefaultSize, wxDP_DROPDOWN | wxDP_SHOWCENTURY);
    filtersSizer->Add(endDatePicker, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

    wxDateTime endEndTime = wxDateTime::Now(); endEndTime.SetHour(23); endEndTime.SetMinute(59); endEndTime.SetSecond(59);//We set to 23:59 to the end time by default
    wxStaticText* endLabel = new wxStaticText(filtersPanel, wxID_ANY, _("End time:"));
    filtersSizer->Add(endLabel, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    endTimePicker = new wxTimePickerCtrl(filtersPanel, wxID_ANY, endEndTime);
    filtersSizer->Add(endTimePicker, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

    filtersPanel->SetSizer(filtersSizer);
    mainSizer->Add(filtersPanel, 0, wxEXPAND | wxALL, 5);

    //Tree:
    treeDays = new wxTreeCtrl(mainPanel, wxID_ANY,wxDefaultPosition, wxDefaultSize,wxTR_HIDE_ROOT | wxTR_DEFAULT_STYLE | wxTR_MULTIPLE | wxTR_HAS_BUTTONS);
    root = treeDays->AddRoot("root");
    wxFont fontTree = treeDays->GetFont();
    fontTree.SetPointSize(12); // tamaño que quieras
    treeDays->SetFont(fontTree);
    mainSizer->Add(treeDays, 1, wxEXPAND | wxALL, 10);

    mainPanel->SetSizer(mainSizer);

    Layout();
    SetMinSize(wxSize(800, 600));
    CenterOnParent();
}


void ActionsFrame::UpdateLabelsMoney() {
    grossCashLabel->SetLabel(wxString::Format(_("Gross Income: %s"), FormatFloatWithCommas(grossCash)));
    totalMoneyCardLabel->SetLabel(wxString::Format(_("Card Income: %s"), FormatFloatWithCommas(totalMoneyCard)));
    manalAdditionsLabel->SetLabel(wxString::Format(_("Manual Income: %s"), FormatFloatWithCommas(manualAdditions)));
    manalWithdrawalsLabel->SetLabel(wxString::Format(_("Manual Withdrawals: %s"), FormatFloatWithCommas(manualWithdrawals)));
    returnsMoneyLabel->SetLabel(wxString::Format(_("Returned: %s"), FormatFloatWithCommas(returnsMoney)));
    netCashLabel->SetLabel(wxString::Format(_("Net Cash: %s"), FormatFloatWithCommas(netCash)));
    totalLabel->SetLabel(wxString::Format(_("Total: %s"), FormatFloatWithCommas(total)));
}

