#include "gui/InfoProductsFrame/InfoProductsFrame.hpp"
#include "utils/window/WindowUtils.h"
#include "utils/MathUtils.hpp"

void InfoProductsFrame::Widgets() {
    mainPanel = new wxPanel(this);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // ---- Panel contenedor SOLO para filtros ----
    wxPanel* filtersPanel = new wxPanel(mainPanel);
    wxBoxSizer* filtersSizer = new wxBoxSizer(wxVERTICAL);

    scrollWidgets = new wxScrolledWindow(filtersPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL);
    scrollWidgets->SetScrollRate(10, 0); // solo horizontal
    wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);
    scrollWidgets->SetSizer(topSizer);

    // ---- Top filter widgets ----
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

    wxStaticText* minLabel = new wxStaticText(scrollWidgets, wxID_ANY, _("Minimum price:"));
    minAmountInput = new wxTextCtrl(scrollWidgets, wxID_ANY, "", wxDefaultPosition, wxSize(80, -1));
    minAmountInput->Bind(wxEVT_TEXT, [this](wxCommandEvent&) {FormatTextCtrlWithCommas(minAmountInput); });

    wxStaticText* maxLabel = new wxStaticText(scrollWidgets, wxID_ANY, _("Maximum price:"));
    maxAmountInput = new wxTextCtrl(scrollWidgets, wxID_ANY, "", wxDefaultPosition, wxSize(80, -1));
    maxAmountInput->Bind(wxEVT_TEXT, [this](wxCommandEvent&) {FormatTextCtrlWithCommas(maxAmountInput); });

    wxStaticText* productNaameBarcodeLabel = new wxStaticText(scrollWidgets, wxID_ANY, _("Product name/barcode:"));
    productNameBarcodeInput = new wxTextCtrl(scrollWidgets, wxID_ANY, "", wxDefaultPosition, wxSize(120, -1));

    descQuantityCheckbox = new wxCheckBox(scrollWidgets, wxID_ANY, _("Highest to lowest"));
    descQuantityCheckbox->SetValue(true);
    descQuantityCheckbox->Bind(wxEVT_ENTER_WINDOW, [this](wxMouseEvent& event) {descQuantityCheckbox->SetCursor(wxCursor(wxCURSOR_HAND)); event.Skip(); });
    descQuantityCheckbox->Bind(wxEVT_LEAVE_WINDOW, [this](wxMouseEvent& event) {descQuantityCheckbox->SetCursor(wxCursor(wxCURSOR_ARROW)); event.Skip();});


    wxButton* searchButton = new wxButton(scrollWidgets, wxID_ANY, _("Search"), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    searchButton->Bind(wxEVT_ENTER_WINDOW, [this](wxMouseEvent& event) {OnWidgetEnter(event, temaOscuro); });
    searchButton->Bind(wxEVT_LEAVE_WINDOW, [this](wxMouseEvent& event) {OnWidgetLeave(event, temaOscuro); });
    searchButton->Bind(wxEVT_BUTTON, &InfoProductsFrame::OnSearch, this);

    // ---- AÃÂ±adir al topSizer ----
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
    topSizer->Add(productNaameBarcodeLabel, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    topSizer->Add(productNameBarcodeInput, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    topSizer->Add(descQuantityCheckbox, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    topSizer->Add(searchButton, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

    // ---- Ajustar scroll solo dentro de su panel ----
    filtersSizer->Add(scrollWidgets, 1, wxEXPAND | wxALL, 0);
    filtersPanel->SetSizer(filtersSizer);

    // ---- Tabla (fuera del scroll totalmente) ----
    list = new wxListCtrl(mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL);
    list->InsertColumn(0, _("Product name"), wxLIST_FORMAT_LEFT, 150);
    list->InsertColumn(1, _("Price"), wxLIST_FORMAT_LEFT, 150);
    list->InsertColumn(2, _("Barcode"), wxLIST_FORMAT_LEFT, 150);
    list->InsertColumn(3, _("Quantity sold"), wxLIST_FORMAT_LEFT, 150);


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
    prevButton->Bind(wxEVT_BUTTON, &InfoProductsFrame::OnLoadPrev, this);
    nextButton->Bind(wxEVT_BUTTON, &InfoProductsFrame::OnLoadNext, this);

    bottomSizer->AddStretchSpacer(1);
    bottomSizer->Add(prevButton, 0, wxALL, 5);
    bottomSizer->Add(pageLabel, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    bottomSizer->Add(nextButton, 0, wxALL, 5);
    bottomSizer->AddStretchSpacer(1);
    bottomPanel->SetSizer(bottomSizer);

    // ---- Agregar todo al sizer principal ----
    mainSizer->Add(filtersPanel, 0, wxEXPAND | wxALL, 5);
    mainSizer->Add(list, 1, wxEXPAND | wxALL, 5);
    mainSizer->Add(bottomPanel, 0, wxALIGN_CENTER | wxBOTTOM, 5);

    mainPanel->SetSizer(mainSizer);
    mainSizer->SetSizeHints(this);
}


void InfoProductsFrame::AjustarColumnasListCtrl() {
    if (list) {
        wxSize clientSize = list->GetClientSize();
        int totalWidth = clientSize.GetWidth();

        if (totalWidth > 0) {
            int productNameWidth = 0;
            int priceWidth = 0;
            int barcodeWidth = 0;
            int quantitySoldWidth = 0;

            // Restar ancho del scrollbar si existe
            wxSize virtualSize = list->GetVirtualSize();
            if (virtualSize.GetHeight() > clientSize.GetHeight())
                totalWidth -= wxSystemSettings::GetMetric(wxSYS_VSCROLL_X);

            if (!IsMaximized()) {
                // DistribuciÃÂ³n en modo normal
                productNameWidth = static_cast<int>(totalWidth * 0.25);
                priceWidth = static_cast<int>(totalWidth * 0.25);
                barcodeWidth = static_cast<int>(totalWidth * 0.25);
                quantitySoldWidth = totalWidth - productNameWidth - priceWidth - barcodeWidth;
            }
            else {
                // DistribuciÃÂ³n en modo maximizado
                productNameWidth = static_cast<int>(totalWidth * 0.25);
                priceWidth = static_cast<int>(totalWidth * 0.25);
                barcodeWidth = static_cast<int>(totalWidth * 0.25);
                quantitySoldWidth = totalWidth - productNameWidth - priceWidth - barcodeWidth;
            }

            // Anchos mÃÂ­nimos
            productNameWidth = wxMax(200, productNameWidth);
            priceWidth = wxMax(100, priceWidth);
            barcodeWidth = wxMax(150, barcodeWidth);
            quantitySoldWidth = wxMax(150, quantitySoldWidth);

            // Asignar a columnas (ajusta el ÃÂ­ndice si el orden es diferente)
            list->SetColumnWidth(0, productNameWidth);
            list->SetColumnWidth(1, priceWidth);
            list->SetColumnWidth(2, barcodeWidth);
            list->SetColumnWidth(3, quantitySoldWidth);
        }
    }
}
