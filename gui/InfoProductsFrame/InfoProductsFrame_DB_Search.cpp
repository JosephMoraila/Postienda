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
        }

        list->DeleteAllItems();

        size_t found = 0; //AddToListFromDB modifica su valor sumandole 1 cada vez que se llama
        //Llamar a las columnas para mostrar
        std::string queySelect = "";
        queySelect = R"(
                SELECT pi.product_name, pro.barcode, SUM(pi.quantity) AS cantidad, COALESCE(pro.price, (SELECT pi2.price_at_purchase 
                 FROM purchase_items pi2 
                 JOIN purchases pur2 ON pi2.purchase_id = pur2.id
                 WHERE pi2.product_name = pi.product_name 
                 ORDER BY pur2.id DESC 
                 LIMIT 1)
            ) AS precio
                FROM purchase_items pi
                JOIN purchases pur ON pi.purchase_id = pur.id
                LEFT JOIN products pro ON pi.product_id = pro.id
                WHERE 1=1
                )";
        queySelect += " AND pur.date BETWEEN ? AND ?";
        if (minAmount != -1.0) queySelect += " AND COALESCE(pro.price, pi.price_at_purchase) >= ?";
        if (maxAmount != -1.0) queySelect += " AND COALESCE(pro.price, pi.price_at_purchase) <= ?";
        if (!productNameBarcode.empty()) {
            if (!EsNumerico(productNameBarcode)) {//Si se busca por nombre
                queySelect += " AND pi.product_name LIKE ?";
                queySelect += " GROUP BY pi.product_name";
            }
            else { //Se busca por codigo de barras por lo que se busca directamente en productos su nombre y no en el historial
                queySelect = R"(
                    SELECT pro.name, pro.barcode, SUM(pi.quantity) AS cantidad, COALESCE(pro.price, (SELECT pi2.price_at_purchase 
                     FROM purchase_items pi2 
                     JOIN purchases pur2 ON pi2.purchase_id = pur2.id
                     WHERE pi2.product_name = pi.product_name 
                     ORDER BY pur2.id DESC 
                     LIMIT 1)
                ) AS precio
                    FROM purchase_items pi
                    JOIN purchases pur ON pi.purchase_id = pur.id
                    JOIN products pro ON pi.product_id = pro.id
                    WHERE 1=1
                    )";
                queySelect += " AND pur.date BETWEEN ? AND ?";
                if (minAmount != -1.0) queySelect += " AND COALESCE(pro.price, pi.price_at_purchase) >= ?";
                if (maxAmount != -1.0) queySelect += " AND COALESCE(pro.price, pi.price_at_purchase) <= ?";
                queySelect += " AND pro.barcode LIKE ?";
                queySelect += " GROUP BY pro.name";

            }
        }
        else queySelect += " GROUP BY pi.product_name";
        queySelect += " ORDER BY cantidad DESC LIMIT ? OFFSET ?;";

        sqlite::database_binder selectStmt = db << queySelect;
        selectStmt << startDateTime << endDateTime;
        if (minAmount != -1.0) selectStmt << minAmount;
        if (maxAmount != -1.0) selectStmt << maxAmount;
        if (!productNameBarcode.empty()) selectStmt << ("%" + productNameBarcode + "%");

        selectStmt << (int)limit << (int)offset
            >> [&](std::string name, std::string barcode, double quantity, double precio) {
            AddToListFromDB(name, precio, barcode, quantity, &found);
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
        wxMessageBox(wxString::Format("Unecpected error: %s", e.what()), "Exception", wxOK | wxICON_ERROR, this);
    }
    catch (...) {
        wxMessageBox(_("An unknown error occurred while processing the purchase."), "Error", wxOK | wxICON_ERROR, this);
    }
}


void InfoProductsFrame::AddToListFromDB(std::string& name, double& price, std::string& barcode, double& quantity, size_t* found) {
    if (found) (*found)++;

    long index = list->InsertItem(list->GetItemCount(), wxString::FromUTF8(name));
    list->SetItem(index, 1, FormatFloatWithCommas(price));
    list->SetItem(index, 2, barcode);

    // Verificar si tiene decimales significativos
    if (quantity == static_cast<size_t>(quantity)) {
        // Es un número entero (ej: 100.0)
        list->SetItem(index, 3, FormatWithCommas(static_cast<size_t>(quantity)));
    }
    else {
        // Tiene decimales (ej: 100.5)
        list->SetItem(index, 3, FormatFloatWithCommas(quantity, 3));
    }
}