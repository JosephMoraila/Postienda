#include "gui/ActionsFrame/ActionsFrame.hpp"
#include "constants/DB/DB.h"
#include <wx/msgdlg.h>
#include <wx/filefn.h> 
#include "sqlite_modern_cpp.h"
#include "utils/MathUtils.hpp"
#include "utils/translate/Translate_Utils.hpp"
#include "utils/DateTimeUtils.hpp"

void ActionsFrame::GetActionsBetweenDates(std::string startDateTime, std::string endDateTime) {
	try {
        this->income = 0.0;
        this->withdrawals = 0.0;
        this->total = 0.0;
        wxString dbPath = GetDBPath();
        // Verificar si existe el archivo de base de datos
        if (!wxFileExists(dbPath)) {
            wxMessageBox(_("The database was not found at:\n") + dbPath, _("Database error"), wxOK | wxICON_ERROR, this);
            return;
        }
        sqlite::database db(GetDBPath());
        db << "PRAGMA encoding = 'UTF-8';";
        std::string query =
            "SELECT * FROM ("
            "   SELECT id, date, total AS amount, worker, method AS extra_info, 'PURCHASE' AS type FROM purchases "
            "   UNION ALL "
            "   SELECT id, date, amount, worker, reason AS extra_info, 'DRAWER' AS type FROM drawer_history WHERE purchase_id IS NULL "
            "   UNION ALL "
            "   SELECT rp.id, rp.return_date AS date, rp.price_at_return AS amount, rp.worker, pi.product_name AS extra_info, 'RETURN' AS type "
            "   FROM returned_products rp "
            "   JOIN purchase_items pi ON pi.id = rp.purchase_item_id "
            ") "
            "WHERE date >= ? AND date <= ? "
            "ORDER BY date ASC;";
                  
        db << query << startDateTime << endDateTime
            >> [&](size_t id, std::string date, double amount, std::string worker, std::string extra_info, std::string type) {

            // Extraer solo la fecha (YYYY-MM-DD) del registro
            std::string onlyDate = date.substr(0, 10);
            auto it = fechaToItem.find(onlyDate);
            if (it == fechaToItem.end()) return; // fecha no encontrada

            wxTreeItemId parent = it->second;
            if (!parent.IsOk()) return;

            auto [fechaFormateada, hora] = DateTimeUtils::FormatDateTimeLocalized(date);

            if (type == "PURCHASE") {
                wxString translatedText = Translate_Utils::GetTranslatedPaymentMethod(extra_info);
                this->income += amount;
                wxString str = wxString::Format(_("- Purchase: ID: %zu | %s | %s | %s | %s"),id, FormatFloatWithCommas(amount), wxString::FromUTF8(worker), translatedText, hora);
                wxTreeItemId purchaseItemId = treeDays->AppendItem(parent, str);
                db << "SELECT pi.product_name, pi.quantity, pi.price_at_purchase, p.barcode FROM purchase_items pi LEFT JOIN products p ON pi.product_id = p.id WHERE pi.purchase_id = ?" << id >>
                    [&](std::string productName, double quantity, double price, std::string barcode) {
                    std::variant<size_t, double> variantQuantity = DoubleToVariant(quantity);
                    wxString quantityStr = "";
                    if (std::holds_alternative<size_t>(variantQuantity)) quantityStr = FormatWithCommas(std::get<size_t>(variantQuantity));
                    else quantityStr = FormatFloatWithCommas(std::get<double>(variantQuantity), 3);
                    wxString strCertainProductPurchase = wxString::Format(_("%s | %s | %s | %s"), wxString::FromUTF8(productName), quantityStr, FormatFloatWithCommas(price), barcode);
                    treeDays->AppendItem(purchaseItemId, strCertainProductPurchase);
                };
            }
            else if (type == "DRAWER") {
                bool isAddition = false;
                db << "SELECT is_addition FROM drawer_history WHERE id = ?" << id >> isAddition;
                isAddition ? this->income += amount : this->withdrawals += amount;
                wxString translatedText = Translate_Utils::GetTranslitionAdditionOrWithdrawal(isAddition);
                wxString str = wxString::Format(_("- Drawer: ID: %zu | %s | %s | %s | %s | %s"),id, FormatFloatWithCommas(amount), wxString::FromUTF8(worker), translatedText, wxString::FromUTF8(extra_info), hora);
                treeDays->AppendItem(parent, str);
            }
            else if (type == "RETURN") {
                this->withdrawals += amount;
                size_t purchaseID = 0;
                db << "SELECT purchase_id FROM returned_products WHERE id = ?" << id >> purchaseID;
                wxString str = wxString::Format(_("- Return: ID: %zu | %s | %s | Purchase ID: %zu | %s | %s"), id, FormatFloatWithCommas(amount), wxString::FromUTF8(worker), purchaseID, wxString::FromUTF8(extra_info), hora);
                treeDays->AppendItem(parent, str);
            }
        };
        this->total = this->income - this->withdrawals;
	}
    catch (const std::exception& e) {
        this->income = 0.0;
        this->withdrawals = 0.0;
        this->total = 0.0;
        wxMessageBox(wxString::Format("Unexpected error: %s", e.what()), "Error", wxOK | wxICON_ERROR, this);
    }
    catch (...) {
        this->income = 0.0;
        this->withdrawals = 0.0;
        this->total = 0.0;
        wxMessageBox(_("An unknown error occurred while opening database"), "Error", wxOK | wxICON_ERROR, this);
    }
    UpdateLabelsMoney();
}
