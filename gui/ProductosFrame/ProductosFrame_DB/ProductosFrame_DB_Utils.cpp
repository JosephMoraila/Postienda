#include "gui/ProductosFrame/ProductosFrame.h"
#include "constants/DB/DB.h"
#include "utils/MathUtils.hpp"


wxTreeItemId ProductosFrame::FindTreeItemByCategoria(const std::shared_ptr<Categoria>& cat) {
    for (const auto& [itemId, catPtr] : treeItemId_Category_Map) {
        if (catPtr->idCategoria == cat->idCategoria) return itemId;
    }
    return wxTreeItemId(); // no encontrado
}

bool ProductosFrame::ExistsCategoryNameInSameLevel(wxString& newCategoryName, size_t& parentCategoryId) {
    try {
        sqlite::database db(GetDBPath());
        db << "PRAGMA encoding = 'UTF-8';";

        int count = 0;
        std::string utf8Name = std::string(newCategoryName.ToUTF8());

        // Buscar directamente en el nivel del parentCategoryId
        db << R"(SELECT COUNT(*) FROM categories WHERE LOWER(name) = LOWER(?) AND parent_id = ?;)"
            << utf8Name
            << parentCategoryId
            >> count;

        return count > 0;
    }
    catch (const std::exception& e) {
        wxString wxError = wxString::FromUTF8(e.what());
        wxString wxFile = wxString::FromUTF8(__FILE__);
        wxString msg = wxString::Format(
            _("Error verifying the existence of a category in the database\n\n""File: %s\nLine: %d\n\n""Error message:\n%s"),wxFile,__LINE__,wxError);
        wxMessageBox(msg, "Error SQLite", wxOK | wxICON_ERROR, this);
        return false;
    }
}


bool ProductosFrame::ExistsProductNameInSameCategory(wxString& newProductName, size_t& categoryId) {
    try {
        sqlite::database db(GetDBPath());
        db << "PRAGMA encoding = 'UTF-8';";
        int count = 0;
        std::string utf8Name = std::string(newProductName.ToUTF8());
        // Buscar directamente en la categorÃÂ­a especificada
        db << R"(SELECT COUNT(*) FROM products WHERE LOWER(name) = LOWER(?) AND category_id = ?;)"
            << utf8Name
            << categoryId
            >> count;
        return count > 0;
    }
    catch (const std::exception& e) {
        wxString wxError = wxString::FromUTF8(e.what());
        wxString wxFile = wxString::FromUTF8(__FILE__);
        wxString msg = wxString::Format(
            _("Error verifying product existence in the database\n\nFile: %s\nLine: %d\n\nError message:\n%s"),
            wxFile,
            __LINE__,
            wxError
        );
        wxMessageBox(msg, "Error SQLite", wxOK | wxICON_ERROR, this);
        return false;
    }
}


bool ProductosFrame::ExistsCodebarGlobal(const std::string& codebar) {
    try {
        sqlite::database db(GetDBPath());
        db << "PRAGMA encoding = 'UTF-8';";
        int count = 0;
        db << R"(SELECT COUNT(*) FROM products WHERE barcode = ?;)" << codebar >> count;
        return count > 0;
    }
    catch (const std::exception& e) {
        wxString wxError = wxString::FromUTF8(e.what());
        wxString wxFile = wxString::FromUTF8(__FILE__);
        wxString msg = wxString::Format(
            _("Error verifying the existence of a barcode in the database\n\nFile: %s\nLine: %d\n\nError message:\n%s"),
            wxFile,
            __LINE__,
            wxError
        );
        wxMessageBox(msg, "Error SQLite", wxOK | wxICON_ERROR, this);
        return false;
    }
}


void ProductosFrame::AdjustStockForProductTypeChange(size_t productId, bool wasByWeight, bool nowByWeight) {
    try {
        sqlite::database db(GetDBPath());

        if (wasByWeight && !nowByWeight) {
            // CambiÃÂ³ de "por peso" a "por unidad": truncar decimales
            // Ejemplo: 12.75 -> 12
            double currentStock = 0.0;
            db << "SELECT quantity FROM stock WHERE product_id = ?;" << productId >> currentStock;

            unsigned long long newStock = static_cast<unsigned long long>(std::floor(currentStock));
            db << "UPDATE stock SET quantity = ? WHERE product_id = ?;" << newStock << productId;
        }
    }
    catch (const sqlite::sqlite_exception& e) {
        wxMessageBox(wxString::Format(_("Error adjusting stock: %s"), e.what()), "Error", wxOK | wxICON_ERROR, this);
    }
}
