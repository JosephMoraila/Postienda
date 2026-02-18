#include "gui/ReturnsFrame/ReturnsFrame.hpp"
#include <wx/display.h>
#include "utils/window/WindowUtils.h"
#include "utils/ValidateStringInput.h"

void ReturnsFrame::Widgets() {
    mainPanel = new wxPanel(this);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Crear panel para Purchase ID
    purchaseIdPanel = new wxPanel(mainPanel);
    wxBoxSizer* purchaseIdSizer = new wxBoxSizer(wxVERTICAL);

    // Agregar widgets al panel de Purchase ID
    wxStaticText* idLabel = new wxStaticText(purchaseIdPanel, wxID_ANY, _("Purchase ID:"));
    purchaseIdSizer->Add(idLabel, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);

    purchaseIDInput = new wxTextCtrl(purchaseIdPanel, wxID_ANY, "", wxDefaultPosition, wxSize(150, -1), wxTE_PROCESS_ENTER);
    purchaseIDInput->SetToolTip(_("Enter the Purchase ID to validate the return"));
    purchaseIDInput->Bind(wxEVT_TEXT_ENTER, &ReturnsFrame::OnEnterPurchaseIdInput, this);
    purchaseIdSizer->Add(purchaseIDInput, 0, wxALL, 5);

    wxButton* validateButton = new wxButton(purchaseIdPanel, wxID_ANY, _("Validate"), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    validateButton->Bind(wxEVT_ENTER_WINDOW, [this](wxMouseEvent& event) {OnWidgetEnter(event, temaOscuro); });
    validateButton->Bind(wxEVT_LEAVE_WINDOW, [this](wxMouseEvent& event) {OnWidgetLeave(event, temaOscuro); });
    validateButton->Bind(wxEVT_BUTTON, &ReturnsFrame::OnButtonValidateClick, this);
    purchaseIdSizer->Add(validateButton, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);

    purchaseIdPanel->SetSizer(purchaseIdSizer);

    // Usar wxScrolledWindow para el panel de productos con scroll vertical
    productsPanel = new wxPanel(mainPanel);  
    wxBoxSizer* productsSizer = new wxBoxSizer(wxVERTICAL);

    returnButton = new wxButton(productsPanel, wxID_ANY, _("Return"), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    returnButton->Bind(wxEVT_ENTER_WINDOW, [this](wxMouseEvent& event) {OnWidgetEnter(event, temaOscuro); });
    returnButton->Bind(wxEVT_LEAVE_WINDOW, [this](wxMouseEvent& event) {OnWidgetLeave(event, temaOscuro); });
    returnButton->Bind(wxEVT_BUTTON, &ReturnsFrame::OnReturnProduct, this);
    productsSizer->Add(returnButton, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);

    // Widgets dentro de productsPanel
    grid = new wxGrid(productsPanel, wxID_ANY);
    grid->CreateGrid(0, 6);  // 0 filas, 6 columnas
    grid->SetColLabelValue(0, _("Product ID"));
    grid->SetColLabelValue(1, _("Purchase Item ID"));
    grid->SetColLabelValue(2, _("Product name"));
    grid->SetColLabelValue(3, _("Quantity"));
    grid->SetColLabelValue(4, _("Price at purchase"));
    grid->SetColLabelValue(5, _("Barcode"));

    // Configurar columnas
    grid->SetColSize(0, 0);  // Ocultar Product ID
    grid->SetColSize(1, 0);  
    grid->SetColSize(2, 250); // Product name
    grid->SetColSize(3, 100); // Quantity
    grid->SetColSize(4, 120); // Price
	grid->SetColSize(5, 200); // Barcode

    // Establecer altura fija para el grid (activará scroll interno cuando haya muchas filas)
    grid->SetMinSize(wxSize(-1, 400));  // -1 = ancho flexible, 300px altura fija
    grid->SetMaxSize(wxSize(-1, 400));

    grid->EnableEditing(false);
    grid->SetSelectionMode(wxGrid::wxGridSelectRows);  // Seleccionar filas completa

    // Agregar grid con proporción 1 y wxEXPAND para ocupar todo el ancho disponible
    productsSizer->Add(grid, 1, wxALL | wxEXPAND, 10);
    productsPanel->SetSizer(productsSizer);

    // AGREGAR los paneles al sizer principal
    mainSizer->Add(purchaseIdPanel, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 10);
    mainSizer->Add(productsPanel, 1, wxALL | wxEXPAND, 10);  // wxEXPAND para ocupar todo el ancho

    // Establecer el sizer principal al panel principal
    mainPanel->SetSizer(mainSizer);

    // Establecer tamaño inicial de la ventana
    SetClientSize(800, 550);  // Ancho inicial y altura predefinida

    // Ocultar DESPUÉS de configurar todo
    productsPanel->Hide();
}

void ReturnsFrame::AplicarTema(bool oscuro) {
    temaOscuro = oscuro;
    ApplyTheme(mainPanel, temaOscuro);
    ApplyTheme(purchaseIdPanel, temaOscuro);
    ApplyTheme(productsPanel, temaOscuro);
}

