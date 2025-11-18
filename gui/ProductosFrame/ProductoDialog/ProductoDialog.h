// ProductoDialog.h
#pragma once
#include <wx/wx.h>
#include "utils/MathUtils.hpp"


/**
 * @class ProductoDialog
 * @brief DiÃÂÃÂ¡logo personalizado para agregar un nuevo producto.
 * @details Permite ingresar el nombre, precio, cÃÂÃÂ³digo de barras (opcional) y si es por peso.
 * @details Incluye validaciones y limpieza de datos antes de aceptar.
 */
class ProductoDialog : public wxDialog {
public:
	ProductoDialog(wxWindow* parent);///< Constructor que inicializa el diÃÂÃÂ¡logo con el padre dado.

    /**
	 * @brief Retorna el nombre del producto ingresado.
	 * @return Nombre del producto como wxString.
     */
	wxString GetNombre() const; 
    /**
     * @brief Retorna el precio del producto ingresado.
     * @return Precio del producto como double.
	 */
	double GetPrecio() const;
    /**
     * @brief Retorna el cÃÂÃÂ³digo de barras del producto ingresado.
     * @return CÃÂÃÂ³digo de barras como wxString (puede estar vacÃÂÃÂ­o si no se ingresÃÂÃÂ³).
	 */
	wxString GetCodigoBarras() const; 
    /**
     * @brief Indica si el producto es vendido por peso.
     * @return true si es por peso, false si es por unidad.
	 */
	bool EsPorPeso() const;  
    /**
     * @brief Aplica el tema claro u oscuro a la ventana y todos sus widgets hijos.
     * @param oscuro true para tema oscuro, false para tema claro.
     */
	void AplicarTema(bool oscuro);

protected:
	void Widgets(); ///< Crea y organiza los widgets en el diÃÂÃÂ¡logo.
	wxTextCtrl* txtNombre; ///< Campo de texto para el nombre del producto.
	wxTextCtrl* txtPrecio; ///< Campo de texto para el precio del producto.
	wxTextCtrl* txtCodigoBarras;  ///< Campo de texto para el cÃÂÃÂ³digo de barras (opcional).
	wxCheckBox* chkPorPeso; ///< Checkbox para indicar si es por peso.
	wxButton* btnOk; ///< BotÃÂÃÂ³n para aceptar y guardar.
	wxButton* btnCancel; ///< BotÃÂÃÂ³n para cancelar y cerrar.
	wxStaticText* precioLabel; ///< Etiqueta para el campo de precio (cambia si es por peso).
	bool temaOscuro = false; ///< Indica si el tema oscuro estÃÂÃÂ¡ activo.

    /**
	 * @brief Evento llamado al presionar el botÃÂÃÂ³n "Aceptar".
	 * @details Valida y limpia los datos ingresados (nombre, precio, cÃÂÃÂ³digo de barras).
	 * @param event Evento de comando estÃÂÃÂ¡ndar de wxWidgets.
     */
    void OnAceptar(wxCommandEvent& event);
    /**
	 * @brief Evento llamado al entrar el mouse sobre un botÃÂÃÂ³n.
	 * @param event Evento de mouse estÃÂÃÂ¡ndar de wxWidgets.
     * 
     */
    void OnButtonEnter(wxMouseEvent& event);
	/**
	* @brief Evento llamado al salir el mouse de un botÃÂÃÂ³n.
	* @param event Evento de mouse estÃÂÃÂ¡ndar de wxWidgets.
    */
    void OnButtonLeave(wxMouseEvent& event);
};