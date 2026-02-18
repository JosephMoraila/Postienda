#include "gui/InfoReturnsFrame/InfoReturnsFrame.hpp"
#include <wx/display.h>
#include "utils/window/WindowUtils.h"
#include "utils/ValidateStringInput.h"


InfoReturnsFrame::InfoReturnsFrame(wxWindow* parent) : wxFrame(parent, wxID_ANY, _("Info Returns"), wxDefaultPosition, wxSize(800, 600)) {
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

    Bind(wxEVT_SIZE, &InfoReturnsFrame::OnSize, this);
}

unsigned long long InfoReturnsFrame::GetID() {
    wxString value = IdInput->GetValue();
    wxString cleanedValue = LimpiarYValidarNombreWx(value);

    // Eliminar todo lo que no sea un dÃÂ­gito
    wxString numericOnly;
    for (wxChar ch : cleanedValue) if (wxIsdigit(ch)) numericOnly += ch;

    // Si quedÃÂ³ vacÃÂ­o o sin nÃÂºmeros, regresa 0
    if (numericOnly.IsEmpty()) return 0;

    unsigned long long idValue = 0;
    if (!numericOnly.ToULongLong(&idValue)) return 0; // conversiÃÂ³n fallida

    return idValue;
}

wxString InfoReturnsFrame::GetStartDate() {
    // Obtener la fecha del DatePicker
    wxDateTime selectedDate = startDatePicker->GetValue();
    // Formatear solo la fecha en formato SQLite
    return selectedDate.FormatISODate(); // YYYY-MM-DD
}

wxString InfoReturnsFrame::GetTimeStart() {
    wxDateTime selectedTime = startTimePicker->GetValue();
    return selectedTime.Format("%H:%M:%S"); // HH:MM:SS
}

wxString InfoReturnsFrame::GetEndDate() {
    // Obtener la fecha del DatePicker
    wxDateTime selectedDate = endDatePicker->GetValue();
    // Formatear solo la fecha en formato SQLite
    return selectedDate.FormatISODate(); // YYYY-MM-DD
}

wxString InfoReturnsFrame::GetTimeEnd() {
    wxDateTime selectedTime = endTimePicker->GetValue();
    return selectedTime.Format("%H:%M:%S"); // HH:MM:SS
}

double InfoReturnsFrame::GetMinAmount() {
    wxString raw = minAmountInput->GetValue();
    raw.Replace(",", ""); // quitar comas
    double precio = 0.0;
    if (!raw.ToDouble(&precio)) return -1.0;
    return precio;
}


double InfoReturnsFrame::GetMaxAmount() {
    wxString raw = maxAmountInput->GetValue();
    raw.Replace(",", ""); // quitar comas
    double precio = 0.0;
    if (!raw.ToDouble(&precio)) return -1.0;
    return precio;
}

wxString InfoReturnsFrame::GetWorker() {
    wxString worker = workerInput->GetValue();
    wxString workerCleaned = LimpiarYValidarNombreWx(worker);
    return workerCleaned;
}


wxString InfoReturnsFrame::GetProductName() {
    wxString productname = productNameInput->GetValue();
    wxString productnameCleaned = LimpiarYValidarNombreWx(productname);
    return productnameCleaned;
}


double InfoReturnsFrame::GetMinQuantity() {
    wxString raw = minQuantityInput->GetValue();
    raw.Replace(",", ""); // quitar comas
    double quantity = 0.0;
    if (!raw.ToDouble(&quantity)) return -1.0;
    return quantity;
}

double InfoReturnsFrame::GetMaxQuantity() {
    wxString raw = maxQuantityInput->GetValue();
    raw.Replace(",", ""); // quitar comas
    double quantity = 0.0;
    if (!raw.ToDouble(&quantity)) return -1.0;
    return quantity;
}

unsigned long long InfoReturnsFrame::GetPurchaseID() {
    wxString value = purchaseIdInput->GetValue();
    wxString cleanedValue = LimpiarYValidarNombreWx(value);

    // Eliminar todo lo que no sea un dÃÂ­gito
    wxString numericOnly;
    for (wxChar ch : cleanedValue) if (wxIsdigit(ch)) numericOnly += ch;

    // Si quedÃÂ³ vacÃÂ­o o sin nÃÂºmeros, regresa 0
    if (numericOnly.IsEmpty()) return 0;

    unsigned long long idValue = 0;
    if (!numericOnly.ToULongLong(&idValue)) return 0; // conversiÃÂ³n fallida

    return idValue;
}

void InfoReturnsFrame::AplicarTema(bool oscuro) {
    temaOscuro = oscuro;
    ApplyTheme(mainPanel, temaOscuro);
    ApplyTheme(scrollWidgets, temaOscuro);
    ApplyTheme(bottomPanel, temaOscuro);
    Refresh();
    Update();
}