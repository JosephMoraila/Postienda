#include "gui/PreviousPurchasesFrame/SpecificPurchaseInfoMenu/SpecificPurchaseInfoMenu.hpp"
#include "utils/window/WindowUtils.h"
#include "utils/MathUtils.hpp"
#include <wx/listctrl.h>
#include <wx/sizer.h>
#include "utils/DateTimeUtils.hpp"

void SpecificPurchaseInfoMenu::Widgets()
{
    // --- Panel principal dentro de la ventana ---
    mainPanel = new wxPanel(this, wxID_ANY);

    // --- Panel inferior (para botones) ---
    buttonPanel = new wxPanel(mainPanel);
    wxBoxSizer* buttomSizer = new wxBoxSizer(wxHORIZONTAL);

    wxButton* printButton = new wxButton(buttonPanel, wxID_ANY, _("Print ticket"),wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	printButton->Bind(wxEVT_BUTTON, &SpecificPurchaseInfoMenu::OnPrintTicketButtonClicked, this);
	printButton->Bind(wxEVT_ENTER_WINDOW, [this](wxMouseEvent& event) {OnWidgetEnter(event, temaOscuro); });
	printButton->Bind(wxEVT_LEAVE_WINDOW, [this](wxMouseEvent& event) {OnWidgetLeave(event, temaOscuro); });
    buttomSizer->Add(printButton, 0, wxALL, 5);

	auto [worker, total, dateTime] = GetWorkerTotalDateTimePurchaseID(); // Llamada para inicializar totalUI
    labelTotal = new wxStaticText(buttonPanel, wxID_ANY, wxString::Format("Total: %s", FormatFloatWithCommas(total)), wxDefaultPosition, wxDefaultSize);
	buttomSizer->Add(labelTotal, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	auto [date, time] = DateTimeUtils::FormatDateTimeLocalized(dateTime);
    wxStaticText* dateLabel = new wxStaticText(buttonPanel, wxID_ANY, date, wxDefaultPosition, wxDefaultSize);
	buttomSizer->Add(dateLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	wxStaticText* timeLabel = new wxStaticText(buttonPanel, wxID_ANY, time, wxDefaultPosition, wxDefaultSize);
	buttomSizer->Add(timeLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	wxStaticText* workerLabel = new wxStaticText(buttonPanel, wxID_ANY, worker, wxDefaultPosition, wxDefaultSize);
	buttomSizer->Add(workerLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

    buttonPanel->SetSizer(buttomSizer);

    // --- Crear lista ---
    list = new wxListCtrl(mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT);

    // --- Agregar columnas ---
    list->InsertColumn(0, _("Name"), wxLIST_FORMAT_LEFT, 200);
    list->InsertColumn(1, _("Quantity"), wxLIST_FORMAT_LEFT, 150);
    list->InsertColumn(2, _("Price"), wxLIST_FORMAT_LEFT, 400);

    // --- Sizer principal del mainPanel ---
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(buttonPanel, 0, wxALIGN_LEFT | wxALL, 5); // panel de botones arriba
    mainSizer->Add(list, 1, wxEXPAND | wxALL, 10);        // lista abajo
    mainPanel->SetSizer(mainSizer);

    // --- Sizer principal de la ventana ---
    wxBoxSizer* windowSizer = new wxBoxSizer(wxVERTICAL);
    windowSizer->Add(mainPanel, 1, wxEXPAND | wxALL, 0);
    SetSizerAndFit(windowSizer);
}


void SpecificPurchaseInfoMenu::AjustarColumnasListCtrl() {
    if (list) {
        wxSize clientSize = list->GetClientSize();
        int totalWidth = clientSize.GetWidth();

        if (totalWidth > 0) {
            int nameWidth = 0;
            int quantityWidth = 0;
            int priceWidth = 0;

            // Restar ancho del scrollbar si existe
            wxSize virtualSize = list->GetVirtualSize();
            if (virtualSize.GetHeight() > clientSize.GetHeight())
                totalWidth -= wxSystemSettings::GetMetric(wxSYS_VSCROLL_X);

            if (!IsMaximized()) {
                // DistribuciÃÂÃÂ³n en modo normal
                nameWidth = static_cast<int>(totalWidth * 0.60);
                quantityWidth = static_cast<int>(totalWidth * 0.18);
                priceWidth = totalWidth - nameWidth - quantityWidth;
            }
            else {
                // DistribuciÃÂÃÂ³n en modo maximizado
                nameWidth = static_cast<int>(totalWidth * 0.60);
                quantityWidth = static_cast<int>(totalWidth * 0.18);
                priceWidth = totalWidth - nameWidth - quantityWidth;
            }

            // Anchos mÃÂÃÂ­nimos
            nameWidth = wxMax(200, nameWidth);
            quantityWidth = wxMax(150, quantityWidth);
            priceWidth = wxMax(220, priceWidth);

            // Asignar a columnas (ajusta el ÃÂÃÂ­ndice si el orden es diferente)
            list->SetColumnWidth(0, nameWidth);
            list->SetColumnWidth(1, quantityWidth);
            list->SetColumnWidth(2, priceWidth);
        }
    }
}
