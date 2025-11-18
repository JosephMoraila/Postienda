#pragma once
#include <wx/wx.h>
#include <wx/spinbutt.h>
#include "utils/MathUtils.hpp"
#include <variant>

/**
 * @class StockDialog
 * @brief DiÃ¡logo personalizado para ajustar el stock de un producto.
 * @details Permite ingresar la cantidad a agregar o quitar del stock actual. Incluye validaciones para evitar cantidades negativas.
 */
class StockDialog : public wxDialog {
public:
	StockDialog(wxWindow* parent, std::variant<unsigned long long, double> availableStock); ///< Constructor que inicializa el diÃ¡logo con el stock disponible.
	std::variant<unsigned long long, double> GetStock() const; ///< Retorna la cantidad de stock ingresada.
	void AplicarTema(bool oscuro); ///< Aplica el tema claro u oscuro a la ventana y todos sus widgets hijos.

private:
	void Widgets(); ///< Crea y organiza los widgets en la ventana.
	bool temaOscuro = false; ///< Indica si el tema oscuro estÃ¡ activo.
	bool m_productByWeight; ///< Indica si el producto es por peso (double) o por unidad (unsigned long long).
	std::variant<unsigned long long, double> m_availableStock; ///< Stock disponible del producto.
	wxTextCtrl* txtStock; ///< Campo de texto para el nombre del producto.
	wxSpinButton* spin; ///< Botones de incremento/decremento para ajustar la cantidad.

	 /**
		 @brief Manejador del evento de incremento del wxSpinButton.
		 @param event - El evento de spin que contiene informaciÃ³n sobre el cambio.
	 **/
	void OnSpinUp(wxSpinEvent& event);
	 /**
		 @brief Manejador del evento de decremento del wxSpinButton.
		 @param event - El evento de spin que contiene informaciÃ³n sobre el cambio.
	 **/
	void OnSpinDown(wxSpinEvent& event);
	 /**
		 @brief Manejador del evento de rueda del ratÃ³n sobre el wxTextCtrl.
		 @param event - El evento de ratÃ³n que contiene informaciÃ³n sobre el movimiento de la rueda.
	 **/
	void OnMouseWheel(wxMouseEvent& event);
	 /**
		 @brief Manejador del evento de cambio de texto en el wxTextCtrl.
		 @param delta - El cambio en la cantidad de stock (positivo o negativo).
	 **/
	void AdjustStock(double delta);

    /**
     * @brief Formatea el contenido de un wxTextCtrl con comas mientras se escribe.
     * @param ctrl Puntero al wxTextCtrl a formatear.
     * @details Convierte el texto actual en nÃºmero, elimina comas existentes,
     * y vuelve a escribirlo con comas usando FormatWithCommas.
     * Mantiene la posiciÃ³n del cursor lo mÃ¡s coherente posible.
     */
	void FormatTextCtrlWithCommas(wxTextCtrl* ctrl);

 /**
	 @brief Formatea y establece el valor del stock disponible en el wxTextCtrl.
	 @param availableStock - El stock disponible, que puede ser un entero sin signo o un nÃºmero de punto flotante.
 **/
	void FormatavailableStockToValue(const std::variant<unsigned long long, double>& availableStock);
};