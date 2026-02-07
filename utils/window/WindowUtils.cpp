#include "utils/window/WindowUtils.h"
#include "constants/RES/ICON/ICON_PATH.hpp"
#include "gui/TicketFrame/Canvas/Canvas.hpp"
#include "gui/TicketFrame/TicketFrame.hpp"
#include <wx/grid.h>
#include <wx/listctrl.h>
#include <wx/spinbutt.h>
#include <wx/treectrl.h>
#include <wx/datectrl.h>
#include <wx/timectrl.h>

void ApplyTheme(wxWindow* ventana, bool oscuro) {
    /// @brief Define los colores del tema de la aplicaciÃÂ³n.
    /// @details
    /// - colorFondo: color de fondo de la ventana principal.
    /// - colorTexto: color de texto para labels, botones y controles.
    /// - colorGrid: color de fondo de tablas o grids (si aplica).
    /// - colorBoton: color de fondo de botones.
    /// @note Valores mÃÂ¡s altos indican colores mÃÂ¡s claros; valores mÃÂ¡s bajos, mÃÂ¡s oscuros.
    wxColour colorFondo = oscuro ? wxColour(32, 32, 32) : wxColour(255, 255, 255);
    wxColour colorTexto = oscuro ? wxColour(255, 255, 255) : wxColour(0, 0, 0);
    wxColour colorGrid = oscuro ? wxColour(45, 45, 45) : wxColour(240, 240, 240);
    wxColour colorBoton = oscuro ? wxColour(64, 64, 64) : wxColour(240, 240, 240);
	wxColour colorGrisClaro = wxColour(220, 220, 220);

    // Verificar si esta ventana es un panel hijo de TicketFrame
    bool esPanelDeTicketFrame = false;
    if (auto* panel = dynamic_cast<wxPanel*>(ventana)) {
        if (dynamic_cast<TicketFrame*>(panel->GetParent())) esPanelDeTicketFrame = true;    
    }

    // Aplicar color de fondo segÃÂºn el tipo de ventana
    if (esPanelDeTicketFrame && !oscuro) ventana->SetBackgroundColour(colorGrisClaro);
    else ventana->SetBackgroundColour(colorFondo);
    

    ventana->SetForegroundColour(colorTexto);

    for (wxWindow* child : ventana->GetChildren()) {
        if (dynamic_cast<Canvas*>(child)) continue; // No aplicar tema al canvas
        
        if (auto* btn = dynamic_cast<wxButton*>(child)) {
            btn->SetOwnBackgroundColour(colorBoton);
            btn->SetOwnForegroundColour(colorTexto);
        }
        else if (auto* textCtrl = dynamic_cast<wxTextCtrl*>(child)) {
            textCtrl->SetBackgroundColour(colorFondo);
            textCtrl->SetForegroundColour(colorTexto);
        }
        else if (auto* panel = dynamic_cast<wxPanel*>(child)) {
            panel->SetBackgroundColour(colorFondo);
            panel->SetForegroundColour(colorTexto);
        }
        else if (auto* grid = dynamic_cast<wxGrid*>(child)) {
            grid->SetBackgroundColour(colorGrid);
            grid->SetForegroundColour(colorTexto);
            grid->SetLabelBackgroundColour(colorBoton);
            grid->SetLabelTextColour(colorTexto);
            grid->SetDefaultCellBackgroundColour(colorGrid);
            grid->SetDefaultCellTextColour(colorTexto);
            grid->ForceRefresh();
        }
        else if (auto* tree = dynamic_cast<wxTreeCtrl*>(child)) {
            tree->SetBackgroundColour(colorFondo);
            tree->SetForegroundColour(colorTexto);
        }
        else if (auto* checkbox = dynamic_cast<wxCheckBox*>(child)) {
            checkbox->SetBackgroundColour(colorFondo);
            checkbox->SetForegroundColour(colorTexto);
		}
        else if (auto* lista = dynamic_cast<wxTextCtrl*>(child)) {
            lista->SetBackgroundColour(colorFondo);
            lista->SetForegroundColour(colorTexto);
        }
        else if (auto* choice = dynamic_cast<wxChoice*>(child)) {
            choice->SetBackgroundColour(colorFondo);
            choice->SetForegroundColour(colorTexto);
        }
        else if (auto* richInput = dynamic_cast<wxRichTextCtrl*>(child)) {
            richInput->Freeze();
            // Cambiar fondo del control
            richInput->SetBackgroundColour(colorFondo);
            // Cambiar color de todo el texto existente
            wxRichTextRange range(0, richInput->GetLastPosition());
            richInput->SetStyle(range, wxTextAttr(colorTexto, colorFondo));
            // Para el texto nuevo - usar BeginTextColour
            richInput->BeginTextColour(colorTexto);
            // O alternativamente, configurar el estilo bÃÂ¡sico
            wxRichTextAttr basicStyle;
            basicStyle.SetTextColour(colorTexto);
            basicStyle.SetBackgroundColour(colorFondo);
            richInput->SetBasicStyle(basicStyle);
            richInput->SetDefaultStyle(wxTextAttr(colorTexto, colorFondo));
            richInput->Thaw();
            richInput->Refresh();
        }
        else if (auto* spin = dynamic_cast<wxSpinButton*>(child)) {
            spin->SetBackgroundColour(colorFondo);
            spin->SetForegroundColour(colorTexto);
		}
        else if (auto* datePicker = dynamic_cast<wxDatePickerCtrl*>(child)) {
            datePicker->SetOwnBackgroundColour(colorBoton);
            datePicker->SetBackgroundColour(colorFondo);
            datePicker->SetForegroundColour(colorTexto);
        }
        else {
            child->SetBackgroundColour(colorFondo);
            child->SetForegroundColour(colorTexto);
        }

    }

    ventana->Refresh();
	ventana->Update();
}

void OnButtonEnter(wxMouseEvent& event, bool temaOscuro) {
    wxButton* btn = dynamic_cast<wxButton*>(event.GetEventObject());
    SetButtonHover(btn, true, temaOscuro);
    event.Skip();
}

void OnButtonLeave(wxMouseEvent& event, bool temaOscuro) {
    wxButton* btn = dynamic_cast<wxButton*>(event.GetEventObject());
    btn->SetCursor(wxCursor(wxCURSOR_ARROW));
    SetButtonHover(btn, false, temaOscuro);
    event.Skip();
}

void SetButtonHover(wxButton* btn, bool enter, bool temaOscuro) {
    if (!btn) return;

    if (enter) {
        btn->SetCursor(wxCursor(wxCURSOR_HAND));
        wxColour colorHover = temaOscuro ? wxColour(80, 80, 80) : wxColour(220, 220, 220);
        btn->SetOwnBackgroundColour(colorHover);
        btn->SetOwnForegroundColour(temaOscuro ? wxColour(255, 255, 255) : wxColour(0, 0, 0));
    }
    else {
        btn->SetCursor(wxCursor(wxCURSOR_ARROW));
        wxColour colorNormal = temaOscuro ? wxColour(64, 64, 64) : wxColour(240, 240, 240);
        btn->SetOwnBackgroundColour(colorNormal);
        btn->SetOwnForegroundColour(temaOscuro ? wxColour(255, 255, 255) : wxColour(0, 0, 0));
    }
    btn->Refresh();
}

void OnWidgetEnter(wxMouseEvent& event, bool temaOscuro) {
    wxWindow* wnd = dynamic_cast<wxWindow*>(event.GetEventObject());
    SetWidgetHover(wnd, true, temaOscuro);
    event.Skip();
}

void OnWidgetLeave(wxMouseEvent& event, bool temaOscuro) {
    wxWindow* wnd = dynamic_cast<wxWindow*>(event.GetEventObject());
    SetWidgetHover(wnd, false, temaOscuro);
    event.Skip();
}

void SetWidgetHover(wxWindow* wnd, bool enter, bool temaOscuro) {
    if (!wnd) return;

    if (enter) {
        wnd->SetCursor(wxCursor(wxCURSOR_HAND));
        wxColour colorHover = temaOscuro ? wxColour(80, 80, 80) : wxColour(220, 220, 220);
        wnd->SetBackgroundColour(colorHover);
        wnd->SetForegroundColour(temaOscuro ? wxColour(255, 255, 255) : wxColour(0, 0, 0));
    }
    else {
        wnd->SetCursor(wxCursor(wxCURSOR_ARROW));
        wxColour colorNormal = temaOscuro ? wxColour(64, 64, 64) : wxColour(240, 240, 240);
        wnd->SetBackgroundColour(colorNormal);
        wnd->SetForegroundColour(temaOscuro ? wxColour(255, 255, 255) : wxColour(0, 0, 0));
    }
    wnd->Refresh();
}


void AplicarIconoPrincipal(wxTopLevelWindow* window) {
    if (!window) return;

#ifdef __WXMSW__
    // En Windows usa el recurso del .rc (MAINICON es el ID que definimos)
    window->SetIcon(wxICON(MAINICON));
#else
    // En Linux/macOS carga el PNG
    if (wxFileExists(ICON_PATH())) {
        window->SetIcon(wxIcon(ICON_PATH(), wxBITMAP_TYPE_PNG));
    }
#endif
}