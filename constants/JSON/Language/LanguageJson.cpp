#include "constants/JSON/Language/LanguageJson.hpp"
#include "constants/FOLDERS/SETTINGS/SETTINGS_FOLDER.hpp"
#include "constants/FOLDERS/LOCALE/LOCALE_FOLDER.hpp"

std::string GetJsonLanguagePath() {
    // Calcular la ruta en tiempo de ejecución evita problemas de orden de inicialización.
#ifdef _WIN64
    return SETTINGS_FOLDER_PATH() + "\\" + LOCALE_FOLDER_PATH + "language.json";
#elif defined(__linux__)
    return SETTINGS_FOLDER_PATH() + "/" + LOCALE_FOLDER_PATH + "language.json";
#endif

}