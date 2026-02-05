/*****************************************************************//**
 * \file   GetFromFile.h
 * \brief  Functions that get data from files.
 * 
 * \author JosephMoraila
 * \date   August 2025
 *********************************************************************/

#pragma once
#include <string>
#include <wx/wx.h>
#include "constants/JSON/Session/Session.h"
#include "third_party/json.hpp"
#include <fstream>
#include "utils/ValidateStringInput.h"
using json = nlohmann::json;

/**
 * @brief Template function to get the current user from the session JSON file.
 *
 * @tparam T : The return type of the function. Must be a type that can hold a username
 *           (e.g., std::string or wxString). No generic implementation exists;
 *           only specializations are provided.
 *
 * @note This is only a declaration. Specialized versions must be used for actual types.
 *       For example:
 *       - getUserFromJSON<std::string>()
 *       - getUserFromJSON<wxString>()
 *
 * @return A value of type T representing the current username.
 */
template<typename T> T getUserFromJSON();


/**
 * @brief Gets the current username from the session JSON file as a wxString.
 *
 * @details
 * This is a **complete specialization** of the `getUserFromJSON` template function for `wxString`.
 * - `template<>` indicates that it's a complete specialization template, `< >` means there are no generic parameters.
 * - `<wxString>` after the function name indicates what type this function works with.
 *   Since the function does **not receive a generic parameter**, the compiler cannot deduce
 *   the type automatically. Therefore, when calling `getUserFromJSON`, you **must explicitly
 *   specify the type** you want to work with, in this case `wxString`.
 *
 * The function performs the following steps:
 * 1. Opens the session JSON file defined by `JSON_SESSION_PATH`.
 * 2. If the file cannot be opened, returns `"Ninguno"`.
 * 3. Reads the JSON content and checks if it contains a `"session"` field of type string.
 * 4. Converts the value from UTF-8 to a `wxString`.
 * 5. Sanitizes and validates the username using `LimpiarYValidarNombre`.
 * 6. If the sanitized username is empty, returns `"Ninguno"`.
 * 7. Otherwise, returns the valid username as a `wxString`.
 *
 * @return wxString The username retrieved from the session file. Returns `"Ninguno"` if
 *         the file cannot be read, the JSON is invalid, or the username is empty after sanitization.
 */
template<> // template<> indicates that it's a complete specialization template, <> means there are no generic parameters
inline wxString getUserFromJSON<wxString>() { // <wxString> indicates the type this function works with because the function doesn't receive a generic parameter, so the type must be specified explicitly
    std::ifstream archivoIn(JSON_SESSION_PATH);
    if (!archivoIn.is_open()) {
        return "Ninguno";
    }

    try {
        json sesionJson;
        archivoIn >> sesionJson;

        if (sesionJson.contains("session") && sesionJson["session"].is_string()) {
            wxString nombreUsuarioSeleccionado = wxString::FromUTF8(sesionJson["session"].get<std::string>());
            std::string userNombreSanitizado = LimpiarYValidarNombre(nombreUsuarioSeleccionado);
            nombreUsuarioSeleccionado = wxString::FromUTF8(userNombreSanitizado);
            if (nombreUsuarioSeleccionado.IsEmpty()) {
                return "Ninguno";
            }
            return nombreUsuarioSeleccionado;
        }
    }
    catch (...) {
        return "Ninguno";
    }
    return "Ninguno";
}


/**
 * @brief Gets the current username from the session JSON file as a std::string.
 *
 * @details
 * This is a **specialization** of the `getUserFromJSON` template function for `std::string`.
 * The function follows these steps:
 * 1. Opens the session JSON file specified by `JSON_SESSION_PATH`.
 * 2. If the file cannot be opened, returns `"Ninguno"`.
 * 3. Reads the JSON content and checks if it contains a `"session"` field of type string.
 * 4. Extracts the string value directly.
 * 5. Sanitizes and validates the username using `LimpiarYValidarNombre`.
 * 6. If the sanitized username is empty, returns `"Ninguno"`.
 * 7. Otherwise, returns the original username string from the file.
 *
 * @note This is a template specialization. The generic `getUserFromJSON<T>()` template
 *       has no general implementation and only supports specialized types like `std::string` and `wxString`.
 *
 * @return std::string The username retrieved from the session file. Returns `"Ninguno"` if
 *         the file cannot be read, the JSON is invalid, or the username is empty after sanitization.
 */
template<> inline std::string getUserFromJSON<std::string>() {
    std::ifstream archivoIn(JSON_SESSION_PATH);
    if (!archivoIn.is_open()) {
        return "Ninguno";
    }

    try {
        json sesionJson;
        archivoIn >> sesionJson;

        if (sesionJson.contains("session") && sesionJson["session"].is_string()) {
            std::string nombreUsuarioSeleccionado = sesionJson["session"].get<std::string>();
            std::string userNombreSanitizado = LimpiarYValidarNombre(nombreUsuarioSeleccionado);
            if (userNombreSanitizado.empty()) {
                return "Ninguno";
            }
            return nombreUsuarioSeleccionado;
        }
    }
    catch (...) {
        return "Ninguno";
    }
    return "Ninguno";
}

inline bool FileExists(const std::string& path) {
    return std::filesystem::exists(path) && std::filesystem::is_regular_file(path);
}
