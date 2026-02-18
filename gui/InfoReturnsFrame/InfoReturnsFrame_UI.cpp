#include "gui/InfoReturnsFrame/InfoReturnsFrame.hpp"
#include "utils/window/WindowUtils.h"
#include "utils/MathUtils.hpp"
#include <wx/statline.h>

void InfoReturnsFrame::Widgets() {
    mainPanel = new wxPanel(this);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // ---- Panel contenedor SOLO para filtros ----
    wxPanel* filtersPanel = new wxPanel(mainPanel);
    wxBoxSizer* filtersSizer = new wxBoxSizer(wxVERTICAL);

    scrollWidgets = new wxScrolledWindow(filtersPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL);
    scrollWidgets->SetScrollRate(10, 0); // solo horizontal
    wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);
    scrollWidgets->SetSizer(topSizer);

    // ----- Widgets -------
    wxButton* searchButton = new wxButton(scrollWidgets, wxID_ANY, _("Search"), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    searchButton->Bind(wxEVT_ENTER_WINDOW, [this](wxMouseEvent& event) {OnWidgetEnter(event, temaOscuro); });
    searchButton->Bind(wxEVT_LEAVE_WINDOW, [this](wxMouseEvent& event) {OnWidgetLeave(event, temaOscuro); });
    searchButton->Bind(wxEVT_BUTTON, &InfoReturnsFrame::OnSearch, this);

    wxStaticText* idLabel = new wxStaticText(scrollWidgets, wxID_ANY, "ID:");
    idLabel->SetToolTip(_("Searching only for the ID cancels all other filters"));
    IdInput = new wxTextCtrl(scrollWidgets, wxID_ANY, "", wxDefaultPosition, wxSize(80, -1));
    IdInput->SetToolTip(_("Searching only for the ID cancels all other filters"));

    wxStaticText* startDateLabel = new wxStaticText(scrollWidgets, wxID_ANY, _("Start date:"));
    startDatePicker = new wxDatePickerCtrl(scrollWidgets, wxID_ANY, wxDefaultDateTime, wxDefaultPosition, wxDefaultSize, wxDP_DROPDOWN | wxDP_SHOWCENTURY);

    wxDateTime zeroStartTime = wxDateTime::Now(); zeroStartTime.SetHour(0); zeroStartTime.SetMinute(0); zeroStartTime.SetSecond(0);//We set to 00:00 to the start time by default
    wxStaticText* startLabel = new wxStaticText(scrollWidgets, wxID_ANY, _("Start time:"));
    startTimePicker = new wxTimePickerCtrl(scrollWidgets, wxID_ANY, zeroStartTime);

    wxStaticText* endDateLabel = new wxStaticText(scrollWidgets, wxID_ANY, _("End date:"));
    endDatePicker = new wxDatePickerCtrl(scrollWidgets, wxID_ANY, wxDefaultDateTime, wxDefaultPosition, wxDefaultSize, wxDP_DROPDOWN | wxDP_SHOWCENTURY);

    wxDateTime endEndTime = wxDateTime::Now(); endEndTime.SetHour(23); endEndTime.SetMinute(59); endEndTime.SetSecond(59);//We set to 23:59 to the end time by default
    wxStaticText* endLabel = new wxStaticText(scrollWidgets, wxID_ANY, _("End time:"));
    endTimePicker = new wxTimePickerCtrl(scrollWidgets, wxID_ANY, endEndTime);

    wxStaticText* minLabel = new wxStaticText(scrollWidgets, wxID_ANY, _("Minimum amount:"));
    minAmountInput = new wxTextCtrl(scrollWidgets, wxID_ANY, "", wxDefaultPosition, wxSize(80, -1));
    minAmountInput->Bind(wxEVT_TEXT, [this](wxCommandEvent&) {FormatTextCtrlWithCommas(minAmountInput); });

    wxStaticText* maxLabel = new wxStaticText(scrollWidgets, wxID_ANY, _("Maximum amount:"));
    maxAmountInput = new wxTextCtrl(scrollWidgets, wxID_ANY, "", wxDefaultPosition, wxSize(80, -1));
    maxAmountInput->Bind(wxEVT_TEXT, [this](wxCommandEvent&) {FormatTextCtrlWithCommas(maxAmountInput); });

    wxStaticText* productNameLabel = new wxStaticText(scrollWidgets, wxID_ANY, _("Product name:"));
    productNameInput = new wxTextCtrl(scrollWidgets, wxID_ANY, "", wxDefaultPosition, wxSize(120, -1));

    wxStaticText* minQuantityLabel = new wxStaticText(scrollWidgets, wxID_ANY, _("Minimum Quantity:"));
    minQuantityInput = new wxTextCtrl(scrollWidgets, wxID_ANY, "", wxDefaultPosition, wxSize(120, -1));

    wxStaticText* maxQuantityLabel = new wxStaticText(scrollWidgets, wxID_ANY, _("Maximum Quantity:"));
    maxQuantityInput = new wxTextCtrl(scrollWidgets, wxID_ANY, "", wxDefaultPosition, wxSize(120, -1));

    wxStaticText* workerLabel = new wxStaticText(scrollWidgets, wxID_ANY, _("Worker:"));
    workerInput = new wxTextCtrl(scrollWidgets, wxID_ANY, "", wxDefaultPosition, wxSize(120, -1));

    wxStaticText* purchaseIdLabel = new wxStaticText(scrollWidgets, wxID_ANY, _("Purchase ID:"));
    purchaseIdInput = new wxTextCtrl(scrollWidgets, wxID_ANY, "", wxDefaultPosition, wxSize(80, -1));
    purchaseIdInput->SetToolTip(_("Searching only for the ID cancels all other filters"));

    totaMonetReturnByFilterLabel = new wxStaticText(mainPanel, wxID_ANY, wxString::Format(_("Returned: %.2f"), totaMonetReturnByFilter));

    wxBoxSizer* totalsSizer = new wxBoxSizer(wxHORIZONTAL);
    int spacing = FromDIP(40);   // Espacio adaptable
    int lineHeight = FromDIP(28);
    int lineWidth = FromDIP(2);

    // ---- Income ----
    totalsSizer->Add(totaMonetReturnByFilterLabel, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, spacing);
    // Línea separadora
    totalsSizer->Add(new wxStaticLine(mainPanel, wxID_ANY, wxDefaultPosition, wxSize(lineWidth, lineHeight), wxLI_VERTICAL), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, spacing);

    //----- Add widgets to sizers -----
    topSizer->Add(searchButton, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    topSizer->Add(idLabel, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    topSizer->Add(IdInput, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    topSizer->Add(startDateLabel, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    topSizer->Add(startDatePicker, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    topSizer->Add(startLabel, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    topSizer->Add(startTimePicker, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    topSizer->Add(endDateLabel, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    topSizer->Add(endDatePicker, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    topSizer->Add(endLabel, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    topSizer->Add(endTimePicker, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    topSizer->Add(minLabel, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    topSizer->Add(minAmountInput, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    topSizer->Add(maxLabel, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    topSizer->Add(maxAmountInput, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    topSizer->Add(productNameLabel, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    topSizer->Add(productNameInput, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    topSizer->Add(minQuantityLabel, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    topSizer->Add(minQuantityInput, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    topSizer->Add(maxQuantityLabel, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    topSizer->Add(maxQuantityInput, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    topSizer->Add(workerLabel, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    topSizer->Add(workerInput, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    topSizer->Add(purchaseIdLabel, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    topSizer->Add(purchaseIdInput, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

    // ---- Ajustar scroll solo dentro de su panel ----
    filtersSizer->Add(scrollWidgets, 1, wxEXPAND | wxALL, 0);
    filtersPanel->SetSizer(filtersSizer);

    // ---- Tabla (fuera del scroll totalmente) ----
    list = new wxListCtrl(mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL);
    list->InsertColumn(0, "ID", wxLIST_FORMAT_LEFT, 50);
    list->InsertColumn(1, _("Date"), wxLIST_FORMAT_LEFT, 70);
    list->InsertColumn(2, _("Time"), wxLIST_FORMAT_LEFT, 100);
    list->InsertColumn(3, _("Product name"), wxLIST_FORMAT_LEFT, 100);
    list->InsertColumn(4, _("Amount"), wxLIST_FORMAT_LEFT, 150);
    list->InsertColumn(5, _("Quantity"), wxLIST_FORMAT_LEFT, 150);
    list->InsertColumn(6, _("Worker"), wxLIST_FORMAT_LEFT, 100);
    list->InsertColumn(7, _("Purchase ID"), wxLIST_FORMAT_LEFT, 150);

    // ---- Panel de botones de paginaciÃÂ³n ----
    bottomPanel = new wxPanel(mainPanel);
    wxBoxSizer* bottomSizer = new wxBoxSizer(wxHORIZONTAL);

    prevButton = new wxButton(bottomPanel, wxID_ANY, _("Load previous"), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    prevButton->Bind(wxEVT_ENTER_WINDOW, [this](wxMouseEvent& event) {OnWidgetEnter(event, temaOscuro); });
    prevButton->Bind(wxEVT_LEAVE_WINDOW, [this](wxMouseEvent& event) {OnWidgetLeave(event, temaOscuro); });

    pageLabel = new wxStaticText(bottomPanel, wxID_ANY, _("Page: -/-"), wxDefaultPosition, wxSize(100, -1), wxALIGN_CENTER);
    wxFont font = pageLabel->GetFont();
    font.SetPointSize(10);
    pageLabel->SetFont(font);

    nextButton = new wxButton(bottomPanel, wxID_ANY, _("Load more"), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    nextButton->Bind(wxEVT_ENTER_WINDOW, [this](wxMouseEvent& event) {OnWidgetEnter(event, temaOscuro); });
    nextButton->Bind(wxEVT_LEAVE_WINDOW, [this](wxMouseEvent& event) {OnWidgetLeave(event, temaOscuro); });

    // por defecto desactivados
    prevButton->Disable();
    nextButton->Disable();

    // opcional: asignar eventos
    prevButton->Bind(wxEVT_BUTTON, &InfoReturnsFrame::OnLoadPrev, this);
    nextButton->Bind(wxEVT_BUTTON, &InfoReturnsFrame::OnLoadNext, this);

    bottomSizer->AddStretchSpacer(1);
    bottomSizer->Add(prevButton, 0, wxALL, 5);
    bottomSizer->Add(pageLabel, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    bottomSizer->Add(nextButton, 0, wxALL, 5);
    bottomSizer->AddStretchSpacer(1);
    bottomPanel->SetSizer(bottomSizer);

    // ---- Agregar todo al sizer principal ----
    mainSizer->Add(filtersPanel, 0, wxEXPAND | wxALL, 5);
    mainSizer->Add(totalsSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, FromDIP(5));
    mainSizer->Add(list, 1, wxEXPAND | wxALL, 5);
    mainSizer->Add(bottomPanel, 0, wxALIGN_CENTER | wxBOTTOM, 5);

    mainPanel->SetSizer(mainSizer);
    mainSizer->SetSizeHints(this);

}


void InfoReturnsFrame::UpdateTotalMoneyReturnLabel() {
	totaMonetReturnByFilterLabel->SetLabel(wxString::Format(_("Returned: %s"), FormatFloatWithCommas(totaMonetReturnByFilter)));
}


void InfoReturnsFrame::AjustarColumnasListCtrl(){
    if (list) {
        wxSize clientSize = list->GetClientSize();
        int totalWidth = clientSize.GetWidth();

        if (totalWidth > 0) {
            int idWidth = 0;
            int dateWidth = 0;
            int timeWidth = 0;
            int productNameWidth = 0;
            int amountWidth = 0;
            int workerWidth = 0;
            int quantityWidth = 0;
            int purchaseIdWidth = 0;

            // Restar ancho del scrollbar si existe
            wxSize virtualSize = list->GetVirtualSize();
            if (virtualSize.GetHeight() > clientSize.GetHeight())
                totalWidth -= wxSystemSettings::GetMetric(wxSYS_VSCROLL_X);

            if (!IsMaximized()) {
                // DistribuciÃÂ³n en modo normal
                idWidth = static_cast<int>(totalWidth * 0.10);
                dateWidth = static_cast<int>(totalWidth * 0.15);
                timeWidth = static_cast<int>(totalWidth * 0.12);
                amountWidth = static_cast<int>(totalWidth * 0.10);
                productNameWidth = static_cast<int>(totalWidth * 0.20);
                workerWidth = static_cast<int>(totalWidth * 0.10);
                quantityWidth = static_cast<int>(totalWidth * 0.10);
                purchaseIdWidth = totalWidth - idWidth - dateWidth - timeWidth - amountWidth - productNameWidth - workerWidth - quantityWidth;
            }
            else {
                // DistribuciÃÂ³n en modo maximizado
                idWidth = static_cast<int>(totalWidth * 0.10);
                dateWidth = static_cast<int>(totalWidth * 0.15);
                timeWidth = static_cast<int>(totalWidth * 0.12);
                amountWidth = static_cast<int>(totalWidth * 0.10);
                productNameWidth = static_cast<int>(totalWidth * 0.20);
                workerWidth = static_cast<int>(totalWidth * 0.10);
                quantityWidth = static_cast<int>(totalWidth * 0.10);
                purchaseIdWidth = totalWidth - idWidth - dateWidth - timeWidth - amountWidth - productNameWidth - workerWidth - quantityWidth;
            }

            // Anchos mÃÂ­nimos
            idWidth = wxMax(100, idWidth);
            dateWidth = wxMax(120, dateWidth);
            timeWidth = wxMax(120, timeWidth);
            amountWidth = wxMax(100, amountWidth);
            productNameWidth = wxMax(200, productNameWidth);
            workerWidth = wxMax(100, workerWidth);
            quantityWidth = wxMax(100, quantityWidth);
            purchaseIdWidth = wxMax(150, purchaseIdWidth);

            // Asignar a columnas (ajusta el ÃÂ­ndice si el orden es diferente)
            list->SetColumnWidth(0, idWidth);
            list->SetColumnWidth(1, dateWidth);
            list->SetColumnWidth(2, timeWidth);
            list->SetColumnWidth(3, productNameWidth);
            list->SetColumnWidth(4, amountWidth);
            list->SetColumnWidth(5, quantityWidth);
            list->SetColumnWidth(6, workerWidth);
            list->SetColumnWidth(7, purchaseIdWidth);

        }
    }
}
