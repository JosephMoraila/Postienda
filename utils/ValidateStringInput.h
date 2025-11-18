/**
 * @file ValidateStringInput.h
 * @brief Funciones para limpiar, validar y truncar cadenas de texto y cÃÂÃÂ³digos de barras.
 *
 * Contiene utilidades para normalizar nombres, validar entradas de usuario,
 * truncar cadenas largas y limpiar cÃÂÃÂ³digos de barras en el contexto de la aplicaciÃÂÃÂ³n.
 */

#pragma once
#include <string>
#include <wx/string.h>

 /**
  * @brief Limpia y valida nombres eliminando caracteres no deseados.
  *
  * - Elimina espacios al inicio y final.
  * - Reemplaza mÃÂÃÂºltiples espacios en medio por uno solo.
  * - Elimina caracteres como &, <, >, /, \, :, |, ?, *, (, ), [, ], $, ".
  *
  * @param nombre Cadena de entrada en std::string.
  * @return Cadena limpia y validada.
  */
std::string LimpiarYValidarNombre(const std::string& nombre);

/**
 * @brief Sobrecarga para trabajar con wxString.
 * @param nombre Cadena de entrada en wxString.
 * @return Cadena limpia y validada como std::string.
 */
std::string LimpiarYValidarNombre(const wxString& nombre);

/**
 * @brief Valida si un nombre no estÃÂÃÂ¡ vacÃÂÃÂ­o despuÃÂÃÂ©s de limpiarlo.
 * @param nombre Cadena ya limpiada.
 * @return true si es vÃÂÃÂ¡lido, false si quedÃÂÃÂ³ vacÃÂÃÂ­o.
 */
bool EsNombreValido(const std::string& nombre);

/**
 * @brief Trunca nombres muy largos agregando puntos suspensivos.
 * @param nombre Cadena de entrada.
 * @param longitudMaxima Longitud mÃÂÃÂ¡xima permitida (por defecto 100).
 * @return Cadena truncada con "..." si excede la longitud.
 */
std::string TruncarNombre(const std::string& nombre, size_t longitudMaxima = 100);

/**
 * @brief Sobrecarga para truncar wxString.
 * @param nombre Cadena en wxString.
 * @param longitudMaxima Longitud mÃÂÃÂ¡xima permitida.
 * @return wxString truncado con "..." si excede la longitud.
 */
wxString TruncarNombreWxString(const wxString& nombre, size_t longitudMaxima);

/**
 * @brief Limpia un cÃÂÃÂ³digo de barras eliminando caracteres no alfanumÃÂÃÂ©ricos.
 * @param codigo Cadena con cÃÂÃÂ³digo de barras.
 * @return CÃÂÃÂ³digo limpio.
 */
std::string LimpiarCodigoBarras(const std::string& codigo);

/**
 * @brief Extrae solo el nombre del producto desde el texto del ÃÂÃÂ¡rbol.
 *
 * El texto seleccionado incluye precio y cÃÂÃÂ³digo de barras, esta funciÃÂÃÂ³n devuelve ÃÂÃÂºnicamente el nombre.
 *
 * @param textoItem Texto del ÃÂÃÂ­tem seleccionado en wxString.
 * @return Nombre limpio del producto en std::string.
 */
std::string ObtenerNombreProductoLimpio(const wxString& textoItem);

/**
 * @brief Sobrecarga que devuelve el nombre del producto como wxString.
 * @param textoItem Texto del ÃÂÃÂ­tem seleccionado en wxString.
 * @return Nombre limpio del producto en wxString.
 */
wxString ObtenerNombreProductoLimpioWX(const wxString& textoItem);

/**
 * @brief Limpia caracteres invÃÂÃÂ¡lidos al agregar un producto.
 * @param nombre Nombre con posibles caracteres invÃÂÃÂ¡lidos.
 * @return Nombre limpio.
 */
std::string LimpiarCaracteresInvalidosOnAddProduct(std::string nombre);

/**
 * @brief Limpia y valida nombres en wxString eliminando caracteres no deseados como multiples espacios.
 * @param nombre Cadena en wxString.
 * @return Cadena limpia. Example:  " Hello  /   World      " -> "Hello World"
 */
wxString LimpiarYValidarNombreWx(const wxString& nombre);
