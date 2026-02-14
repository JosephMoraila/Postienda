#include <string>

namespace DB_Functions{

	namespace Drawer_DB_Functions {
	
		/**
		 * @brief Inserts a record into the drawer history and updates the drawer amount accordingly.
		 * @param amount The amount to add or withdraw from the drawer.
		 * @param isAddition A boolean indicating whether the amount is an addition (true) or withdrawal (false).
		 * @param reason A string describing the reason for the drawer operation.
		 * @param purchaseId (Optional) A pointer to a size_t representing the purchase ID
		 */
		bool Insert_Drawer_History(double& amount, bool& isAddition, std::string& reason, size_t* purchaseId = nullptr);

		/** @brief Get the amount in drawer */
		double GetCurrentDrawerAmount();
	
	}

}
