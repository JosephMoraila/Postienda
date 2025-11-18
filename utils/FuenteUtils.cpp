#include "utils/FuenteUtils.h"

void AjustarFuenteSegunAlto(wxWindow* ventana, wxButton* boton, wxGrid* grid, wxPanel* panel) {
    if (!ventana) return;

    // Calcula el alto de cliente de la ventana
    int h = ventana->GetClientSize().GetHeight();

    // Calcula el tamaÃÂ±o de fuente con un mÃÂ­nimo razonable
    int fontSize = std::max(13, h / 60);

    // Crea una fuente
    wxFont font(fontSize, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

    // Aplica al botÃÂ³n si se pasa
    if (boton) {
        boton->SetFont(font);
    }

    // Aplica a la tabla si se pasa
    if (grid) {
        grid->SetLabelFont(font);
        grid->SetDefaultCellFont(font);
    }

    // Aplica a los hijos del panel si se pasa
    if (panel) {
        wxWindowList& children = panel->GetChildren();
        for (wxWindow* child : children) {
            child->SetFont(font);
        }
    }

    // Refresca la ventana
    ventana->Refresh();
    ventana->Update();
}
