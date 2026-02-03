#include "constants/TICKET/TICKET_PATH.hpp"
#include "constants/FOLDERS/SETTINGS/SETTINGS_FOLDER.hpp"

#ifdef _WIN64
const std::string TICKET_PATH = SETTINGS_FOLDER_PATH() + "\\" + "ticket.zip"; /// Ruta del archivo de ticket
#elif defined(__linux__)
const std::string TICKET_PATH = SETTINGS_FOLDER_PATH() + "/" + "ticket.zip"; /// Ruta del archivo de ticket
#endif