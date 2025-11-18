#include "gui/ProductosFrame/ProductosFrame.h"
#include "constants/DB/DB.h"

//Insert Category:
size_t ProductosFrame::InsertCategoryToDB(const std::string& nombre, size_t& parentId) {
    try {
        sqlite::database db(GetDBPath());
        db << "PRAGMA encoding = 'UTF-8';";

        // El nombre ya viene en UTF-8, no necesita conversiÃÂ³n adicional
        db << "INSERT INTO categories (name, parent_id) VALUES (?, ?);"
            << nombre << parentId;

        // Obtener el ID generado automÃÂ¡ticamente
        size_t newCategoryId = db.last_insert_rowid();
        return newCategoryId;
    }
    catch (const std::exception& e) {
        wxString wxError = wxString::FromUTF8(e.what());
        wxString wxFile = wxString::FromUTF8(__FILE__);
        wxString msg = wxString::Format(_("Error inserting new category into database\n\n""File: %s\nLine: %d\n\n" "Error message:\n%s"), wxFile, __LINE__, wxError);
        wxMessageBox(msg, "Error SQLite", wxOK | wxICON_ERROR, this);
        return 0; // Indicar error con ID 0
    }
}

//Insert Product:
void ProductosFrame::InsertProductToDB(Producto& product, size_t& categoryId) {
    try {
        sqlite::database db(GetDBPath());
        db << "PRAGMA encoding = 'UTF-8';";

        // Si el cÃÂ³digo de barras estÃÂ¡ vacÃÂ­o, insertamos NULL
        if (product.codigoBarras.empty()) {
            db << R"(
                INSERT INTO products (name, price, barcode, byWeight, category_id)
                VALUES (?, ?, NULL, ?, ?);
            )"
                << product.nombre
                << product.precio
                << static_cast<unsigned char>(product.porPeso)
                << categoryId;
        }
        else {
            db << R"(
                INSERT INTO products (name, price, barcode, byWeight, category_id)
                VALUES (?, ?, ?, ?, ?);
            )"
                << product.nombre
                << product.precio
                << product.codigoBarras
                << static_cast<unsigned char>(product.porPeso)
                << categoryId;
        }

        // Obtener el ID generado automÃÂ¡ticamente
        product.Id = db.last_insert_rowid();
        InsertStockProductToDB(product.Id);
    }
    catch (const std::exception& e) {
        wxString wxError = wxString::FromUTF8(e.what());
        wxString wxFile = wxString::FromUTF8(__FILE__);
        wxString msg = wxString::Format(_("Error inserting new product into database\n\n""File: %s\nLine: %d\n\n""Error message:\n%s"), wxFile, __LINE__, wxError);
        wxMessageBox(msg, "Error SQLite", wxOK | wxICON_ERROR, this);
    }
}


//Insert Stock Product:
void ProductosFrame::InsertStockProductToDB(size_t productId) {
    try {
        sqlite::database db(GetDBPath());
        db << "PRAGMA encoding = 'UTF-8';";
        db << R"(
            INSERT INTO stock (product_id, quantity)
            VALUES (?, 0.000);
        )"
            << productId;
    }
    catch (const std::exception& e) {
        wxString wxError = wxString::FromUTF8(e.what());
        wxString wxFile = wxString::FromUTF8(__FILE__);
        wxString msg = wxString::Format(_("Error inserting stock for the product into the database\n\n""File: %s\nLine: %d\n\n""Error message:\n%s"), wxFile, __LINE__, wxError);
        wxMessageBox(msg, "Error SQLite", wxOK | wxICON_ERROR, this);
    }
}
