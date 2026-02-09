#include "constants/RES/RES_FOLDER_PATH.hpp"
#include "constants/FOLDERS/SETTINGS/SETTINGS_FOLDER.hpp"

std::string RES_FOLDER_PATH(){

	std::string settingsFolder = SETTINGS_FOLDER_PATH();

	#ifdef _WIN32

	std::string resFolder = settingsFolder + "\\res\\";

	#else

	std::string resFolder = settingsFolder + "/res/";

	#endif

	return resFolder;

}
