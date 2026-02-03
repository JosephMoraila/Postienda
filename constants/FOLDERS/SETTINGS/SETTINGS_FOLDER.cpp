#pragma once

#include "constants/APPNAME/APPNAME.h"
#include "constants/FOLDERS/SETTINGS/SETTINGS_FOLDER.hpp"
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

/**
 * @brief If debug mode is enabled the settings folder is created on the executable path, if not in appdata/.config folder
 *
 * \return path / APPNAME / settings
 */
std::string SETTINGS_FOLDER_PATH() {

    fs::path base;

#if defined(_DEBUG) || !defined(NDEBUG)
    // -----------------------------
    //          DEBUG MODE
    // -----------------------------
    base = fs::current_path() / APPNAMEDIR / "settings";

#else
    // -----------------------------
    //         RELEASE MODE
    // -----------------------------
#ifdef _WIN64
    base = fs::path(getenv("APPDATA")) / APPNAME / "settings";
#elif defined(__linux__)
    base = fs::path(getenv("HOME")) / ".config" / APPNAME / "settings";
#endif

#endif // DEBUG/RELEASE

    // Create folders recurcively
    fs::create_directories(base);

    return base.string();
}
