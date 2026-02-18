#include "gui/InfoReturnsFrame/InfoReturnsFrame.hpp"
#include "constants/DB/DB.h"
#include <wx/msgdlg.h>
#include <wx/filefn.h> 
#include "sqlite_modern_cpp.h"
#include "utils/MathUtils.hpp"
#include <locale>
#include <iomanip>
#include <sstream>
#include "utils/DateTimeUtils.hpp"

void InfoReturnsFrame::AddToListFromDB(unsigned long long& ide, unsigned long long& purchaseId, double& price, double& quantity, std::string& date, std::string& worker, std::string& productName, size_t* found) {
    if (found) (*found)++;
    auto [fechaFormateada, hora] = DateTimeUtils::FormatDateTimeLocalized(date);
    auto quantityVariant = DoubleToVariant(quantity);

    long index = list->InsertItem(list->GetItemCount(), wxString::Format("%llu", ide));
    list->SetItem(index, 1, fechaFormateada);
    list->SetItem(index, 2, hora);
    list->SetItem(index, 3, wxString::FromUTF8(productName));
    list->SetItem(index, 4, FormatFloatWithCommas(price));
	if (std::holds_alternative<size_t>(quantityVariant)) list->SetItem(index, 5, FormatWithCommas(std::get<size_t>(quantityVariant)));
    else list->SetItem(index, 5, FormatFloatWithCommas(std::get<double>(quantityVariant), 3));
    list->SetItem(index, 6, wxString::FromUTF8(worker));
	list->SetItem(index, 7, wxString::Format("%llu", purchaseId));
}

void InfoReturnsFrame::GetReturnById(unsigned long long& id){
	try {
        wxString dbPath = GetDBPath();
        // Verificar si existe el archivo de base de datos
        if (!wxFileExists(dbPath)) {
            wxMessageBox(_("The database was not found at:\n") + dbPath, _("Database error"), wxOK | wxICON_ERROR, this);
            return;
        }
        totaMonetReturnByFilter = 0.0;
        sqlite::database db(GetDBPath());
        db << "PRAGMA encoding = 'UTF-8';";

        char tableExists = 0;
        db << "SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name='returned_products';" >> tableExists;

        if (tableExists == 0) {
            wxMessageBox(_("No returned products found. You have not done any."), _("Table not found"), wxOK | wxICON_ERROR, this);
            return;
        }
        size_t found = 0;
        list->DeleteAllItems();
        pageLabel->SetLabelText(_("Page: -"));
        db << "SELECT rp.id, rp.purchase_id, rp.price_at_return, rp.quantity, rp.return_date, rp.worker, pi.product_name FROM returned_products rp INNER JOIN purchase_items pi ON rp.purchase_item_id =  pi.id WHERE rp.id = ?" << id >> [&](unsigned long long ide, unsigned long long purchaseId, double price, double quantity, std::string date,std::string worker, std::string productName){
            AddToListFromDB(ide, purchaseId, price, quantity, date, worker, productName, &found);
            totaMonetReturnByFilter = price;
			};
        if (!found) {
            totaMonetReturnByFilter = 0.0;
            wxMessageBox(wxString::Format(_("No returns were found with the ID %llu."), id), _("No results"), wxOK | wxICON_INFORMATION, this);
        }
	}
    catch (const std::exception& e) {
		totaMonetReturnByFilter = 0.0;
        wxMessageBox(wxString::Format("Unexpected error: %s", e.what()), "Exception", wxOK | wxICON_ERROR, this);
    }
    catch (...) {
        totaMonetReturnByFilter = 0.0;
        wxMessageBox(_("An unknown error occurred while processing the purchase."), "Error", wxOK | wxICON_ERROR, this);
    }
    UpdateTotalMoneyReturnLabel();
}


void InfoReturnsFrame::GetReturnsByPurchaseID(unsigned long long& purchaseId){
    try {
        wxString dbPath = GetDBPath();
        // Verificar si existe el archivo de base de datos
        if (!wxFileExists(dbPath)) {
            wxMessageBox(_("The database was not found at:\n") + dbPath, _("Database error"), wxOK | wxICON_ERROR, this);
            return;
        }
        totaMonetReturnByFilter = 0.0;
        sqlite::database db(GetDBPath());
        db << "PRAGMA encoding = 'UTF-8';";

        char tableExists = 0;
        db << "SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name='returned_products';" >> tableExists;

        if (tableExists == 0) {
            wxMessageBox(_("No returned products found. You have not done any."), _("Table not found"), wxOK | wxICON_ERROR, this);
            return;
        }
        size_t found = 0;
        double totalMoneyReturned = 0.0;
        list->DeleteAllItems();
        pageLabel->SetLabelText(_("Page: -"));
        db << "SELECT rp.id, rp.purchase_id, rp.price_at_return, rp.quantity, rp.return_date, rp.worker, pi.product_name FROM returned_products rp INNER JOIN purchase_items pi ON rp.purchase_item_id = pi.id WHERE rp.purchase_id = ?" << purchaseId >> [&](unsigned long long ide, unsigned long long purchaseId, double price, double quantity, std::string date, std::string worker, std::string productName) {
            AddToListFromDB(ide, purchaseId, price, quantity, date, worker, productName, &found);
            totalMoneyReturned += price;
            };
		totaMonetReturnByFilter = totalMoneyReturned;
    }
    catch (const std::exception& e) {
        totaMonetReturnByFilter = 0.0;
        wxMessageBox(wxString::Format("Unexpected error: %s", e.what()), "Exception", wxOK | wxICON_ERROR, this);
    }
    catch (...) {
        totaMonetReturnByFilter = 0.0;
        wxMessageBox(_("An unknown error occurred while processing the purchase."), "Error", wxOK | wxICON_ERROR, this);
    }
    UpdateTotalMoneyReturnLabel();
}


void InfoReturnsFrame::GetReturns(std::string startDateTime, std::string endDateTime, double minAmount, double maxAmount, std::string worker, std::string productNam, double minQuantity, double maxQuantity, size_t offset, size_t limit){
    try {
        wxString dbPath = GetDBPath();
        if (!wxFileExists(dbPath)) {
            wxMessageBox(_("The database was not found at:\n") + dbPath, _("Database error"), wxOK | wxICON_ERROR, this);
            return;
        }

        sqlite::database db(GetDBPath());
        db << "PRAGMA encoding = 'UTF-8';";

        char tableExists = 0;
        db << "SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name='returned_products';" >> tableExists;

        if (tableExists == 0) {
            totaMonetReturnByFilter = 0.0;
            wxMessageBox(_("No returns found. You have not done any."), _("Table not found"), wxOK | wxICON_ERROR, this);
            return;
        }

        if (offset == 0) { //Contar total de registros
            std::string countQuery = "SELECT COUNT(*) FROM returned_products rp INNER JOIN purchase_items pi ON rp.purchase_item_id = pi.id WHERE 1=1";
            countQuery += " AND rp.return_date BETWEEN ? AND ?";
            if (minAmount != -1.0) countQuery += " AND rp.price_at_return >= ?";
            if (maxAmount != -1.0) countQuery += " AND rp.price_at_return <= ?";
			if (minQuantity != -1.0) countQuery += " AND rp.quantity >= ?";
			if (maxQuantity != -1.0) countQuery += " AND rp.quantity <= ?";
			if (!productNam.empty()) countQuery += " AND pi.product_name LIKE ?";
            if (!worker.empty()) countQuery += " AND rp.worker LIKE ?";

            auto countStmt = db << countQuery;
            countStmt << startDateTime << endDateTime;
            if (minAmount != -1.0) countStmt << minAmount;
            if (maxAmount != -1.0) countStmt << maxAmount;
			if (minQuantity != -1.0) countStmt << minQuantity;
			if (maxQuantity != -1.0) countStmt << maxQuantity;
			if (!productNam.empty()) countStmt << ("%" + productNam + "%");
			if (!worker.empty()) countStmt << ("%" + worker + "%");

            countStmt >> [&](size_t count) {
                totalRecords = count;
                };
        }

        totaMonetReturnByFilter = 0.0;
        list->DeleteAllItems();
        size_t found = 0;
        double totalMoneyReturned = 0.0;
		std::string query = "SELECT rp.id, rp.purchase_id, rp.price_at_return, rp.quantity, rp.return_date, rp.worker, pi.product_name FROM returned_products rp INNER JOIN purchase_items pi ON rp.purchase_item_id = pi.id WHERE 1=1";
        query += " AND rp.return_date BETWEEN ? AND ?";
        if (minAmount != -1.0) query += " AND rp.price_at_return >= ?";
        if (maxAmount != -1.0) query += " AND rp.price_at_return <= ?";
        if (minQuantity != -1.0) query += " AND rp.quantity >= ?";
        if (maxQuantity != -1.0) query += " AND rp.quantity <= ?";
        if (!productNam.empty()) query += " AND pi.product_name LIKE ?";
        if (!worker.empty()) query += " AND rp.worker LIKE ?";
        query += " ORDER BY rp.id ASC LIMIT ? OFFSET ?;";

        auto stmt = db << query;
        stmt << startDateTime << endDateTime;
        if (minAmount != -1.0) stmt << minAmount;
        if (maxAmount != -1.0) stmt << maxAmount;
        if (minQuantity != -1.0) stmt << minQuantity;
        if (maxQuantity != -1.0) stmt << maxQuantity;
        if (!productNam.empty()) stmt << ("%" + productNam + "%");
		if (!worker.empty()) stmt << ("%" + worker + "%");
        stmt << (int)limit << (int)offset
            >> [&](unsigned long long ide, unsigned long long purchaseId, double price, double quantity, std::string date, std::string worker, std::string productName) {
            AddToListFromDB(ide, purchaseId, price, quantity, date, worker, productName, &found);
            totalMoneyReturned += price;
            };
        totaMonetReturnByFilter = totalMoneyReturned;

        if (found > 0) {
            // ========== ACTUALIZAR LABEL CON FORMATO X/Y ==========
            size_t currentPage = (currentOffset / PAGE_SIZE) + 1;
            size_t totalPages = (totalRecords + PAGE_SIZE - 1) / PAGE_SIZE;  // Redondeo hacia arriba
            if (totalPages == 0) totalPages = 1;

            pageLabel->SetLabelText(wxString::Format("%zu/%zu", currentPage, totalPages));

            if (currentOffset >= PAGE_SIZE) prevButton->Enable();
            else prevButton->Disable();


            if (found >= PAGE_SIZE) nextButton->Enable();
            else nextButton->Disable();

        }
        else {
            prevButton->Disable();
            nextButton->Disable();

            if (offset == 0) { //Primera nueva busqueda
                pageLabel->SetLabelText("0/0");
                wxMessageBox(_("No returned products were found with the specified filters."), _("No results"), wxOK | wxICON_INFORMATION, this);
                totaMonetReturnByFilter = 0.0;
            }
            else { //Se busca en nueva paginacion
                wxMessageBox(_("No more records available."), _("End of results"), wxOK | wxICON_INFORMATION, this);
                if (currentOffset >= PAGE_SIZE) currentOffset -= PAGE_SIZE;
            }
        }
    }
    catch (const std::exception& e) {
        wxMessageBox(wxString::Format("Error inesperado: %s", e.what()), "Error", wxOK | wxICON_ERROR, this);
        prevButton->Disable();
        nextButton->Disable();
		totaMonetReturnByFilter = 0.0;
    }
    catch (...) {
        wxMessageBox(_("An unknown error occurred while retrieving movements."), "Error", wxOK | wxICON_ERROR, this);
        prevButton->Disable();
        nextButton->Disable();
        totaMonetReturnByFilter = 0.0;
    }
    UpdateTotalMoneyReturnLabel();
}
