#include "gui/ReturnsFrame/ReturnsFrame.hpp"
#include "gui/MainFrame/MainFrame.h"
#include <wx/display.h>
#include "utils/window/WindowUtils.h"
#include "utils/ValidateStringInput.h"

ReturnsFrame::ReturnsFrame(wxWindow* parent, MainFrame* mainFrame) : wxFrame(parent, wxID_ANY, _("Returns"), wxDefaultPosition, wxDefaultSize), mainFrame(mainFrame) {
	AplicarIconoPrincipal(this);
    CreateTableReturns();
	Widgets();
    Bind(wxEVT_SIZE, &ReturnsFrame::OnSize, this);
    Bind(wxEVT_CLOSE_WINDOW, &ReturnsFrame::OnClose, this);
}

unsigned long long ReturnsFrame::GetPurchaseID() {
    wxString value = purchaseIDInput->GetValue();
    wxString cleanedValue = LimpiarYValidarNombreWx(value);

    // Eliminar todo lo que no sea un dÃ?Â­gito
    wxString numericOnly;
    for (wxChar ch : cleanedValue) if (wxIsdigit(ch)) numericOnly += ch;

    // Si quedÃ?Â³ vacÃ?Â­o o sin nÃ?Âºmeros, regresa 0
    if (numericOnly.IsEmpty()) return 0;

    unsigned long long idValue = 0;
    if (!numericOnly.ToULongLong(&idValue)) return 0; // conversiÃ?Â³n fallida

    return idValue;
}
