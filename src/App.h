#pragma once
#include <wx/snglinst.h>
#include "gui/MainFrame/MainFrame.h"
#include <wx/wx.h>

/**
 * @class App
 * @brief Clase principal de la aplicaciÃÂÃÂ³n.
 *
 * Hereda de wxApp y se encarga de inicializar la aplicaciÃÂÃÂ³n
 * y crear la ventana principal.
 */
class App : public wxApp
{
    /**
     * @brief Inicializa la aplicaciÃÂÃÂ³n.
     *
     * Este mÃÂÃÂ©todo se ejecuta al inicio del programa y crea la ventana
     * principal (`MainFrame`).
     * Es obligatorio que exista con este nombre ya que wxWidgets lo
     * utiliza internamente para arrancar la aplicaciÃÂÃÂ³n.
     *
     * @return true si la inicializaciÃÂÃÂ³n fue exitosa.
     */
    bool OnInit();
    wxLocale m_locale; ///Init language
    wxSingleInstanceChecker* m_checker = nullptr; ///Only one instance checker
    MainFrame* m_mainFrame = nullptr;
     /**
         @brief Sets langToTry to the saved language or by default
         @param lang - String to compare with to select that language
         @param langToTry - Output: Set to that language
     **/
	void InitLanguage(wxString& lang, wxLanguage& langToTry);
public:
    /**
        @brief Focus existing instance when another instance wants to be opened
    **/
    void FocusMainWindow();
};
