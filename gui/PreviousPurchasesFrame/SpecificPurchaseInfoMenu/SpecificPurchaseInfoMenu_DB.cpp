#include "gui/PreviousPurchasesFrame/SpecificPurchaseInfoMenu/SpecificPurchaseInfoMenu.hpp"
#include "constants/DB/DB.h"
#include "utils/MathUtils.hpp"

void SpecificPurchaseInfoMenu::LoadPurchaseDetails(unsigned long long id) {
    try {
        sqlite::database db(GetDBPath());

        auto query = db << "SELECT product_name, quantity, price_at_purchase FROM purchase_items WHERE purchase_id = ?;" << id;

        list->DeleteAllItems();

        long index = 0;
        for (auto&& row : query) {
            std::string productName;
            double quantity, price;

            row >> productName >> quantity >> price;

            wxString quantityStr;
            if (std::fmod(quantity, 1.0) == 0.0) quantityStr = wxString::Format("%.0f", quantity);  // sin decimales
            else quantityStr = wxString::Format("%.3f", quantity);  // hasta 3 decimales

            // Insertar una nueva fila en el wxListCtrl
            index = list->InsertItem(index, wxString::FromUTF8(productName));
            list->SetItem(index, 1, quantityStr);
            list->SetItem(index, 2, FormatFloatWithCommas(price));

            ++index;
        }

        // Ajustar las columnas al contenido
        for (int i = 0; i < 3; ++i)
            list->SetColumnWidth(i, wxLIST_AUTOSIZE);

        if (index == 0) {
            wxMessageBox(_("No items were found for this purchase."), _("Info"), wxOK | wxICON_INFORMATION);
        }
    }
    catch (const std::exception& e) {
        wxMessageBox(_("Error loading purchase details:\n") + wxString(e.what()), "Error", wxOK | wxICON_ERROR);
    }
    catch (...) {
        wxMessageBox(_("Unknown error loading purchase details."), "Error", wxOK | wxICON_ERROR);
    }
}


std::string SpecificPurchaseInfoMenu:: GetBarcodeIfExists(sqlite::database& db, unsigned long long productId) {
    std::string barcode = "";
    try {
        db << "SELECT IFNULL(barcode, '') FROM products WHERE id = ?;" << productId >> barcode;
    }
    catch (const std::exception& e) {
        barcode = "";
    }
    return barcode;
}

std::tuple<wxString, double, std::string> SpecificPurchaseInfoMenu::GetWorkerTotalDateTimePurchaseID() {
    std::string workerPurchaseID = "";
    double total = -1.0;
	std::string dateTime = "";

    try {
        sqlite::database db(GetDBPath());
        auto query = db << "SELECT total, worker, date FROM purchases WHERE id = ?;" << m_purchaseId;

        for (auto&& row : query) {
			row >> total >> workerPurchaseID >> dateTime;
        }
    }
    catch (const std::exception& e) {
        // Si falla, igual devolvemos lo que tengamos
    }

    return { wxString::FromUTF8(workerPurchaseID), total, dateTime};
}
