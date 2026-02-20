#pragma once

#include <wx/string.h>

namespace Translate_Utils {
	/**
	 * @brief Returns the translated method
	 * @param cashPayment: true if the payment method is cash, false if it's card
	 * @return the translated method
	 */
	wxString GetTranslatedPaymentMethod(std::string method);

	/**
	 * @brief Retorna las traducciones si es suma o retiro de drawer
	 * @param Bool si es adicion/suma
	 * @return exString con la traduccion
	 */
	wxString GetTranslitionAdditionOrWithdrawal(bool isAddition);
}
