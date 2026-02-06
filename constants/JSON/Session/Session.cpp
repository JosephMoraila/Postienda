#include "constants/JSON/Session/Session.h"
#include "constants/FOLDERS/SETTINGS/SETTINGS_FOLDER.hpp"

#ifdef _WIN64
const std::string JSON_SESSION_PATH = SETTINGS_FOLDER_PATH() + "\\" + "session.json"; // Ruta al json de sesiÃÂ³n
#elif defined(__linux__)
const std::string JSON_SESSION_PATH = SETTINGS_FOLDER_PATH() + "/" + "session.json";
#endif

