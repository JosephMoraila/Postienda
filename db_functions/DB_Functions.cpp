#include "db_functions/DB_Functions.hpp"
#include "constants/DB/DB.h"
#include "sqlite_modern_cpp.h"
#include <wx/wx.h>
#include "utils/GetFromFile.h"

namespace DB_Functions {
	namespace Drawer_DB_Functions {
		bool Insert_Drawer_History(double& amount, bool& isAddition, std::string& reason, size_t* purchaseId) {
			try {
				sqlite::database db(GetDBPath());
				db << "PRAGMA foreign_keys = ON;";
				db << "PRAGMA encoding = 'UTF-8';";

				wxString currentDate = wxDateTime::Now().FormatISOCombined(' ');
				std::string dateStr = std::string(currentDate.mb_str());
				std::string actualUser = getUserFromJSON<std::string>();

				// Obtener el saldo actual del drawer
				double drawer_after_insertion = 0.0;
				db << "SELECT amount FROM drawer LIMIT 1" >> drawer_after_insertion;

				// Actualizar el saldo
				if (isAddition) drawer_after_insertion += amount;
				else drawer_after_insertion -= amount;

				// Definir condiciones UNA VEZ
				bool hasUser = !actualUser.empty() && actualUser != "Ninguno";
				bool hasReason = !reason.empty();
				bool hasPurchase = (purchaseId != nullptr);

				//  Construir query con las mismas condiciones
				std::string dbQuery = "INSERT INTO drawer_history (date, amount, is_addition, drawer_after_insertion";

				if (hasPurchase) dbQuery += ", purchase_id";
				if (hasUser) dbQuery += ", worker";
				if (hasReason) dbQuery += ", reason";

				dbQuery += ") VALUES (?, ?, ?, ?";

				if (hasPurchase) dbQuery += ", ?";
				if (hasUser) dbQuery += ", ?";
				if (hasReason) dbQuery += ", ?";

				dbQuery += ");";

				// Insertar datos
				auto statement = db << dbQuery;

				// Valores obligatorios
				statement << dateStr << amount << (isAddition ? 1 : 0) << drawer_after_insertion;

				// Valores opcionales (usando LAS MISMAS condiciones)
				if (hasPurchase) statement << *purchaseId;
				if (hasUser) statement << actualUser;
				if (hasReason) statement << reason;

				// Actualizar el drawer
				db << "UPDATE drawer SET amount = ?;" << drawer_after_insertion;

				return true;

			}
			catch (const sqlite::sqlite_exception& e) {
				wxMessageBox(_("Error making operation in drawer: ") + wxString::FromUTF8(e.what()),
					"Error", wxOK | wxICON_ERROR);
				return false;
			}
		}

		double GetCurrentDrawerAmount() {
			sqlite::database db(GetDBPath());
			db << "PRAGMA foreign_keys = ON;";
			db << "PRAGMA encoding = 'UTF-8';";
			//Comprobar si existe el campo del drawer, si no existe lo crea con valor 0.00
			int count = 0;
			db << "SELECT COUNT(*) FROM drawer" >> count;
			if (count == 0) db << "INSERT INTO drawer (amount) VALUES (0.00)";
			// Obtener el saldo actual del drawer
			double drawerMoney = 0.0;
			db << "SELECT amount FROM drawer LIMIT 1" >> drawerMoney;
			return drawerMoney;
		}

	}
}