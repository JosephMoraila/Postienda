#include <string>
#include "utils/MathUtils.hpp"

/**
    @struct ProductInfo
	@brief Struct that holds information about a product to be displayed in the GUI.
**/
struct ProductInfo {
    size_t id;
    std::string nombre;
    double precio;
    std::string codigoBarras;
    bool porPeso;
	std::string categoria; ///>Category of the product
    double cantidad;  ///< Cantidad con mÃ¡ximo 3 decimales
    double stock;

    /**
        @brief ProductInfo object constructor by default
    **/
    ProductInfo()
        : id(0), nombre(""), precio(0.0), codigoBarras(""), porPeso(false), categoria(""), cantidad(0.0), stock(0.0){}

    /**
        @brief Full ProductInfo object constructor
    **/
    ProductInfo(size_t id, const std::string& nombre, double precio, const std::string& codigoBarras, bool porPeso, const std::string& categoria)
        : id(id), nombre(nombre), precio(precio), codigoBarras(codigoBarras), porPeso(porPeso), categoria(categoria) {
    }
    ProductInfo(size_t id, const std::string& nombre, double precio, const std::string& codigoBarras, bool porPeso, const std::string& categoria, double stock)
        : id(id), nombre(nombre), precio(precio), codigoBarras(codigoBarras), porPeso(porPeso), categoria(categoria), stock(stock) {
    }

    /**
		@brief ProductInfo object constructor without category (for backward compatibility)
    **/
    ProductInfo(size_t id, const std::string& nombre, double precio, const std::string& codigoBarras, bool porPeso)
        : id(id), nombre(nombre), precio(precio), codigoBarras(codigoBarras), porPeso(porPeso), categoria("") {
    }
    ProductInfo(size_t id, const std::string& nombre, double precio, const std::string& codigoBarras, bool porPeso, double stock)
        : id(id), nombre(nombre), precio(precio), codigoBarras(codigoBarras), porPeso(porPeso), categoria(""), stock(stock) {
    }

    /**
		@brief Sets the quantity with a maximum of 3 decimal places.
		@param value - Quantity to set.
    **/
    void setCantidad(double value) {
        cantidad = round3(value);
    }
};
