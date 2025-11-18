/**
@file MathUtils.hpp
@brief Funciones matemÃ¡ticas Ãºtiles.
*/

#pragma once
#include <cmath>

/**
@brief Redondea un numero decimal a dos decimales. Ejemplo: 12.34567 -> 12.35
@param price: numero decimal a redondear.
@return numero redondeado a dos decimales.
*/
inline double round2(double price) {
    return std::round(price * 100.0) / 100.0;
}

/**
	@brief Redondea un nÃºmero decimal a tres decimales. Ejemplo: 12.34567 -> 12.346
	@param  value - nÃºmero decimal a redondear.
	@retval - nÃºmero redondeado a tres decimales.
**/
inline double round3(double value) {
    return std::round(value * 1000.0) / 1000.0;
}

#pragma once
#include <wx/string.h>
#include <type_traits>

/**
 * @brief Formatea un nÃºmero entero agregando comas cada tres dÃ­gitos.
 * @tparam T Tipo entero (signed o unsigned). Se verifica en tiempo de compilaciÃ³n.
 * @param value NÃºmero a formatear.
 * @return wxString con el nÃºmero formateado con comas. Los negativos conservan el signo.
 *
 * @details
 * Esta funciÃ³n permite formatear nÃºmeros enteros de cualquier tamaÃ±o,
 * incluyendo int, long, long long, unsigned int, size_t y unsigned long long.
 * Siempre utiliza coma como separador de miles.
 *
 * @code
 * int x = 1234567;
 * wxString s = utils::FormatWithCommas(x); // "1,234,567"
 *
 * long long y = -987654321;
 * wxString t = utils::FormatWithCommas(y); // "-987,654,321"
 *
 * unsigned long long z = 1000000000;
 * wxString u = utils::FormatWithCommas(z); // "1,000,000,000"
 * @endcode
 */
template<typename T>
inline wxString FormatWithCommas(T value) {
    static_assert(std::is_integral<T>::value, "FormatWithCommas only admits integer");
    // Verifica en tiempo de compilaciÃ³n que T sea un tipo entero

    bool negativo = false;
    // Bandera para recordar si el nÃºmero es negativo

    using unsigned_T = typename std::make_unsigned<T>::type;
    // Define un tipo unsigned equivalente a T (para manejar valor absoluto seguro)
    unsigned_T absValue;
    // Variable para almacenar el valor absoluto del nÃºmero

    if constexpr (std::is_signed<T>::value) {
        // Si T es un tipo signed (int, long, long long, etc.)
        if (value < 0) {
            negativo = true;
            // Marcamos como negativo
            absValue = -static_cast<long long>(value);
            // Tomamos el valor absoluto para formatear (ej: -123 -> 123)
        }
        else {
            absValue = value;
            // NÃºmero positivo, no hay cambio
        }
    }
    else {
        absValue = value;
        // Para tipos unsigned (ej: size_t, unsigned long long), solo asignamos
    }

    wxString str = wxString::Format("%llu", absValue);
    // Convertimos el valor absoluto a string, ej: 1234567 -> "1234567"

    int insertPosition = str.Length() - 3;
    // PosiciÃ³n inicial para insertar la primera coma desde la derecha

    while (insertPosition > 0) {
        str = str.Left(insertPosition) + "," + str.Mid(insertPosition);
        // Inserta una coma en la posiciÃ³n calculada, ej: "1234567" -> "1,234567"
        insertPosition -= 3;
        // Movemos 3 posiciones hacia la izquierda para la siguiente coma
    }

    if (negativo) str = "-" + str;
    // Si el nÃºmero era negativo, agregamos el signo al inicio, ej: "123,456" -> "-123,456"

    return str;
    // Retorna el wxString final formateado
}

/**
 * @brief Formatea un nÃºmero flotante agregando comas a la parte entera y opcionalmente fija decimales.
 * @tparam T Tipo flotante (float o double). Se verifica en tiempo de compilaciÃ³n.
 * @param value NÃºmero a formatear.
 * @param decimals Cantidad de decimales a mostrar (por defecto 2).
 * @return wxString con el nÃºmero formateado, parte entera con comas y decimales con punto.
 *
 * @code
 * double x = 1234567.89;
 * wxString s = FormatFloatWithCommas(x); // "1,234,567.89"
 *
 * float y = -98765.4321f;
 * wxString t = FormatFloatWithCommas(y, 3); // "-98,765.432"
 * @endcode
 */
template<typename T>
inline wxString FormatFloatWithCommas(T value, int decimals = 2) {
    static_assert(std::is_floating_point<T>::value, "FormatFloatWithCommas only admits float o double");

    bool negativo = false;
    if (value < 0) {
        negativo = true;
        value = -value; // tomamos valor absoluto
    }

    // Separamos parte entera y decimal
    unsigned long long entero = static_cast<unsigned long long>(std::floor(value));
    T fraccion = value - entero;

    // Formateamos la parte entera con comas
    wxString strEntera = FormatWithCommas(entero);

    // Formateamos la parte decimal
    wxString strDecimal;
    if (decimals > 0) {
        // Redondeamos la fracciÃ³n a los decimales deseados
        unsigned long long dec = static_cast<unsigned long long>(std::round(fraccion * std::pow(10, decimals)));
        strDecimal = wxString::Format(".%0*llu", decimals, dec); // rellena con ceros a la izquierda si hace falta
    }

    wxString resultado = strEntera + strDecimal;
    if (negativo) resultado = "-" + resultado;

    return resultado;
}

/**
    @brief Formatea el texto en un wxTextCtrl para mostrar nÃºmeros con comas como separadores de miles y un nÃºmero fijo de decimales.
    @param ctrl - Puntero al wxTextCtrl que se va a formatear.
    @param decimals - NÃºmero de decimales a mostrar (por defecto es 2).
**/
inline void FormatTextCtrlWithCommas(wxTextCtrl* ctrl, int decimals = 2) {
    if (!ctrl) return;

    long pos = ctrl->GetInsertionPoint();
    wxString raw = ctrl->GetValue();

    if (raw.IsEmpty()) return;

    // Separar parte entera y decimal
    wxString intPart, fracPart;
    int dotPos = raw.Find('.');
    if (dotPos != wxNOT_FOUND) {
        intPart = raw.Left(dotPos);
        fracPart = raw.Mid(dotPos); // incluye el '.'
    }
    else {
        intPart = raw;
        fracPart = "";
    }

    // Remover comas de la parte entera
    intPart.Replace(",", "");

    wxString formattedInt;
    try {
        if (!intPart.IsEmpty()) {
            formattedInt = FormatWithCommas(std::stoull(std::string(intPart.mb_str())));
        }
        else {
            formattedInt = "";
        }
    }
    catch (const std::exception&) {
        // Si no se puede convertir, dejamos la parte entera como estaba
        formattedInt = intPart;
    }

    wxString formatted = formattedInt + fracPart;

    if (formatted != ctrl->GetValue()) {
        ctrl->ChangeValue(formatted);
        long diff = formatted.Length() - raw.Length();
        ctrl->SetInsertionPoint(pos + diff);
    }
}
