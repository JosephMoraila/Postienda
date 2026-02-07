#include "constants/RES/ICON/ICON_PATH.hpp"
#include "constants/RES/RES_FOLDER_PATH.hpp"

std::string ICON_PATH() {
	std::string resFolder = RES_FOLDER_PATH();

#ifdef _WIN32
	
	std::string iconPath = "";

#else

	std::string iconPath = resFolder + "favicon.png"

#endif

	return iconPath;


}
