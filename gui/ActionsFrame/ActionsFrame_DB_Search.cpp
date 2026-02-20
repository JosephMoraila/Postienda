#include "gui/ActionsFrame/ActionsFrame.hpp"
#include "constants/DB/DB.h"
#include <wx/msgdlg.h>
#include <wx/filefn.h> 
#include "sqlite_modern_cpp.h"
#include "utils/MathUtils.hpp"
#include "utils/translate/Translate_Utils.hpp"
#include "utils/DateTimeUtils.hpp"
#include <format>

void ActionsFrame::GetActionsBetweenDates(std::string startDateTime, std::string endDateTime) {
	try {
        this->manualAdditions = 0.0;
        this->returnsMoney = 0.0;
        this->manualWithdrawals = 0.0;
        this->total = 0.0;
        this->totalMoneyCash = 0.0;
        this->totalMoneyCard = 0.0;
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

        std::string dateExists = ""; ///Sirve para poner la fecha al inicio de cada día
        wxString purchaseWord = _("Purchase");
        std::string strPurchaseWord = purchaseWord.ToStdString();
        wxString drawerWord = _("Drawer");
        std::string strDrawerWord = drawerWord.ToStdString();
        wxString returnWord = _("Return");
        std::string strReturnWord = returnWord.ToStdString();
                  
        db << query << startDateTime << endDateTime
            >> [&](size_t id, std::string date, double amount, std::string worker, std::string extra_info, std::string type) {

            bool purchaseData = false; ///Servira porque los productos al mostrarse en ticket dejan dos saltos de linea y cuando no pasa por PURCHASE se dejan dos espacios de linea pero si pasa dejar uno al final

            // Extraer solo la fecha (YYYY-MM-DD) del registro
            std::string onlyDate = date.substr(0, 10);

            auto [fechaFormateadaToPrint, horaToPrint] = DateTimeUtils::FormatDateTimeLocalized(date);
            wxString upper = fechaFormateadaToPrint.Upper();
            std::string fechaStrToPrint = std::string(upper.ToUTF8());
            if (fechaStrToPrint != dateExists) {//Por ejemplo en la primera iteracion entra porque por ejemplo el primer producto es jueves; 19 de febrero de 2026 y en otra iteracion donde haya actividad el mismo dia no entra porque son iguales
                stringToPrint += std::format("\n{}:\n\n", fechaStrToPrint); //Agregamos que queremos imprimir solo hasta arriba de cada día la fecha
                dateExists = fechaStrToPrint; //Le ponemos igual para que si otro producto del mismo dia no vuelva a guardar el dia
            }
            
            auto it = fechaToItem.find(onlyDate);
            if (it == fechaToItem.end()) return; // fecha no encontrada

            wxTreeItemId parent = it->second;
            if (!parent.IsOk()) return;

            auto [fechaFormateada, hora] = DateTimeUtils::FormatDateTimeLocalized(date);
            wxString cantidaDinerodWx = FormatFloatWithCommas(amount);
            std::string strCantidadDinero = cantidaDinerodWx.ToStdString();
            std::string horaStr = hora.ToStdString();

            if (type == "PURCHASE") {
                wxString methodPaid = Translate_Utils::GetTranslatedPaymentMethod(extra_info);
                std::string strMethod = methodPaid.ToStdString(); 
                stringToPrint += std::format("{}:ID:{}|{}\n{}|{}|{}:\n", strPurchaseWord, id, strCantidadDinero, horaStr, strMethod, worker);
                if (strMethod == "Cash") this->totalMoneyCash += amount;
                else this->totalMoneyCard += amount;
                wxString str = wxString::Format(_("- %s: ID: %zu | %s | %s | %s | %s"), purchaseWord,id, cantidaDinerodWx, wxString::FromUTF8(worker), methodPaid, hora);
                wxTreeItemId purchaseItemId = treeDays->AppendItem(parent, str);
                purchaseData = true;
                db << "SELECT pi.product_name, pi.quantity, pi.price_at_purchase, p.barcode FROM purchase_items pi LEFT JOIN products p ON pi.product_id = p.id WHERE pi.purchase_id = ?" << id >>
                    [&](std::string productName, double quantity, double price, std::string barcode) {
                    std::variant<size_t, double> variantQuantity = DoubleToVariant(quantity);
                    wxString quantityWx = "";
                    if (std::holds_alternative<size_t>(variantQuantity)) quantityWx = FormatWithCommas(std::get<size_t>(variantQuantity));
                    else quantityWx = FormatFloatWithCommas(std::get<double>(variantQuantity), 3);
                    wxString wxPrice = FormatFloatWithCommas(price);
                    std::string strPrice = wxPrice.ToStdString(); std::string quantityStr = quantityWx.ToStdString();
                    stringToPrint += std::format("  [{}-{}-{}-{}]\n", productName, quantityStr, strPrice, barcode);
                    wxString strCertainProductPurchase = wxString::Format(_("%s | %s | %s | %s"), wxString::FromUTF8(productName), quantityWx, wxPrice, barcode);
                    treeDays->AppendItem(purchaseItemId, strCertainProductPurchase);
                };
            }
            else if (type == "DRAWER") {
                bool isAddition = false;
                db << "SELECT is_addition FROM drawer_history WHERE id = ?" << id >> isAddition;
                if (isAddition) this->manualAdditions += amount;
                else this->manualWithdrawals += amount;
                wxString wxIncomeOrWithdraw = Translate_Utils::GetTranslitionAdditionOrWithdrawal(isAddition);
                std::string strIncomeOrWithdraw = wxIncomeOrWithdraw.ToStdString();
                stringToPrint += std::format("{}:ID:{}|{}\n{}|{}|{}\n{}",strDrawerWord, id, strCantidadDinero, horaStr, strIncomeOrWithdraw, worker, extra_info);
                wxString str = wxString::Format(_("- %s: ID: %zu | %s | %s | %s | %s | %s"), drawerWord,id, cantidaDinerodWx, wxString::FromUTF8(worker), wxIncomeOrWithdraw, wxString::FromUTF8(extra_info), hora);
                treeDays->AppendItem(parent, str);
            }
            else if (type == "RETURN") {
                this->returnsMoney += amount;
                size_t purchaseID = 0;
                db << "SELECT purchase_id FROM returned_products WHERE id = ?" << id >> purchaseID;
                stringToPrint += std::format("{}:ID:{}|{}\n{}|{}:{}:\n{}|{}", strReturnWord, id, strCantidadDinero, horaStr, strPurchaseWord, purchaseID, extra_info, worker);
                wxString str = wxString::Format(_("- %s: ID: %zu | %s | %s | %s ID: %zu | %s | %s"), returnWord,id, cantidaDinerodWx, wxString::FromUTF8(worker), purchaseWord,purchaseID, wxString::FromUTF8(extra_info), hora);
                treeDays->AppendItem(parent, str);
            }
            if (purchaseData) stringToPrint += std::format("\n");
            else stringToPrint += std::format("\n\n");;
        };
        this->total = totalMoneyCash + totalMoneyCard + manualAdditions - manualWithdrawals - returnsMoney;
        wxString totalWX = FormatFloatWithCommas(total); 
        wxString incomeCashWx = FormatFloatWithCommas(totalMoneyCash); 
        wxString withdrawalsWx = FormatFloatWithCommas(manualWithdrawals); 
        wxString manualAdditionWx = FormatFloatWithCommas(manualAdditions);
        wxString returnedWx = FormatFloatWithCommas(returnsMoney);
        wxString incomeCardWx = FormatFloatWithCommas(totalMoneyCard);
        wxString a = wxString::Format(_("Total: %s"), totalWX); 
        wxString b = wxString::Format(_("Income Cash: %s"), incomeCashWx); 
        wxString c = wxString::Format(_("Manual Withdrawals: %s"), withdrawalsWx); 
        wxString d = wxString::Format(_("Income Card: %s"), incomeCardWx);
        wxString e = wxString::Format(_("Manual Income: %s"), manualAdditionWx);
        wxString f = wxString::Format(_("Returned: %s"), returnedWx);
        std::string totalStr = a.ToStdString(); 
        std::string incomeCashStr = b.ToStdString(); 
        std::string withdrawalsStr = c.ToStdString(); 
        std::string incomeCardStr = d.ToStdString();
        std::string manualIncomeStr = e.ToStdString();
        std::string returnedStr = f.ToStdString();
        stringToPrint += std::format("{}\n{}\n{}\n{}\n{}\n{}\n\n", incomeCashStr, incomeCardStr, manualIncomeStr,withdrawalsStr, returnedStr, totalStr);

	}
    catch (const std::exception& e) {
        this->manualAdditions = 0.0;
        this->returnsMoney = 0.0;
        this->manualWithdrawals = 0.0;
        this->total = 0.0;
        this->totalMoneyCash = 0.0;
        this->totalMoneyCard = 0.0;
        wxMessageBox(wxString::Format("Unexpected error: %s", e.what()), "Error", wxOK | wxICON_ERROR, this);
    }
    catch (...) {
        this->manualAdditions = 0.0;
        this->returnsMoney = 0.0;
        this->manualWithdrawals = 0.0;
        this->total = 0.0;
        this->totalMoneyCash = 0.0;
        this->totalMoneyCard = 0.0;
        wxMessageBox(_("An unknown error occurred while opening database"), "Error", wxOK | wxICON_ERROR, this);
    }
    UpdateLabelsMoney();
}
