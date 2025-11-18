/*****************************************************************//**
 * @file   models.hpp
 * @brief  Definiciones de las estructuras de datos para productos y categorÃ­as.
 * @author JosephMoraila
 * @date   August 2025
 *********************************************************************/

#pragma once
#include <string>
#include <vector>
#include <memory>

 /**
  * @struct Producto
  * @brief Representa un producto dentro del sistema.
  *
  * @par Campos
  * - `std::string nombre` Â Nombre del producto.
  * - `double precio` Â Precio unitario.
  * - `std::string codigoBarras` Â CÃ³digo de barras (opcional).
  * - `bool porPeso` Â Si se vende por peso (por defecto `false`).
  *
  * @details
  * Un producto contiene nombre, precio, cÃ³digo de barras y una bandera para indicar
  * si se vende por peso (ejemplo: frutas, verduras).
  */
struct Producto {
    /**ID unico del producto*/
    size_t Id;

    /** Nombre del producto. */
    std::string nombre;

    /** Precio unitario del producto. */
    double precio;

    /** CÃ³digo de barras asociado al producto. */
    std::string codigoBarras;

    /**
     * Indica si el producto se vende por peso.
     *
     * - `true`: vendido por peso (ej. frutas, verduras).
     * - `false`: vendido por unidad.
     *
     * Valor por defecto: `false`.
     */
    bool porPeso = false;

    /**
     * @brief Constructor por defecto.
     * Inicializa: precio = 0.0, cÃ³digo de barras vacÃ­o y porPeso = false.
     */
    Producto() : precio(0.0), codigoBarras(""), porPeso(false) {}

    /**
     * @brief Constructor con parÃ¡metros.
     * @param n Nombre del producto.
     * @param p Precio del producto.
     * @param cb CÃ³digo de barras (opcional, por defecto vacÃ­o).
     * @param byWeight `true` si es por peso (opcional, por defecto `false`).
     */
    Producto(const std::string& n, double p, const std::string& cb = "", bool byWeight = false)
        : nombre(n), precio(p), codigoBarras(cb), porPeso(byWeight) {
    }
    /**
        @brief Producto object constructor for id, name, price, barcode and is byWeight
        @param id       - id of the product
        @param n        - name of the product
        @param p        - Price of the product
        @param cb       - barcode of the product
        @param byWeight - Sell by unit or by weight
    **/
    Producto(const size_t id ,const std::string& n, double p, const std::string& cb = "", bool byWeight = false)
        : Id(id), nombre(n), precio(p), codigoBarras(cb), porPeso(byWeight) {
    }
};


/**
 * @struct Categoria
 * @brief Representa una categorÃ­a de productos.
 *
 * @par Campos
 * - `size_t idCategoria` Â Identificador Ãºnico.
 * - `std::string nombre` Â Nombre de la categorÃ­a.
 * - `std::vector<Producto> productos` Â Lista de productos asociados.
 * - `std::vector<std::shared_ptr<Categoria>> subcategorias` Â Hijas exclusivas de esta categorÃ­a.
 *
 * @details
 * Cada categorÃ­a contiene un identificador, un nombre, productos y un conjunto de
 * subcategorÃ­as hijas.
 *
 * @see Producto
 */
struct Categoria {
    /** Identificador Ãºnico de la categorÃ­a. */
    size_t idCategoria;

    /** Nombre de la categorÃ­a. */
    std::string nombre;

    /**
     * Lista de productos dentro de esta categorÃ­a.
     * Se usa `std::vector<Producto>` para almacenar mÃºltiples productos.
     */
    std::vector<std::shared_ptr<Producto>> productos;

    /**
     * SubcategorÃ­as que pertenecen exclusivamente a esta categorÃ­a.
     *
     * Vector de `std::shared_ptr<Categoria>` para gestionar memoria de forma automÃ¡tica.
     * Cada subcategorÃ­a pertenece Ãºnicamente a su categorÃ­a padre.
     */
    std::vector<std::shared_ptr<Categoria>> subcategorias;
};
