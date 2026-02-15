#include "gui/InfoDrawerFrame/InfoDrawerFrame.hpp"
#include "constants/DB/DB.h"
#include <wx/msgdlg.h>
#include <wx/filefn.h> 
#include "sqlite_modern_cpp.h"
#include "utils/MathUtils.hpp"
#include <locale>
#include <iomanip>
#include <sstream>
#include "utils/DateTimeUtils.hpp"

void InfoDrawerFrame::AddToListFromDB(unsigned long long& ide, std::string& date, double& amount, std::string& worker, bool& isAddition, std::string reason, unsigned long long& purchaseId, double& drawerAfter, size_t* found) {
    if (found) (*found)++;
    auto [fechaFormateada, hora] = DateTimeUtils::FormatDateTimeLocalized(date);

    //Translate method
    wxString translatedText;
    if (isAddition) translatedText = _("Income");
    else translatedText = _("Withdrawal");

	//PurchaseId puede ser 0 si es NULL por lo que mejor dejamos vacio el campo en vez de mostrar un 0 que puede confundir al usuario
	wxString PurchaseIdStr;
	if (!purchaseId) PurchaseIdStr = "";
	else PurchaseIdStr = wxString::Format("%llu", purchaseId);

    // Insertar en la tabla
    long index = list->InsertItem(list->GetItemCount(), wxString::Format("%llu", ide));
    list->SetItem(index, 1, fechaFormateada);
    list->SetItem(index, 2, hora);
    list->SetItem(index, 3, FormatFloatWithCommas(amount));
    list->SetItem(index, 4, wxString::FromUTF8(reason));
    list->SetItem(index, 5, wxString::FromUTF8(worker));
    list->SetItem(index, 6, wxString::FromUTF8(translatedText));
	list->SetItem(index, 7, PurchaseIdStr);
	list->SetItem(index, 8, FormatFloatWithCommas(drawerAfter));
}

void InfoDrawerFrame::GetDrawerHistoryById(unsigned long long& id) {
    try {
        wxString dbPath = GetDBPath();
        // Verificar si existe el archivo de base de datos
        if (!wxFileExists(dbPath)) {
            wxMessageBox(_("The database was not found at:\n") + dbPath, _("Database error"), wxOK | wxICON_ERROR, this);
            return;
        }

        totalIncomeByFilter = 0.0;
        totalWithdrawalByFilter = 0.0;
        totalOnDrawerByFilter = 0.0;

        sqlite::database db(GetDBPath());
        db << "PRAGMA encoding = 'UTF-8';";

        char tableExists = 0;
        db << "SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name='drawer_history';" >> tableExists;

        if (tableExists == 0) {
            wxMessageBox(_("No drawer history found. You have not done any."), _("Table not found"), wxOK | wxICON_ERROR, this);
            return;
        }

        size_t found = 0;
        list->DeleteAllItems();
        pageLabel->SetLabelText(_("Page: -"));
        db << "SELECT id, date, amount, worker, is_addition, reason, purchase_id, drawer_after_insertion FROM drawer_history WHERE id = ?;" << id >> [&](unsigned long long ide, std::string date, double amount, std::string worker, bool is_addition, std::string reason, size_t purchase_id, double drawer_after_insertion) {
            AddToListFromDB(ide, date, amount, worker, is_addition, reason, purchase_id, drawer_after_insertion ,&found);
            if (is_addition) totalIncomeByFilter = amount;
            else totalWithdrawalByFilter = amount;
            };
        if (!found) {
            totalIncomeByFilter = 0.0;
            totalWithdrawalByFilter = 0.0;
            totalOnDrawerByFilter = 0.0;
            wxMessageBox(wxString::Format(_("No movements were found with the ID %llu."), id), _("No results"), wxOK | wxICON_INFORMATION, this);
        }

    }
    catch (const std::exception& e) {
        totalIncomeByFilter = 0.0;
        totalWithdrawalByFilter = 0.0;
        totalOnDrawerByFilter = 0.0;
        wxMessageBox(wxString::Format("Unexpected error: %s", e.what()), "Exception", wxOK | wxICON_ERROR, this);
    }
    catch (...) {
        totalIncomeByFilter = 0.0;
        totalWithdrawalByFilter = 0.0;
        totalOnDrawerByFilter = 0.0;
        wxMessageBox(_("An unknown error occurred while processing the purchase."), "Error", wxOK | wxICON_ERROR, this);
    }
    UpdateTotalLabels();
}

void InfoDrawerFrame::GetDrawerHistoryByPurchaseId(unsigned long long& id) {
    try {
        wxString dbPath = GetDBPath();
        // Verificar si existe el archivo de base de datos
        if (!wxFileExists(dbPath)) {
            wxMessageBox(_("The database was not found at:\n") + dbPath, _("Database error"), wxOK | wxICON_ERROR, this);
            return;
        }
        totalIncomeByFilter = 0.0;
        totalWithdrawalByFilter = 0.0;
        totalOnDrawerByFilter = 0.0;

        sqlite::database db(GetDBPath());
        db << "PRAGMA encoding = 'UTF-8';";

        char tableExists = 0;
        db << "SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name='drawer_history';" >> tableExists;

        if (tableExists == 0) {
            wxMessageBox(_("No drawer history found. You have not done any."), _("Table not found"), wxOK | wxICON_ERROR, this);
            return;
        }

        size_t found = 0;
        list->DeleteAllItems();
        pageLabel->SetLabelText(_("Page: -"));
        db << "SELECT id, date, amount, worker, is_addition, reason, purchase_id, drawer_after_insertion FROM drawer_history WHERE purchase_id = ?;" << id >> [&](unsigned long long ide, std::string date, double amount, std::string worker, bool is_addition, std::string reason, size_t purchase_id, double drawer_after_insertion) {
            AddToListFromDB(ide, date, amount, worker, is_addition, reason, purchase_id, drawer_after_insertion, &found);
            totalIncomeByFilter = amount;

            };
        if (!found) {
            totalIncomeByFilter = 0.0;
            totalWithdrawalByFilter = 0.0;
            totalOnDrawerByFilter = 0.0;
            wxMessageBox(wxString::Format(_("No movements were found with the purchase ID %llu."), id), _("No results"), wxOK | wxICON_INFORMATION, this);
        }

    }
    catch (const std::exception& e) {
        totalIncomeByFilter = 0.0;
        totalWithdrawalByFilter = 0.0;
        totalOnDrawerByFilter = 0.0;
        wxMessageBox(wxString::Format("Unexpected error: %s", e.what()), "Exception", wxOK | wxICON_ERROR, this);
    }
    catch (...) {
        totalIncomeByFilter = 0.0;
        totalWithdrawalByFilter = 0.0;
        totalOnDrawerByFilter = 0.0;
        wxMessageBox(_("An unknown error occurred while processing the purchase."), "Error", wxOK | wxICON_ERROR, this);
    }
    UpdateTotalLabels();
}


void InfoDrawerFrame::GetDrawerHistories(std::string startDateTime, std::string endDateTime, double minAmount, double maxAmount, std::string worker, DrawerAction action, std::string reason, double drawerAfterMinAmount, double drawerAfterMaxAmount, size_t offset, size_t limit)
{
    try {
        wxString dbPath = GetDBPath();
        if (!wxFileExists(dbPath)) {
            wxMessageBox(_("The database was not found at:\n") + dbPath, _("Database error"), wxOK | wxICON_ERROR, this);
            return;
        }

        sqlite::database db(GetDBPath());
        db << "PRAGMA encoding = 'UTF-8';";

        char tableExists = 0;
        db << "SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name='drawer_history';" >> tableExists;

        if (tableExists == 0) {
            wxMessageBox(_("No drawer history found. You have not done any."), _("Table not found"), wxOK | wxICON_ERROR, this);
            totalIncomeByFilter = 0.0;
            totalWithdrawalByFilter = 0.0;
            totalOnDrawerByFilter = 0.0;
            return;
        }

        //Contar total de registros
        if (offset == 0) {  // Solo contar en la primera bÃÂÃÂºsqueda
            std::string countQuery = "SELECT COUNT(*) FROM drawer_history WHERE 1=1";
            countQuery += " AND date BETWEEN ? AND ?";
            if (minAmount != -1.0) countQuery += " AND amount >= ?";
            if (maxAmount != -1.0) countQuery += " AND amount <= ?";
            if (!worker.empty()) countQuery += " AND worker LIKE ?";
            if (action != DrawerAction::DRAWER_ANY) {
                if (action == DrawerAction::DRAWER_ADDITION) countQuery += " AND is_addition = 1";
				else if (action == DrawerAction::DRAWER_WITHDRAWL) countQuery += " AND is_addition = 0";
            }
			if (!reason.empty()) countQuery += " AND reason LIKE ?";
			if (drawerAfterMinAmount != -1.0) countQuery += " AND drawer_after_insertion >= ?";
			if (drawerAfterMaxAmount != -1.0) countQuery += " AND drawer_after_insertion <= ?";

            auto countStmt = db << countQuery;
            countStmt << startDateTime << endDateTime;
            if (minAmount != -1.0) countStmt << minAmount;
            if (maxAmount != -1.0) countStmt << maxAmount;
            if (!worker.empty()) countStmt << ("%" + worker + "%");
            // No need of action parameter because it is already added in the query string
			if (!reason.empty()) countStmt << ("%" + reason + "%");
			if (drawerAfterMinAmount != -1.0) countStmt << drawerAfterMinAmount;
			if (drawerAfterMaxAmount != -1.0) countStmt << drawerAfterMaxAmount;

            countStmt >> [&](size_t count) {
                totalRecords = count;
                };

            // Obtener income y withdrawals contando todos los registros con el filtro sin importar paginación
            std::string sumQuery = "SELECT "
                "SUM(CASE WHEN is_addition = 1 THEN amount ELSE 0 END) as total_income, "
                "SUM(CASE WHEN is_addition = 0 THEN amount ELSE 0 END) as total_withdrawals "
                "FROM drawer_history WHERE 1=1";

            sumQuery += " AND date BETWEEN ? AND ?";
            if (minAmount != -1.0) sumQuery += " AND amount >= ?";
            if (maxAmount != -1.0) sumQuery += " AND amount <= ?";
            if (!worker.empty()) sumQuery += " AND worker LIKE ?";
            if (action != DrawerAction::DRAWER_ANY) {
                if (action == DrawerAction::DRAWER_ADDITION) sumQuery += " AND is_addition = 1";
                else if (action == DrawerAction::DRAWER_WITHDRAWL) sumQuery += " AND is_addition = 0";
            }
            if (!reason.empty()) sumQuery += " AND reason LIKE ?";
            if (drawerAfterMinAmount != -1.0) sumQuery += " AND drawer_after_insertion >= ?";
            if (drawerAfterMaxAmount != -1.0) sumQuery += " AND drawer_after_insertion <= ?";

            // Ejecutar query
            auto sumStmt = db << sumQuery;
            sumStmt << startDateTime << endDateTime;
            if (minAmount != -1.0) sumStmt << minAmount;
            if (maxAmount != -1.0) sumStmt << maxAmount;
            if (!worker.empty()) sumStmt << ("%" + worker + "%");
            if (!reason.empty()) sumStmt << ("%" + reason + "%");
            if (drawerAfterMinAmount != -1.0) sumStmt << drawerAfterMinAmount;
            if (drawerAfterMaxAmount != -1.0) sumStmt << drawerAfterMaxAmount;

            double totalIncome = 0.0;
            double totalWithdrawals = 0.0;

            sumStmt >> [&](double income, double withdrawals) {
                totalIncome = income;
                totalWithdrawals = withdrawals;
                };
            totalOnDrawerByFilter = totalIncome - totalWithdrawals;
        }
        totalIncomeByFilter = 0.0;
        totalWithdrawalByFilter = 0.0;
        totalOnDrawerByFilter = 0.0;
        list->DeleteAllItems();

        size_t found = 0; //AddToListFromDB modifica su valor sumandole 1 cada vez que se llama

        // --- Construir consulta dinÃÂÃÂ¡micamente ---
        std::string query = "SELECT id, date, amount, worker, is_addition, reason, purchase_id, drawer_after_insertion FROM drawer_history WHERE 1=1";
        query += " AND date BETWEEN ? AND ?";
        if (minAmount != -1.0) query += " AND amount >= ?";
        if (maxAmount != -1.0) query += " AND amount <= ?";
        if (!worker.empty()) query += " AND worker LIKE ?";
        if (action != DrawerAction::DRAWER_ANY) {
            if (action == DrawerAction::DRAWER_ADDITION) query += " AND is_addition = 1";
            else if (action == DrawerAction::DRAWER_WITHDRAWL) query += " AND is_addition = 0";
        }
        if (!reason.empty()) query += " AND reason LIKE ?";
        if (drawerAfterMinAmount != -1.0) query += " AND drawer_after_insertion >= ?";
        if (drawerAfterMaxAmount != -1.0) query += " AND drawer_after_insertion <= ?";
        query += " ORDER BY id ASC LIMIT ? OFFSET ?;";

        auto stmt = db << query;
        stmt << startDateTime << endDateTime;
        if (minAmount != -1.0) stmt << minAmount;
        if (maxAmount != -1.0) stmt << maxAmount;
        if (!worker.empty()) stmt << ("%" + worker + "%");
        if (!reason.empty()) stmt << ("%" + reason + "%");
        if (drawerAfterMinAmount != -1.0) stmt << drawerAfterMinAmount;
        if (drawerAfterMaxAmount != -1.0) stmt << drawerAfterMaxAmount;

        stmt << (int)limit << (int)offset
            >> [&](unsigned long long ide, std::string date, double amount, std::string worker, bool is_addition, std::string reason, size_t purchase_id, double drawer_after_insertion) {
            AddToListFromDB(ide, date, amount, worker, is_addition, reason, purchase_id, drawer_after_insertion,&found);
            if(is_addition) totalIncomeByFilter += amount;
			else totalWithdrawalByFilter += amount;
            };

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
                wxMessageBox(_("No movements were found with the specified filters."), _("No results"), wxOK | wxICON_INFORMATION, this);
                totalIncomeByFilter = 0.0;
				totalWithdrawalByFilter = 0.0;
                totalOnDrawerByFilter = 0.0;
            }
            else { //Se busca en nueva paginacion
                wxMessageBox(_("No more records available."), _("End of results"), wxOK | wxICON_INFORMATION, this);
                if (currentOffset >= PAGE_SIZE) currentOffset -= PAGE_SIZE;
            }
        }
    }
    catch (const std::exception& e) {
        wxMessageBox(wxString::Format("Error inesperado: %s", e.what()),"Excepción", wxOK | wxICON_ERROR, this);
        prevButton->Disable();
        nextButton->Disable();
        totalIncomeByFilter = 0.0;
        totalWithdrawalByFilter = 0.0;
        totalOnDrawerByFilter = 0.0;
    }
    catch (...) {
        wxMessageBox(_("An unknown error occurred while retrieving movements."),"Error", wxOK | wxICON_ERROR, this);
        prevButton->Disable();
        nextButton->Disable();
        totalIncomeByFilter = 0.0;
        totalWithdrawalByFilter = 0.0;
        totalOnDrawerByFilter = 0.0;
    }
    UpdateTotalLabels();
}