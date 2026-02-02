#include "utils/ValidateStringInput.h"
#include <algorithm>
#include <cctype>
#include <regex>
#include <wx/regex.h>


std::string LimpiarYValidarNombre(const std::string& nombre) {
    if (nombre.empty()) {
        return "";
    }

    std::string resultado = nombre;

    // === TRIM INICIO ===
    // Elimina todos los caracteres en blanco (espacios, tabulaciones, saltos de lÃÂÃÂ­nea, etc.)
    // que estÃÂÃÂ©n al principio de la cadena `resultado`.

    resultado.erase(
        resultado.begin(), // Punto de inicio para borrar
        std::find_if(       // Encuentra el primer carÃÂÃÂ¡cter que NO sea un espacio
            resultado.begin(), resultado.end(), // Busca desde el inicio hasta el final
            [](unsigned char ch) {
                // std::isspace(ch) devuelve true si es espacio, tab, salto de lÃÂÃÂ­nea, etc.
                // Al negar (!) queremos el primer carÃÂÃÂ¡cter que NO sea un espacio.
				return !std::isspace(ch); //Es decir, devuelve True si el caracter no es un espacio, y eso devuelve de find_if el iterador hasta el primer caracter no espacio
            }
        )
    );

    resultado.erase(
		std::find_if(
            resultado.rbegin(), //Se busca desde el final de la cadena
            resultado.rend(), //Hasta el principio, rend devuelve el iteador inverso
            [](unsigned char ch) {
        return !std::isspace(ch);
		}).base(), // base() convierte el iterador inverso a uno normal 
			resultado.end()); //Borra desde el primer espacio encontrado hasta el final de la cadena hasta el final de la cadena

    // Si estÃÂÃÂ¡ vacÃÂÃÂ­o despuÃÂÃÂ©s de limpiar espacios, retornar vacÃÂÃÂ­o
    if (resultado.empty()) {
        return "";
    }

    // Eliminar caracteres especiales peligrosos para JSON/archivos pero mantener acentos y ÃÂÃÂ±
    std::regex caracteresProhibidos(R"([\x00-\x1F\x7F\"\\/<>:\|?*\(\)\[\]])");
    resultado = std::regex_replace(resultado, caracteresProhibidos, "");

    // Reemplazar mÃÂÃÂºltiples espacios consecutivos con uno solo
    std::regex espaciosMultiples(R"(\s+)");
    resultado = std::regex_replace(resultado, espaciosMultiples, " ");

    // Limpiar espacios nuevamente despuÃÂÃÂ©s de las transformaciones
    resultado.erase(resultado.begin(), std::find_if(resultado.begin(), resultado.end(), [](unsigned char ch) {
        return !std::isspace(ch);
        }));
    resultado.erase(std::find_if(resultado.rbegin(), resultado.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
        }).base(), resultado.end());

    return resultado;
}


std::string LimpiarYValidarNombre(const wxString& nombre) {
    // Convertir wxString a std::string usando UTF-8
    std::string nombreStd = nombre.ToUTF8().data();
    return LimpiarYValidarNombre(nombreStd);
}

wxString LimpiarYValidarNombreWx(const wxString& nombre) {
    if (nombre.IsEmpty()) return "";

    wxString resultado = nombre;

    // Trim
    resultado.Trim(true);  // inicio
    resultado.Trim(false); // final

    if (resultado.IsEmpty()) return "";

    // Quitar caracteres prohibidos (usando wxRegEx)
    wxRegEx caracteresProhibidos("[\\x00-\\x1F\\x7F\"\\\\/<>:\\|?*\\(\\)\\[\\]]");
    caracteresProhibidos.ReplaceAll(&resultado, "");

    // Reemplazar mÃÂÃÂºltiples espacios
    wxRegEx espaciosMultiples("\\s+");
    espaciosMultiples.ReplaceAll(&resultado, " ");

    resultado.Trim(true).Trim(false);

    return resultado;
}


bool EsNombreValido(const std::string& nombre) {
    std::string nombreLimpio = LimpiarYValidarNombre(nombre);
    return !nombreLimpio.empty() && nombreLimpio.length() >= 1;
}

std::string TruncarNombre(const std::string& nombre, size_t longitudMaxima) {
    std::string nombreLimpio = LimpiarYValidarNombre(nombre);

	//Si nombre es mennor o igual a la longitud mÃÂÃÂ¡xima, no se necesita truncar
    if (nombreLimpio.length() <= longitudMaxima) {
        return nombreLimpio;
    }

	// Truncar y agregar puntos suspensivos, empezando desde el inicio y cortando a longitudMaxima - 3 para los tres puntos suspensivos
    std::string truncado = nombreLimpio.substr(0, longitudMaxima - 3);

    // Asegurarse de no cortar en medio de un carÃÂÃÂ¡cter UTF-8
    while (truncado.length() > 0 //Si la cadena truncada es mayor a 0 ya que no queremos cortar una cadena vacÃÂÃÂ­a
		&& (truncado.back() & 0x80) != 0 //Verifica si el ÃÂÃÂºltimo byte es parte de un carÃÂÃÂ¡cter UTF-8 multibyte, se le agrega la condicion si ni no es igual a 0 porque si es igual a 0 significa que es un carÃÂÃÂ¡cter ASCII de un byte
		//Ejemplo: En UTF-8 (usa multibyte) el primer bit siempre es 1, por lo que al comparar con & 0x80(127->01111111), si no es igual a 0 significa que es un carÃÂÃÂ¡cter UTF-8 multibyte y se corta para evitar corrupciones.
		&& (truncado.back() & 0x40) == 0) // Verifica que el segundo bit no sea 1, lo que indica que es un byte de continuaciÃÂÃÂ³n
	{   //Ejemplo: En UTF-8, los bytes de continuaciÃÂÃÂ³n tienen el primer bit en 1 y el segundo en 0, por lo que al comparar con & 0x40(64->01000000), si es igual a 0 significa que es un byte de continuaciÃÂÃÂ³n y se corta para evitar corrupciones.
        truncado.pop_back();// Quita el ÃÂÃÂºltimo byte, para no cortar en medio de un carÃÂÃÂ¡cter UTF-8 incompleto
    }

    return truncado + "...";
}

wxString TruncarNombreWxString(const wxString& nombre, size_t longitudMaxima) {
    // Convertimos wxString ÃÂ¢ÃÂÃÂ UTF-8 std::string
    std::string utf8 = nombre.ToUTF8().data();

    // Reutilizamos la versiÃÂÃÂ³n UTF-8 segura
    std::string truncadoUtf8 = TruncarNombre(utf8, longitudMaxima);

    // Convertimos UTF-8 ÃÂ¢ÃÂÃÂ wxString
    return wxString::FromUTF8(truncadoUtf8);
}

std::string LimpiarCodigoBarras(const std::string& codigo) {
    std::string codigoLimpio;

    // Mantener solo nÃÂÃÂºmeros y letras (sin espacios ni caracteres especiales)
    for (char c : codigo) {
        if (std::isalnum(c)) {
            codigoLimpio += c;
        }
    }

    // Limitar longitud (mÃÂÃÂ¡ximo 20 caracteres para cÃÂÃÂ³digos de barras)
    if (codigoLimpio.length() > 20) {
		// Si es muy largo, truncar a 20 caracteres
        codigoLimpio = codigoLimpio.substr(0, 20);
    }

    return codigoLimpio;
}

std::string ObtenerNombreProductoLimpio(const wxString& textoItem) {
    std::string texto = textoItem.ToUTF8().data();  //Usa UTF-8

    // Eliminar el punto inicial si existe (y espacio)
    if (!texto.empty() && texto[0] == '-') {
		texto = texto.substr(2); // Salta "- " quitando los dos primeros caracteres
    }

    /* Buscar parÃÂÃÂ©ntesis(precio) - puede ser " ($" o solo " (" 
    Si la encuentra, devuelve la posiciÃÂÃÂ³n donde empieza esa secuencia
    Si NO la encuentra, devuelve std::string::npos (un valor especial que significa "no encontrado")*/
    size_t posParentesis = texto.find(" (");
    if (posParentesis != std::string::npos) {
		texto = texto.substr(0, posParentesis); // Sustrae el texto desde posiciÃÂÃÂ³n 0 hasta el inicio de la posiciÃÂÃÂ³n dejando afuera " ("
    }

    // TambiÃÂÃÂ©n buscar corchetes [ID] por si acaso
    size_t posCorchete = texto.find(" [");
    if (posCorchete != std::string::npos) {
        texto = texto.substr(0, posCorchete); // Solo el nombre
    }

    return texto;
}

wxString ObtenerNombreProductoLimpioWX(const wxString& textoItem) {
    wxString texto = textoItem;

    // Eliminar el punto inicial si existe (y espacio)
    if (!texto.IsEmpty() && texto[0] == '-') {
        texto = texto.Mid(2); // Salta "- " quitando los dos primeros caracteres
    }

    // Buscar parÃÂÃÂ©ntesis (precio)
    int posParentesis = texto.Find(" (");
    if (posParentesis != wxNOT_FOUND) {
        texto = texto.Left(posParentesis); // Solo el nombre antes del parÃÂÃÂ©ntesis
    }

    // Buscar corchetes [ID]
    int posCorchete = texto.Find(" [");
    if (posCorchete != wxNOT_FOUND) {
        texto = texto.Left(posCorchete); // Solo el nombre
    }

    return texto; // wxString con UTF-8 intacto
}


std::string LimpiarCaracteresInvalidosOnAddProduct(std::string nombre) {
    // Eliminar caracteres especiales peligrosos para JSON/archivos pero mantener acentos y ÃÂÃÂ±
    std::regex caracteresProhibidos(R"([\x00-\x1F\x7F\"\\/<>:\|?*\(\)\[\]\$])");
    nombre = std::regex_replace(nombre, caracteresProhibidos, "");
    // Reemplazar mÃÂÃÂºltiples espacios consecutivos con uno solo
    std::regex espaciosMultiples(R"(\s+)");
    nombre = std::regex_replace(nombre, espaciosMultiples, " ");
    // Limpiar espacios al inicio y al final
    nombre.erase(nombre.begin(), std::find_if(nombre.begin(), nombre.end(), [](unsigned char ch) {
        return !std::isspace(ch);
        }));
    nombre.erase(std::find_if(nombre.rbegin(), nombre.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
        }).base(), nombre.end());
	return nombre;
}

bool EsNumerico(const std::string& str) {
    if (str.empty()) return false;
    return std::all_of(str.begin(), str.end(), [](unsigned char c) { return std::isdigit(c); });
}