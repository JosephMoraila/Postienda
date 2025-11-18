/*****************************************************************//**
 * \file   ChangeProductDialog.h
 * \brief  Archivo que contiene la definicion de la clase ChangeProductDialog.
 * 
 * \author JosephMoraila
 * \date   August 2025
 *********************************************************************/

#pragma once
#include <wx/wx.h>
#include "include/models.hpp"
#include "gui/ProductosFrame/ProductoDialog/ProductoDialog.h"
#include <memory>

/**
 * @class ChangeProductDialog
 * @brief DiÃÂÃÂ¡logo para editar un producto existente.
 *
 * @details
 * Hereda de @ref ProductoDialog y permite modificar los datos de un producto ya existente.
 * Al abrir el diÃÂÃÂ¡logo, los campos se inicializan con los valores actuales del producto.
 * Incluye validaciones y aplica el tema claro/oscuro segÃÂÃÂºn configuraciÃÂÃÂ³n.
 */
class ChangeProductDialog : public ProductoDialog {
public:
    /**
     * @brief Constructor del diÃÂÃÂ¡logo de ediciÃÂÃÂ³n de producto.
     * @param parent Ventana padre que contendrÃÂÃÂ¡ el diÃÂÃÂ¡logo.
     * @param product Referencia al producto a editar. Los cambios se aplican directamente sobre este objeto.
     */
    ChangeProductDialog(wxWindow* parent, Producto& product);

private:
    /**
     * @brief Referencia al producto que se va a editar.
     * @details Permite modificar directamente los datos del producto original.
     */
    Producto& m_product;

    /**
     * @brief Inicializa los campos del diÃÂÃÂ¡logo con los datos actuales del producto.
     * @details Se llama en el constructor para mostrar los valores actuales en los controles de ediciÃÂÃÂ³n.
     */
    void EstablecerDatos();
};