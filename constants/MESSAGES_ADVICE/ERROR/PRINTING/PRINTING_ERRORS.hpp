#pragma once
#include <wx/wx.h>

/**
    @namespace PRINTING_ERROR_MESSAGES
    @brief Contains printing error Messages
**/
namespace PRINTING_ERROR_MESSAGES
{
	inline const wxString INVALID_PRINTER_MESSAGE = _("You have not selected a valid printer.");  ///INVALID PRINTER
	inline const wxString PRINT_ERROR = _("Printing Error");  ///PRINT ERROR
	inline const wxString NO_ITEMS_TICKET_MESSAGE = _("No items found in ticket.");
	inline const wxString ERROR_GETTING_MODE_MESSAGE = _("An error occurred while retrieving mode.");
	inline const wxString ERROR_PRINTING_TICKET_MESSAGE = _("Error printing the ticket:\n");
}


