#pragma once

#include <wx/string.h>

namespace Translate_Utils {
	/**
	 * @brief Returns the translated method
	 * @param cashPayment: true if the payment method is cash, false if it's card
	 * @return the translated method
	 */
	wxString GetTranslatedPaymentMethod(std::string method);
}
