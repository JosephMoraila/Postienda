#include "gui/ProductosFrame/ProductosFrame.h"
#include "constants/DB/DB.h"

//Delete Category:
void ProductosFrame::DeleteCategoryFromDB(const std::shared_ptr<Categoria>& categoria) {
    try {
        sqlite::database db(GetDBPath());
        db << "PRAGMA encoding = 'UTF-8';";
        db << "DELETE FROM categories WHERE id = ?;"
            << categoria->idCategoria;
    }
    catch (const std::exception& e) {
        wxString wxError = wxString::FromUTF8(e.what());
        wxString wxFile = wxString::FromUTF8(__FILE__);
        wxString msg = wxString::Format(_("Error deleting category from database\n\nFile: %s\nLine: %d\n\nError message:\n%s"), wxFile, __LINE__, wxError);
        wxMessageBox(msg, "Error SQLite", wxOK | wxICON_ERROR, this);
    }
}

//Delete Product:
void ProductosFrame::DeleteProductFromDB(const Producto& producto, const size_t idCategoriaPadre) {
    try {
        sqlite::database db(GetDBPath());
        db << "PRAGMA encoding = 'UTF-8';";
        db << "DELETE FROM products WHERE category_id = ? AND name = ? AND price = ? AND byWeight = ? AND id = ?;" << idCategoriaPadre << producto.nombre << producto.precio << (producto.porPeso ? 1 : 0) << producto.Id;
    }
    catch (const sqlite::sqlite_exception& e) {
        wxMessageBox(wxString::Format(_("Error obtaining products: %s"), e.what()), "Error", wxOK | wxICON_ERROR, this);
    }
}
