#include "gui/InfoProductsFrame/InfoProductsFrame.hpp"
#include <wx/display.h>
#include "utils/window/WindowUtils.h"
#include "utils/ValidateStringInput.h"

InfoProductsFrame::InfoProductsFrame(wxWindow* parent) : wxFrame(parent, wxID_ANY, _("Info products"), wxDefaultPosition, wxDefaultSize) {
    AplicarIconoPrincipal(this);
    Widgets();
    wxRect screenRect = wxDisplay(wxDisplay::GetFromWindow(this)).GetClientArea();
    int screenWidth = screenRect.GetWidth();
    int screenHeight = screenRect.GetHeight();

    // Definir el ancho mÃ?Â­nimo absoluto requerido para que todos los filtros se vean.
    const int MIN_FILTERS_WIDTH = 1100;
    const int MIN_HEIGHT = 500;

    // Calcular el ancho inicial (80% de la pantalla, pero no menos que el mÃ?Â­nimo)
    int initialWidth = wxMax(MIN_FILTERS_WIDTH, (int)(screenWidth * 0.80));
    int initialHeight = wxMax(MIN_HEIGHT, (int)(screenHeight * 0.70));

    // Aplicar tamaÃ?Â±os
    SetSize(initialWidth, initialHeight);

    // **CRUCIAL**: Establece el mÃ?Â­nimo ancho de la ventana al mÃ?Â­nimo de los filtros.
    // Esto evita que la ventana se achique demasiado.
    SetMinClientSize(wxSize(MIN_FILTERS_WIDTH, MIN_HEIGHT));

    Bind(wxEVT_SIZE, &InfoProductsFrame::OnSize, this);
}

void InfoProductsFrame::AplicarTema(bool oscuro) {
    temaOscuro = oscuro;
    ApplyTheme(mainPanel, temaOscuro);
    ApplyTheme(scrollWidgets, temaOscuro);
    ApplyTheme(bottomPanel, temaOscuro);
    Refresh();
    Update();
}


wxString InfoProductsFrame::GetStartDate() {
    // Obtener la fecha del DatePicker
    wxDateTime selectedDate = startDatePicker->GetValue();
    // Formatear solo la fecha en formato SQLite
    return selectedDate.FormatISODate(); // YYYY-MM-DD
}

wxString InfoProductsFrame::GetTimeStart() {
    wxDateTime selectedTime = startTimePicker->GetValue();
    return selectedTime.Format("%H:%M:%S"); // HH:MM:SS
}

wxString InfoProductsFrame::GetEndDate() {
    // Obtener la fecha del DatePicker
    wxDateTime selectedDate = endDatePicker->GetValue();
    // Formatear solo la fecha en formato SQLite
    return selectedDate.FormatISODate(); // YYYY-MM-DD
}

wxString InfoProductsFrame::GetTimeEnd() {
    wxDateTime selectedTime = endTimePicker->GetValue();
    return selectedTime.Format("%H:%M:%S"); // HH:MM:SS
}

double InfoProductsFrame::GetMinPrice() {
    wxString raw = minAmountInput->GetValue();
    raw.Replace(",", ""); // quitar comas
    double precio = 0.0;
    if (!raw.ToDouble(&precio)) return -1.0;
    return precio;
}


double InfoProductsFrame::GetMaxPrice() {
    wxString raw = maxAmountInput->GetValue();
    raw.Replace(",", ""); // quitar comas
    double precio = 0.0;
    if (!raw.ToDouble(&precio)) return -1.0;
    return precio;
}

wxString InfoProductsFrame::GetNameBarcodeProduct() {
    wxString productNameBarcode = productNameBarcodeInput->GetValue();
    wxString productNameBarcodeCleaned = LimpiarYValidarNombreWx(productNameBarcode);
    return productNameBarcodeCleaned;
}