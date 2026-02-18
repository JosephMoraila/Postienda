#include "gui/ReturnsFrame/ReturnsFrame.hpp"
#include "constants/DB/DB.h"
#include "sqlite_modern_cpp.h"
#include "utils/MathUtils.hpp"
#include "db_functions/DB_Functions.hpp"
#include "gui/MainFrame/MainFrame.h"
#include "utils/GetFromFile.h"

void ReturnsFrame::ValidatePurchaseId() {
	unsigned long long purchaseId = GetPurchaseID();
	if (!purchaseId) {
		wxMessageBox(_("Please enter a valid purchase ID."), _("Invalid ID"), wxICON_ERROR | wxOK);
		return;
	}
	bool purchaseIdExists = ValidateIfPurchaseIdExistsTable(purchaseId);
	if (!purchaseIdExists) {
		wxMessageBox(wxString::Format(_("No purchase was found with the ID %llu."), purchaseId), _("Not found"), wxICON_INFORMATION | wxOK);
		return;
	}
	productsPanel->Show();
	this->purchaseID = purchaseId;
	GetProductsByPurchaseId(purchaseId);
	
}

bool ReturnsFrame::ValidateIfPurchaseIdExistsTable(unsigned long long& purchaseId) {
	try {
		wxString dbPath = GetDBPath();
		// Verificar si existe el archivo de base de datos
		if (!wxFileExists(dbPath)) {
			wxMessageBox(_("The database was not found at:\n") + dbPath, _("Database error"), wxOK | wxICON_ERROR, this);
			return false;
		}
		sqlite::database db(GetDBPath());
		db << "PRAGMA encoding = 'UTF-8';";

		char tableExists = 0;
		db << "SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name='purchases';" >> tableExists;
		if (tableExists == 0) return false;

		char purchaseIdExists = 0;
		db << "SELECT COUNT(*) FROM purchases WHERE id = ?;" << purchaseId >> purchaseIdExists;
		if (purchaseIdExists) return true;
		else return false;
	}
	catch (const std::exception& e) {
		wxMessageBox(wxString::Format("Unexpected error: %s", e.what()), "Exception", wxOK | wxICON_ERROR, this);
		return false;
	}
	catch (...) {
		wxMessageBox(_("An unknown error occurred while processing the ID."), "Error", wxOK | wxICON_ERROR, this);
		return false;
	}
}


void ReturnsFrame::GetProductsByPurchaseId(unsigned long long& purchaseId) {
    try {
        sqlite::database db(GetDBPath());
        db << "PRAGMA encoding = 'UTF-8';";

        // Limpiar la tabla antes de cargar nuevos datos
        grid->ClearGrid();
        if (grid->GetNumberRows() > 0) {
            grid->DeleteRows(0, grid->GetNumberRows());
        }

        db << "SELECT purit.id,purit.product_id, purit.product_name,purit. quantity, purit.price_at_purchase, p.barcode FROM purchase_items purit "
			"LEFT JOIN products p ON purit.product_id = p.id "
            "WHERE purit.purchase_id = ?"
            << purchaseId
            >> [&](size_t purchaseItemId,size_t productId, std::string productName, double quantity, double priceAtPurchase, std::string barcode) {
            auto quantityVariant = DoubleToVariant(quantity);
            int row = grid->GetNumberRows();
            grid->AppendRows(1);

            // Columna 0: Product ID (oculta)
            grid->SetCellValue(row, 0, wxString::Format("%zu", productId));

			// Columna 1: Purchase Item ID (oculta)
			grid->SetCellValue(row, 1, wxString::Format("%zu", purchaseItemId));

            // Columna 1: Product name
            grid->SetCellValue(row, 2, wxString::FromUTF8(productName));

            // Columna 2: Quantity
            if (std::holds_alternative<size_t>(quantityVariant)) grid->SetCellValue(row, 3, FormatWithCommas(std::get<size_t>(quantityVariant)));
            else grid->SetCellValue(row, 3, FormatFloatWithCommas(std::get<double>(quantityVariant), 3));

            // Columna 3: Price at purchase
            grid->SetCellValue(row, 4, FormatFloatWithCommas(priceAtPurchase));

            grid->SetCellValue(row, 5, wxString::FromUTF8(barcode));

            // Hacer read-only todas las columnas
            grid->SetReadOnly(row, 0, true);
            grid->SetReadOnly(row, 1, true);
            grid->SetReadOnly(row, 2, true);
            grid->SetReadOnly(row, 3, true);
            grid->SetReadOnly(row, 4, true);
            grid->SetReadOnly(row, 5, true);
            };

        // Mostrar el panel
        productsPanel->Show();

        // Ajustar columnas visibles al contenido
        grid->AutoSizeColumn(2);  // Product name
        grid->AutoSizeColumn(3);  // Quantity
        grid->AutoSizeColumn(4);  // Price
        grid->AutoSizeColumn(5);

        // Actualizar layouts
        mainPanel->Layout();
        Layout();
        Refresh();

    }
    catch (const std::exception& e) {
        wxMessageBox(wxString::Format(_("Error retrieving the data: %s"), e.what()),"Error", wxOK | wxICON_ERROR, this);
    }
}


bool ReturnsFrame::ValidateProductID(unsigned long long& productId){
    try {
        sqlite::database db(GetDBPath());
        db << "PRAGMA encoding = 'UTF-8';";
        char tableExists = 0;
        db << "SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name='products';" >> tableExists;
        if (tableExists == 0) return false;
        char productExists = 0;
        db << "SELECT COUNT(*) FROM products WHERE id = ?;" << productId >> productExists;
        if (productExists) return true;
        else return false;
    }
    catch (const std::exception& e) {
        wxMessageBox(wxString::Format(_("Error retrieving the product: %s"), e.what()), "Error", wxOK | wxICON_ERROR, this);
        return false;
    }
}


void ReturnsFrame::ReturnProductStock(unsigned long long& productId, double& quantity, unsigned long long& purchaseItemId, wxString& productName){
    try {
        sqlite::database db(GetDBPath());
        db << "PRAGMA encoding = 'UTF-8';";
		db << "PRAGMA foreign_keys = ON;";

		double priceAtPurchase = 0.0;
        db << "SELECT price_at_purchase FROM purchase_items WHERE id = ?" << purchaseItemId >> priceAtPurchase;
        wxString currentDate = wxDateTime::Now().FormatISOCombined(' ');
        std::string dateStr = std::string(currentDate.mb_str());
        std::string worker = getUserFromJSON<std::string>();
        bool noWorker = worker == "Ninguno" || worker == "";

        std::string query = "INSERT INTO returned_products (purchase_id, product_id, purchase_item_id, price_at_return, quantity, return_date, worker) ";
        query += "VALUES (?, ?, ?, ?, ?, ?, ";
        if (noWorker) query += " NULL);";
        else query += " ?);";

        auto stmt = db << query;
        stmt << this->purchaseID << productId << purchaseItemId << priceAtPurchase << quantity << dateStr;
        if (!noWorker) stmt << worker;
		db << "UPDATE stock SET quantity = quantity + ? WHERE product_id = ?;" << quantity << productId;

        bool isAdd = false; //No se suma sino que quitamos dinero del cajon para devolver el dinero
        wxString reason = wxString::Format(_("Returned: %s from purchase ID: %llu"), productName, this->purchaseID);
        std::string reasonStr = std::string(reason.utf8_str());
        size_t sz = static_cast<size_t>(this->purchaseID);
        DB_Functions::Drawer_DB_Functions::Insert_Drawer_History(priceAtPurchase, isAdd, reasonStr, &sz);
        mainFrame->setLabelDrawer();
        wxMessageBox(_("The product was successfully returned to stock."), _("Success"), wxOK | wxICON_INFORMATION, this);
    }
    catch (const std::exception& e) {
        wxMessageBox(wxString::Format(_("Error returning to stock: %s"), e.what()), "Error", wxOK | wxICON_ERROR, this);
    }
}

bool ReturnsFrame::ValidateIfProductIsAlreadyReturned(unsigned long long& purchaseItemId) {
    try {
        sqlite::database db(GetDBPath());
        db << "PRAGMA encoding = 'UTF-8';";
        char tableExists = 0;
        db << "SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name='returned_products';" >> tableExists;
        if (tableExists == 0) return true;
        char alreadyReturned = 0;
        db << "SELECT COUNT(*) FROM returned_products WHERE purchase_item_id = ?;" << purchaseItemId >> alreadyReturned;
        if (alreadyReturned) return true;
        else return false;
    }
    catch (const std::exception& e) {
        wxMessageBox(wxString::Format(_("Error checking return status: %s"), e.what()), "Error", wxOK | wxICON_ERROR, this);
        return true;
    }
}


void ReturnsFrame::CreateTableReturns() {
    try {
        sqlite::database db(GetDBPath());
        db << "PRAGMA encoding = 'UTF-8';";
        db << "CREATE TABLE IF NOT EXISTS returned_products ("
              "id INTEGER PRIMARY KEY AUTOINCREMENT,"
              "purchase_id INTEGER NOT NULL,"
              "product_id INTEGER NOT NULL,"
			"purchase_item_id INTEGER NOT NULL,"
            "price_at_return REAL NOT NULL CHECK(price_at_return = ROUND(price_at_return, 2)),"
              "quantity REAL NOT NULL,"
            "worker TEXT,"
              "return_date TEXT NOT NULL,"
              "FOREIGN KEY(purchase_id) REFERENCES purchases(id),"
              "FOREIGN KEY(product_id) REFERENCES products(id)"
			"FOREIGN KEY(purchase_item_id) REFERENCES purchase_items(id)"
              ");";
    }
    catch (const std::exception& e) {
        wxMessageBox(wxString::Format(_("Error creating returns table: %s"), e.what()), "Error", wxOK | wxICON_ERROR, this);
    }
}