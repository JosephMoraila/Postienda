#include "gui/ActionsFrame/ActionsFrame.hpp"
#include <wx/display.h>
#include "utils/window/WindowUtils.h"
#include "utils/ValidateStringInput.h"

ActionsFrame::ActionsFrame(wxWindow* parent) : wxFrame(parent, wxID_ANY, _("Actions"), wxDefaultPosition, wxSize(800, 600)) {
	AplicarIconoPrincipal(this);
	Widgets();

}

void ActionsFrame::AplicarTema(bool oscuro) {
    temaOscuro = oscuro;
    ApplyTheme(mainPanel, temaOscuro);
    ApplyTheme(buttonPanel, temaOscuro);
    ApplyTheme(filtersPanel, temaOscuro);
    Refresh();
    Update();
}


wxString ActionsFrame::GetStartDate() {
    // Obtener la fecha del DatePicker
    wxDateTime selectedDate = startDatePicker->GetValue();
    // Formatear solo la fecha en formato SQLite
    return selectedDate.FormatISODate(); // YYYY-MM-DD
}

wxString ActionsFrame::GetTimeStart() {
    wxDateTime selectedTime = startTimePicker->GetValue();
    return selectedTime.Format("%H:%M:%S"); // HH:MM:SS
}

wxString ActionsFrame::GetEndDate() {
    // Obtener la fecha del DatePicker
    wxDateTime selectedDate = endDatePicker->GetValue();
    // Formatear solo la fecha en formato SQLite
    return selectedDate.FormatISODate(); // YYYY-MM-DD
}

wxString ActionsFrame::GetTimeEnd() {
    wxDateTime selectedTime = endTimePicker->GetValue();
    return selectedTime.Format("%H:%M:%S"); // HH:MM:SS
}
