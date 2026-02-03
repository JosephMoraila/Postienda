#include "constants/JSON/DarkMode/DarkMode.h"
#include "constants/FOLDERS/SETTINGS/SETTINGS_FOLDER.hpp"

#ifdef _WIN64
const std::string JSON_DARKMODE_PATH = SETTINGS_FOLDER_PATH() + "\\" + "darkMode.json"; // Ruta al json de modo oscuro
#elif defined(__linux__)
const std::string JSON_DARKMODE_PATH = SETTINGS_FOLDER_PATH() + "/" + "darkMode.json"; // Ruta al json de modo oscuro
#endif
