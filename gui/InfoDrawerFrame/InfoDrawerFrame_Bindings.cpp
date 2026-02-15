#include "gui/InfoDrawerFrame/InfoDrawerFrame.hpp"

void InfoDrawerFrame::OnSize(wxSizeEvent& event) {
    event.Skip(); // deja que wxWidgets ajuste la ventana
    CallAfter([this]() { // CallAfter para asegurar que la ventana ya se haya redimensionado completamente
        AjustarColumnasListCtrl();
        if (mainPanel) mainPanel->Layout();
        });
}
