#include "constants/DB/DB.h"
#include "constants/FOLDERS/SETTINGS/SETTINGS_FOLDER.hpp"

std::string GetDBPath() {
    return SETTINGS_FOLDER_PATH + "dbpos.db";
}