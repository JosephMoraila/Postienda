#include "gui/PreviousPurchasesFrame/PreviousPurchasesFrame.hpp"
#include "constants/DB/DB.h"
#include <wx/msgdlg.h>
#include <wx/filefn.h> 
#include "sqlite_modern_cpp.h"
#include "utils/MathUtils.hpp"
#include <locale>
#include <iomanip>
#include <sstream>
#include "utils/DateTimeUtils.hpp"

void PreviousPurchaseFrame::AddToListFromDB(unsigned long long& ide, std::string& date, double& total, std::string& worker, size_t* found) {
    if (found) (*found)++;
	auto [fechaFormateada, hora] = DateTimeUtils::FormatDateTimeLocalized(date);
    
    // Insertar en la tabla
    long index = list->InsertItem(list->GetItemCount(), wxString::Format("%llu", ide));
    list->SetItem(index, 1, fechaFormateada);
    list->SetItem(index, 2, hora);
    list->SetItem(index, 3, FormatFloatWithCommas(total));
    list->SetItem(index, 4, wxString::FromUTF8(worker));
}

void PreviousPurchaseFrame::GetPurchaseById(unsigned long long& id) {
    try {
        wxString dbPath = GetDBPath();
        // Verificar si existe el archivo de base de datos
        if (!wxFileExists(dbPath)) {
            wxMessageBox(_("The database was not found at:\n") + dbPath, _("Database error"), wxOK | wxICON_ERROR,this);
            return; 
        }

        sqlite::database db(GetDBPath());
        db << "PRAGMA encoding = 'UTF-8';";

        char tableExists = 0;
        db << "SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name='purchases';" >> tableExists;

        if (tableExists == 0) {
            wxMessageBox(_("No purchases found. You have not done any purchase."), _("Table not found"), wxOK | wxICON_ERROR, this);
            return;
        }

        size_t found = 0;
        list->DeleteAllItems();
        pageLabel->SetLabelText(_("Page: -"));
        db << "SELECT id, date, total, worker FROM purchases WHERE id = ?;" << id >> [&](unsigned long long ide, std::string date, double total, std::string worker) {
            AddToListFromDB(ide, date, total, worker, &found);
            totalAppliedFilter = total;
            totalByFilter->SetLabel(wxString::Format("Total: $%s", FormatFloatWithCommas(total)));

        };
        if(!found) wxMessageBox(wxString::Format(_("No purchases were found with the ID %llu."), id), _("No results"), wxOK | wxICON_INFORMATION, this);

    }
    catch (const std::exception& e) {
        wxMessageBox(wxString::Format("Unecpected error: %s", e.what()), "Exception",wxOK | wxICON_ERROR,this);
    }
    catch (...) {
        wxMessageBox(_("An unknown error occurred while processing the purchase."),"Error",wxOK | wxICON_ERROR,this);
    }
}


void PreviousPurchaseFrame::GetPurchases(std::string startDateTime, std::string endDateTime,double minAmount, double maxAmount, std::string worker, size_t offset, size_t limit)
{
    try {
        wxString dbPath = GetDBPath();
        if (!wxFileExists(dbPath)) {
            wxMessageBox(_("The database was not found at:\n") + dbPath,
                _("Database error"), wxOK | wxICON_ERROR, this);
            return;
        }

        sqlite::database db(GetDBPath());
        db << "PRAGMA encoding = 'UTF-8';";

        char tableExists = 0;
        db << "SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name='purchases';" >> tableExists;

        if (tableExists == 0) {
            wxMessageBox(_("No purchases found. You have not done any purchase."), _("Table not found"), wxOK | wxICON_ERROR, this);
            return;
        }

        //Contar total de registros
        if (offset == 0) {  // Solo contar en la primera bÃÂÃÂºsqueda
            std::string countQuery = "SELECT COUNT(*) FROM purchases WHERE 1=1";
            countQuery += " AND date BETWEEN ? AND ?";
            if (minAmount != -1.0) countQuery += " AND total >= ?";
            if (maxAmount != -1.0) countQuery += " AND total <= ?";
            if (!worker.empty()) countQuery += " AND worker LIKE ?";

            auto countStmt = db << countQuery;
            countStmt << startDateTime << endDateTime;
            if (minAmount != -1.0) countStmt << minAmount;
            if (maxAmount != -1.0) countStmt << maxAmount;
            if (!worker.empty()) countStmt << ("%" + worker + "%");

            countStmt >> [&](size_t count) {
                totalRecords = count;
            };
        }
        list->DeleteAllItems();

        size_t found = 0; //AddToListFromDB modifica su valor sumandole 1 cada vez que se llama

        // --- Construir consulta dinÃÂÃÂ¡micamente ---
        std::string query = "SELECT id, date, total, worker FROM purchases WHERE 1=1";
        query += " AND date BETWEEN ? AND ?";
        if (minAmount != -1.0) query += " AND total >= ?";
        if (maxAmount != -1.0) query += " AND total <= ?";
        if (!worker.empty()) query += " AND worker LIKE ?";
        query += " ORDER BY id ASC LIMIT ? OFFSET ?;";

        auto stmt = db << query;
        stmt << startDateTime << endDateTime;
        if (minAmount != -1.0) stmt << minAmount;
        if (maxAmount != -1.0) stmt << maxAmount;
        if (!worker.empty()) stmt << ("%" + worker + "%");

        stmt << (int)limit << (int)offset
            >> [&](unsigned long long ide, std::string date, double total, std::string workerName) {
            AddToListFromDB(ide, date, total, workerName, &found);
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
                wxMessageBox(_("No purchases were found with the specified filters."), _("No results"), wxOK | wxICON_INFORMATION, this);
            }
            else { //Se busca en nueva paginacion
                wxMessageBox(_("No more records available."), _("End of results"), wxOK | wxICON_INFORMATION, this);
                if (currentOffset >= PAGE_SIZE) currentOffset -= PAGE_SIZE;
            }
        }
    }
    catch (const std::exception& e) {
        wxMessageBox(wxString::Format("Error inesperado: %s", e.what()),
            "Excepción", wxOK | wxICON_ERROR, this);
        prevButton->Disable();
        nextButton->Disable();
    }
    catch (...) {
        wxMessageBox(_("An unknown error occurred while retrieving purchases."),
            "Error", wxOK | wxICON_ERROR, this);
        prevButton->Disable();
        nextButton->Disable();
    }
}

void PreviousPurchaseFrame::GetTotalByFilter(std::string startDateTime, std::string endDateTime, double minAmount, double maxAmount, std::string worker){
    sqlite::database db(GetDBPath());
    db << "PRAGMA encoding = 'UTF-8';";
    std::string countQuery = "SELECT IFNULL(SUM(total), 0) FROM purchases WHERE 1=1";
    countQuery += " AND date BETWEEN ? AND ?";
    if (minAmount != -1.0) countQuery += " AND total >= ?";
    if (maxAmount != -1.0) countQuery += " AND total <= ?";
    if (!worker.empty()) countQuery += " AND worker LIKE ?";

    auto countStmt = db << countQuery;
    countStmt << startDateTime << endDateTime;
    if (minAmount != -1.0) countStmt << minAmount;
    if (maxAmount != -1.0) countStmt << maxAmount;
    if (!worker.empty()) countStmt << ("%" + worker + "%");

    countStmt >> [&](double totalSold) {
        totalAppliedFilter = totalSold;
        };
    totalByFilter->SetLabel(wxString::Format("Total: $%s", FormatFloatWithCommas(totalAppliedFilter)));
}
