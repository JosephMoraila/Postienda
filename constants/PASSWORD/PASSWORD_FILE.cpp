#include "constants/PASSWORD/PASSWORD_FILE.hpp"
#include "constants/FOLDERS/SETTINGS/SETTINGS_FOLDER.hpp"

std::string GetPasswordPath() {
#ifdef _WIN64
    return SETTINGS_FOLDER_PATH() + "\\" + "pw.dat";
#elif defined(__linux__)
    return SETTINGS_FOLDER_PATH() + "/" + "pw.dat";
#endif

}