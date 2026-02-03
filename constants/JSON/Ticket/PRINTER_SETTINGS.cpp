#include "constants/JSON/Ticket/PRINTER_SETTINGS.hpp"
#include "constants/FOLDERS/SETTINGS/SETTINGS_FOLDER.hpp"

#ifdef _WIN64
const std::string PRINT_SETTINGS_PATH = SETTINGS_FOLDER_PATH() + "\\" + "printerSettings.json"; /// Ruta del archivo de settings de la impresora
#elif defined(__linux__)
const std::string PRINT_SETTINGS_PATH = SETTINGS_FOLDER_PATH() + "/" + "printerSettings.json"; /// Ruta del archivo de settings de la impresora
#endif
