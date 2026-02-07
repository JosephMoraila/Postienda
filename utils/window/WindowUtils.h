/**
* @file WindowUtils.h
* @brief DeclaraciÃÂÃÂ³n de funciones utilitarias para la gestiÃÂÃÂ³n de ventanas en wxWidgets.
*/
#pragma once
#include <wx/wx.h>

/**
 * @brief Aplica un tema (oscuro o claro) a una ventana y todos sus controles hijos.
 * Esta funciÃÂÃÂ³n evita que la implementaciÃÂÃÂ³n del tema en cada ventana (clase) sea larga en codigo.
 * 
 * @param ventana Puntero a la ventana raÃÂÃÂ­z donde se aplicarÃÂÃÂ¡ el tema.
 * @param oscuro true para aplicar el tema oscuro, false para el tema claro.
 */
void ApplyTheme(wxWindow* ventana, bool oscuro);

/**
* @brief Cambia el estilo visual de un botÃÂÃÂ³n y el cursor al pasar el mouse sobre ÃÂÃÂ©l (hover) y al salir.
 * 
 * @param btn Puntero al botÃÂÃÂ³n que se va a modificar.
 * @param enter true si el mouse entra en el ÃÂÃÂ¡rea del botÃÂÃÂ³n, false si sale.
 * @param temaOscuro true si el tema actual es oscuro, false si es claro.
 */
void SetButtonHover(wxButton* btn, bool enter, bool temaOscuro);

/**
 * @brief Evento que se dispara al entrar el mouse en el ÃÂÃÂ¡rea de un botÃÂÃÂ³n.
 * Cambia el estilo visual del botÃÂÃÂ³n para indicar que estÃÂÃÂ¡ activo cambiando su color de fondo y el cursor.
 * 
 * @param event Evento del mouse.
 * @param temaOscuro true si el tema actual es oscuro, false si es claro.
 */
void OnButtonEnter(wxMouseEvent& event, bool temaOscuro);

/**
 * @brief Evento que se dispara al salir el mouse del ÃÂÃÂ¡rea de un botÃÂÃÂ³n.
 * Restaura el estilo visual original del botÃÂÃÂ³n y el cursor.
 * 
 * @param event Evento del mouse.
 * @param temaOscuro true si el tema actual es oscuro, false si es claro.
 */
void OnButtonLeave(wxMouseEvent& event, bool temaOscuro);

/**
 * @brief Evento que se dispara al entrar el mouse en el ÃÂÃÂ¡rea de un widget cualquiera (no botÃÂÃÂ³n).
 * Cambia el estilo visual del widget para indicar que estÃÂÃÂ¡ activo cambiando su color de fondo y el cursor.
 * 
 * @param event Evento del mouse.
 * @param temaOscuro true si el tema actual es oscuro, false si es claro.
 */
void OnWidgetEnter(wxMouseEvent& event, bool temaOscuro);

/**
 * @brief Evento que se dispara al salir el mouse del ÃÂÃÂ¡rea de un widget cualquiera (no botÃÂÃÂ³n).
 * Restaura el estilo visual original del widget y el cursor.
 * 
 * @param event Evento del mouse.
 * @param temaOscuro true si el tema actual es oscuro, false si es claro.
 */
void OnWidgetLeave(wxMouseEvent& event, bool temaOscuro);

/**
 * @brief Cambia el estilo visual de un widget cualquiera (no botÃÂÃÂ³n) al pasar el mouse sobre ÃÂÃÂ©l (hover) y al salir.
 * 
 * @param wnd Puntero al widget que se va a modificar.
 * @param enter true si el mouse entra en el ÃÂÃÂ¡rea del widget, false si sale.
 * @param temaOscuro true si el tema actual es oscuro, false si es claro.
 */
void SetWidgetHover(wxWindow* wnd, bool enter, bool temaOscuro);

void AplicarIconoPrincipal(wxTopLevelWindow* window);
