#include "utils/translate/Translate_Utils.hpp"
#include <wx/intl.h>

namespace Translate_Utils
{
	wxString GetTranslatedPaymentMethod(std::string method) {
        wxString translatedText;
        if (method == "Cash") translatedText = _("Cash");
        else if (method == "Card") translatedText = _("Card");
        else translatedText = method; // fallback
		return translatedText;
	}
}
