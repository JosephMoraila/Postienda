/**
 * @file FuenteUtils.h
 * @brief Utility functions for dynamic font adjustment in wxWidgets windows.
 * @details Provides helper functions to automatically resize fonts of widgets
 * based on the height of the parent window. Useful for responsive UIs where
 * font size should adapt to window resizing.
 */



#pragma once
#include <wx/wx.h>
#include <wx/grid.h>


 /**
  * @brief Adjusts fonts dynamically according to the height of the parent window.
  * @details
  * This function calculates a proportional font size based on the client height
  * of the given window. It then applies the new font to:
  * - A specific button (if provided),
  * - A grid, updating both labels and cells (if provided),
  * - All child widgets of a panel (if provided).
  *
  * Finally, the parent window is refreshed and updated to reflect the changes.
  *
  * @param ventana Pointer to the main wxWindow (mandatory). Used to calculate the font size.
  * @param boton Optional pointer to a wxButton. If not null, the new font is applied to the button.
  * @param grid Optional pointer to a wxGrid. If not null, the new font is applied to grid labels and cells.
  * @param panel Optional pointer to a wxPanel. If not null, the new font is applied to all child widgets of the panel.
  */
void AjustarFuenteSegunAlto(wxWindow* ventana,
    wxButton* boton = nullptr,
    wxGrid* grid = nullptr,
    wxPanel* panel = nullptr);
