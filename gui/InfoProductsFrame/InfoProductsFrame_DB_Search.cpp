#include "gui/InfoProductsFrame/InfoProductsFrame.hpp"
#include "constants/DB/DB.h"
#include <wx/msgdlg.h>
#include <wx/filefn.h> 
#include "sqlite_modern_cpp.h"
#include "utils/MathUtils.hpp"
#include <locale>
#include <iomanip>
#include <sstream>
#include "utils/DateTimeUtils.hpp"
#include "utils/ValidateStringInput.h"

void InfoProductsFrame::GetSoldProducts(std::string startDateTime, std::string endDateTime, double minAmount, double maxAmount, std::string productNameBarcode, size_t offset, size_t limit) {
    try {
        wxString dbPath = GetDBPath();
        // Verificar si existe el archivo de base de datos
        if (!wxFileExists(dbPath)) {
            wxMessageBox(_("The database was not found at:\n") + dbPath, _("Database error"), wxOK | wxICON_ERROR, this);
            return;
        }

        sqlite::database db(GetDBPath());
        db << "PRAGMA encoding = 'UTF-8';";

        char tableExists = 0;
        db << "SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name='purchase_items';" >> tableExists;
        if (tableExists == 0) {
            wxMessageBox(_("No purchases found. You have not done any purchase."), _("Table not found"), wxOK | wxICON_ERROR, this);
            return;
        }

        //Contar total de registros
        if (offset == 0) {  // Solo contar en la primera busqueda para dar el total de resultados
            std::string subQuery = "";
            subQuery = R"(
                SELECT pi.product_name
                FROM purchase_items pi
                JOIN purchases pur ON pi.purchase_id = pur.id
                WHERE 1=1
                )";
            subQuery += " AND pur.date BETWEEN ? AND ?";
            if (minAmount != -1.0) subQuery += " AND pi.price_at_purchase >= ?";
            if (maxAmount != -1.0) subQuery += " AND pi.price_at_purchase <= ?";
            if (!productNameBarcode.empty()) {
                if (!EsNumerico(productNameBarcode)) {//Si se busca por nombre
                    subQuery += " AND pi.product_name LIKE ?";
                    subQuery += " GROUP BY pi.product_name";
                }
                else { //Se busca por codigo de barras por lo que se busca directamente en productos su nombre y no en el historial
                    subQuery = R"(
                    SELECT pro.name
                    FROM purchase_items pi
                    JOIN purchases pur ON pi.purchase_id = pur.id
                    JOIN products pro ON pi.product_id = pro.id
                    WHERE 1=1
                    )";
                    subQuery += " AND pur.date BETWEEN ? AND ?";
                    if (minAmount != -1.0) subQuery += " AND pi.price_at_purchase >= ?";
                    if (maxAmount != -1.0) subQuery += " AND pi.price_at_purchase <= ?";
                    subQuery += " AND pro.barcode LIKE ?";
                    subQuery += " GROUP BY pro.name";

                }
            }
            else subQuery += " GROUP BY pi.product_name";
            // Envolver en el COUNT
            std::string countQuery = "SELECT COUNT(*) FROM (" + subQuery + ") AS subquery";
            auto Countstmt = db << countQuery;
            Countstmt << startDateTime << endDateTime;
            if (minAmount != -1.0) Countstmt << minAmount;
            if (maxAmount != -1.0) Countstmt << maxAmount;
            if (!productNameBarcode.empty()) Countstmt << ("%" + productNameBarcode + "%");
            Countstmt >> [&](size_t count) {
                totalRecords = count;
                };
            wxMessageBox(wxString::Format("Total encontrado: %zu", totalRecords));
        }
        list->DeleteAllItems();

    }
    catch (const std::exception& e) {
        wxMessageBox(wxString::Format("Unecpected error: %s", e.what()), "Exception", wxOK | wxICON_ERROR, this);
    }
    catch (...) {
        wxMessageBox(_("An unknown error occurred while processing the purchase."), "Error", wxOK | wxICON_ERROR, this);
    }
}
