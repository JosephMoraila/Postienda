#pragma once
#include <wx/wx.h>

/**
 * @namespace DB_ERROR_MESSAGES
 * @brief Contains database error message constants.
 */
namespace DB_ERROR_MESSAGES
{
	/// @brief Database error message format string.
    inline const wxString DB_ERROR_LINE_MESSAGE =
        _("Database error\n\nFile: %s\n""Line: %d\n\nError message:\n%s");
}
