#include "constants/FOLDERS/LOCALE/LOCALE_FOLDER.hpp"

#ifdef _WIN32
const std::string LOCALE_FOLDER_PATH = "locale\\";
#else
const std::string LOCALE_FOLDER_PATH = "locale/";
#endif