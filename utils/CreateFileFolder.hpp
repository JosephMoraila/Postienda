#include <filesystem>
#include <iostream>
#include "constants/FOLDERS/SETTINGS/SETTINGS_FOLDER.hpp"

namespace fs = std::filesystem;
/**
    @namespace CreateFolder
    @brief Create a foler namespace
**/
namespace CreateFolder {
    /**
        @brief Create the settings folder
    **/
    inline void CreateSettingsFolder() {
        fs::path dirPath = SETTINGS_FOLDER_PATH;
        fs::create_directories(dirPath);
    }
}