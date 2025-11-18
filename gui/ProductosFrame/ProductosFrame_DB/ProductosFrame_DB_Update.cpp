#include "gui/ProductosFrame/ProductosFrame.h"
#include "constants/DB/DB.h"

//Change Category Name:
void ProductosFrame::UpdateCategoryNameInDB(std::string newName, size_t idCategoria, std::string oldName) {
    try {
        sqlite::database db(GetDBPath());
        db << "PRAGMA encoding = 'UTF-8';";
        db << "UPDATE categories SET name = ? WHERE id = ? AND name = ?;"
            << newName << idCategoria << oldName;
    }
    catch (const sqlite::sqlite_exception& e) {
        // Solo mostramos mensaje si es por UNIQUE constraint
        if (std::string(e.what()).find("UNIQUE constraint failed") != std::string::npos) {
            wxMessageBox(_("There is already a category with the same name at the same level."), "Error", wxOK | wxICON_ERROR, this);
            return;
        }
        else {
            // Otros errores opcionalmente tambiÃÂ©n se pueden mostrar
            wxMessageBox(wxString::Format(_("Error inserting category: %s"), e.what()), "Error", wxOK | wxICON_ERROR, this);
            return;
        }
    }
}


//Update Product:
void ProductosFrame::UpdateProductInDB(const Producto& oldProductInfo, const std::shared_ptr<Producto>& newProductInfo, size_t idCategoriaPadre) {
    try {
        sqlite::database db(GetDBPath());
        db << "PRAGMA encoding = 'UTF-8';";

        // Verificar si cambiÃÂ³ el tipo de producto (por peso vs por unidad)
        bool cambioTipoProducto = (oldProductInfo.porPeso != newProductInfo->porPeso);

        //Si codigo de barras es vacÃÂ­o, se pasa NULL
        if (newProductInfo->codigoBarras.empty()) {
            db << "UPDATE products "
                "SET name = ?, price = ?, barcode = NULL, byWeight = ? "
                "WHERE category_id = ? AND name = ? AND price = ? AND byWeight = ? AND id = ?;"
                //SET
                << newProductInfo->nombre << newProductInfo->precio << (newProductInfo->porPeso ? 1 : 0)
                //WHERE
                << idCategoriaPadre << oldProductInfo.nombre << oldProductInfo.precio << (oldProductInfo.porPeso ? 1 : 0) << oldProductInfo.Id;
        }
        else {
            db << "UPDATE products "
                "SET name = ?, price = ?, barcode = ?, byWeight = ? "
                "WHERE category_id = ? AND name = ? AND price = ? AND byWeight = ? AND id = ?;"
                //SET
                << newProductInfo->nombre << newProductInfo->precio << newProductInfo->codigoBarras << (newProductInfo->porPeso ? 1 : 0)
                //where
                << idCategoriaPadre << oldProductInfo.nombre << oldProductInfo.precio << (oldProductInfo.porPeso ? 1 : 0) << oldProductInfo.Id;
        }

        // Si cambiÃÂ³ el tipo de producto, ajustar el stock
        if (cambioTipoProducto) AdjustStockForProductTypeChange(oldProductInfo.Id, oldProductInfo.porPeso, newProductInfo->porPeso);

    }
    catch (const sqlite::sqlite_exception& e) {
        std::string err = e.what();
        if (err.find("products.name, products.category_id") != std::string::npos) {
            wxMessageBox(_("There is already a product with the same name at the same level."), "Error", wxOK | wxICON_ERROR, this);
        }
        else if (err.find("products.barcode") != std::string::npos) {
            wxMessageBox(_("There is already a product with that barcode."), "Error", wxOK | wxICON_ERROR, this);
        }
        else {
            wxMessageBox(wxString::Format(_("Error updating product: %s"), e.what()), "Error", wxOK | wxICON_ERROR, this);
        }
    }
}

//Update Stock In DB:
template<typename T>
void ProductosFrame::UpdateStockInDB(size_t idProducto, T stock) {
    static_assert(std::is_same_v<T, unsigned long long> || std::is_same_v<T, double>, "Stock must be unsigned long long or double");

    try {
        sqlite::database db(GetDBPath());
        db << "UPDATE stock SET quantity = ? WHERE product_id = ?;" << stock << idProducto;
    }
    catch (const sqlite::sqlite_exception& e) {
        wxMessageBox(wxString::Format("Error stock: %s", e.what()), "Error", wxOK | wxICON_ERROR, this);
    }
}
// Instanciaciones explÃÂ­citas
template void ProductosFrame::UpdateStockInDB<unsigned long long>(size_t, unsigned long long);
template void ProductosFrame::UpdateStockInDB<double>(size_t, double);
