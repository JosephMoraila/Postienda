#include "constants/FOLDERS/SETTINGS/SETTINGS_FOLDER.hpp"
#include "constants/APPNAME/APPNAME.h"
#include <filesystem>

namespace fs = std::filesystem;

#ifdef _WIN32
const std::string SETTINGS_FOLDER_PATH = []() {
    std::string path = APPNAME + "\\"+  "settings\\";
    fs::create_directories(path);
    return path;
    }();
#else
const std::string SETTINGS_FOLDER_PATH = []() {
    std::string path = std::string("PostiendaDir") + "/" + "settings/";
    fs::create_directories(path);
    return path;
    }();
#endif