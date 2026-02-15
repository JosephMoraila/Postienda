#include "gui/InfoDrawerFrame/InfoDrawerFrame.hpp"
#include <wx/display.h>
#include "utils/window/WindowUtils.h"
#include "utils/ValidateStringInput.h"

InfoDrawerFrame::InfoDrawerFrame(wxWindow* parent) : wxFrame(parent, wxID_ANY, _("Info products"), wxDefaultPosition, wxDefaultSize) {
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

    Bind(wxEVT_SIZE, &InfoDrawerFrame::OnSize, this);
}


void InfoDrawerFrame::AplicarTema(bool oscuro) {
    temaOscuro = oscuro;
    ApplyTheme(mainPanel, temaOscuro);
    ApplyTheme(scrollWidgets, temaOscuro);
    ApplyTheme(bottomPanel, temaOscuro);
    Refresh();
    Update();
}

unsigned long long InfoDrawerFrame::GetID() {
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

wxString InfoDrawerFrame::GetStartDate() {
    // Obtener la fecha del DatePicker
    wxDateTime selectedDate = startDatePicker->GetValue();
    // Formatear solo la fecha en formato SQLite
    return selectedDate.FormatISODate(); // YYYY-MM-DD
}

wxString InfoDrawerFrame::GetTimeStart() {
    wxDateTime selectedTime = startTimePicker->GetValue();
    return selectedTime.Format("%H:%M:%S"); // HH:MM:SS
}

wxString InfoDrawerFrame::GetEndDate() {
    // Obtener la fecha del DatePicker
    wxDateTime selectedDate = endDatePicker->GetValue();
    // Formatear solo la fecha en formato SQLite
    return selectedDate.FormatISODate(); // YYYY-MM-DD
}

wxString InfoDrawerFrame::GetTimeEnd() {
    wxDateTime selectedTime = endTimePicker->GetValue();
    return selectedTime.Format("%H:%M:%S"); // HH:MM:SS
}

double InfoDrawerFrame::GetMinAmount() {
    wxString raw = minAmountInput->GetValue();
    raw.Replace(",", ""); // quitar comas
    double precio = 0.0;
    if (!raw.ToDouble(&precio)) return -1.0;
    return precio;
}


double InfoDrawerFrame::GetMaxAmount() {
    wxString raw = maxAmountInput->GetValue();
    raw.Replace(",", ""); // quitar comas
    double precio = 0.0;
    if (!raw.ToDouble(&precio)) return -1.0;
    return precio;
}

wxString InfoDrawerFrame::GetWorker() {
    wxString worker = workerInput->GetValue();
    wxString workerCleaned = LimpiarYValidarNombreWx(worker);
    return workerCleaned;
}

wxString InfoDrawerFrame::GetReason() {
    wxString reason = reasonInput->GetValue();
    wxString reasonCleaned = LimpiarYValidarNombreWx(reason);
    return reasonCleaned;
}

unsigned long long InfoDrawerFrame::GetPurchaseID() {
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

double InfoDrawerFrame::GetDrawerAfterMinAmount() {
    wxString raw = drawerAfterMinAmountInput->GetValue();
    raw.Replace(",", ""); // quitar comas
    double precio = 0.0;
    if (!raw.ToDouble(&precio)) return -1.0;
    return precio;
}


double InfoDrawerFrame::GetDrawerAfterMaxAmount() {
    wxString raw = drawerAfterMaxAmountInput->GetValue();
    raw.Replace(",", ""); // quitar comas
    double precio = 0.0;
    if (!raw.ToDouble(&precio)) return -1.0;
    return precio;
}
