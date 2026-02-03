#include "constants/DB/DB.h"
#include "constants/FOLDERS/SETTINGS/SETTINGS_FOLDER.hpp"

std::string GetDBPath() {
#ifdef _WIN64
    return SETTINGS_FOLDER_PATH() + "\\" + "dbpos.db";
#elif defined(__linux__)
    return SETTINGS_FOLDER_PATH() + "/" + "dbpos.db";
#endif

}