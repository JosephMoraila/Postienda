#include "gui/ProductosFrame/ProductosFrame.h"
#include "constants/DB/DB.h"
#include "constants/MESSAGES_ADVICE/ERROR/DB/DB_ERRORS.hpp"
using namespace DB_ERROR_MESSAGES;

void ProductosFrame::CrearConexionBD() {
    try {
        sqlite::database db(GetDBPath());

        // Configurar SQLite para UTF-8
        db << "PRAGMA foreign_keys = ON;";
        db << "PRAGMA encoding = 'UTF-8';";

        // --- Crear tablas normales ---
        db << "CREATE TABLE IF NOT EXISTS categories ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "name TEXT NOT NULL COLLATE NOCASE,"
            "parent_id INTEGER,"
            "FOREIGN KEY(parent_id) REFERENCES categories(id) ON DELETE CASCADE,"
            "UNIQUE(name, parent_id)"
            ");";

        db << "CREATE TABLE IF NOT EXISTS products ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "name TEXT NOT NULL COLLATE NOCASE,"
            "price REAL NOT NULL DEFAULT 0.00 CHECK(price = ROUND(price, 2)),"
            "barcode TEXT UNIQUE,"
            "byWeight INTEGER NOT NULL DEFAULT 0,"
            "category_id INTEGER,"
            "FOREIGN KEY(category_id) REFERENCES categories(id) ON DELETE CASCADE,"
            "UNIQUE(name, category_id)"
            ");";

        // --- Crear tabla de stock que tendrÃÂÃÂ¡ la cantidad de cada producto ---
        db << "CREATE TABLE IF NOT EXISTS stock ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "product_id INTEGER NOT NULL UNIQUE,"   // RelaciÃÂÃÂ³n 1:1 con products
            "quantity REAL NOT NULL DEFAULT 0.000 CHECK(quantity = ROUND(quantity, 3)),"
            "FOREIGN KEY(product_id) REFERENCES products(id) ON DELETE CASCADE"
            ");";

        //Trigger para eliminar productos al eliminar una categorÃÂÃÂ­a
        db << "CREATE TRIGGER IF NOT EXISTS delete_products_on_category_delete "
            "AFTER DELETE ON categories "
            "FOR EACH ROW "
            "BEGIN "
            "DELETE FROM products WHERE category_id = OLD.id; "
            "END;";

        //Trigger para eliminar categorias hijas al eliminar una categorÃÂÃÂ­a padre
        db << "CREATE TRIGGER IF NOT EXISTS delete_child_categories "
            "AFTER DELETE ON categories "
            "FOR EACH ROW "
            "BEGIN "
            "DELETE FROM categories WHERE parent_id = OLD.id; "
            "END;";

        // --- Crear el TRIGGER para eliminar stock asociado ---
        db << "CREATE TRIGGER IF NOT EXISTS delete_product_stock "
            "AFTER DELETE ON products "
            "FOR EACH ROW "
            "BEGIN "
            "DELETE FROM stock WHERE product_id = OLD.id; "
            "END;";


        db << "PRAGMA foreign_keys = ON;";

        // --- Asegurar categorÃÂÃÂ­a raÃÂÃÂ­z "Productos" en permanentes ya que siempre serÃÂÃÂ¡ 1---
        int root_count = 0;
        db << "SELECT COUNT(*) FROM categories WHERE id = 1;" >> root_count;
        if (root_count == 0) db << "INSERT INTO categories (name, parent_id) VALUES ('Products', NULL);";


    }
    catch (const std::exception& e) {
        wxString wxError = wxString::FromUTF8(e.what());
        wxString wxFile = wxString::FromUTF8(__FILE__);

        wxString msg = wxString::Format(
            DB_ERROR_LINE_MESSAGE,
            wxFile,
            __LINE__,
            wxError
        );

        wxMessageBox(msg, "Error SQLite", wxOK | wxICON_ERROR, this);
    }

}

void ProductosFrame::LoadCategoriesFromDB() {
    try {
        sqlite::database db(GetDBPath());
        db << "PRAGMA encoding = 'UTF-8';";

        auto query = db << "SELECT id, name, parent_id FROM categories;";

        // Crear raÃÂÃÂ­z si no existe
        if (!arbolCategorias->GetRootItem().IsOk()) {
            wxTreeItemId idRoot = arbolCategorias->AddRoot(_("Products"));
            auto rootCat = std::make_shared<Categoria>(Categoria{ 1, "Products", {}, {} });
            treeItemId_Category_Map[idRoot] = rootCat;
        }

        // Map auxiliar: id -> categorÃÂÃÂ­a (para poder buscar el padre)
        std::unordered_map<size_t, std::shared_ptr<Categoria>> idToCategoria;
        idToCategoria[1] = treeItemId_Category_Map[arbolCategorias->GetRootItem()];

        for (auto&& row : query) {
            size_t id;
            std::string name;
            std::optional<size_t> parent_id;
            row >> id >> name >> parent_id;

            // Omitir la raÃÂÃÂ­z (Productos)
            if (id == 1) continue;

            auto categoria = std::make_shared<Categoria>();
            categoria->idCategoria = id;
            categoria->nombre = name;

            wxTreeItemId parentItemId;

            //Todas las catregorÃÂÃÂ­as menos la raÃÂÃÂ­z deben tener padre
            if (parent_id.has_value()) {
                // Buscar padre en los ya registrados
                auto itParent = idToCategoria.find(parent_id.value());
                if (itParent != idToCategoria.end()) {
                    // El padre existe en el mapa
                    parentItemId = FindTreeItemByCategoria(itParent->second);
                    if (!parentItemId.IsOk()) parentItemId = arbolCategorias->GetRootItem();

                    // === Enlazar en el modelo (padre->subcategorias) ===
                    itParent->second->subcategorias.push_back(categoria);
                }
                else parentItemId = arbolCategorias->GetRootItem(); // Padre no encontrado, lo cuelgo del root como fallback
            }
            else {
                // Si no tiene padre, va al root
                parentItemId = arbolCategorias->GetRootItem();
            }

            // Agregar al ÃÂÃÂ¡rbol - CONVERTIR DE UTF-8 A wxString
            wxString nombreWx = wxString::FromUTF8(categoria->nombre.c_str());
            wxTreeItemId newItemId = arbolCategorias->AppendItem(parentItemId, nombreWx);
            treeItemId_Category_Map[newItemId] = categoria;

            // Guardar la categorÃÂÃÂ­a para futuras referencias
            idToCategoria[id] = categoria;
        }

        arbolCategorias->ExpandAll();
    }
    catch (const std::exception& e) {
        wxString wxError = wxString::FromUTF8(e.what());
        wxString wxFile = wxString::FromUTF8(__FILE__);
        wxString msg = wxString::Format(
            DB_ERROR_LINE_MESSAGE,
            wxFile,
            __LINE__,
            wxError
        );
        wxMessageBox(msg, "Error SQLite", wxOK | wxICON_ERROR, this);
    }
}

void ProductosFrame::LoadProductsFromDB() {
    try {
        sqlite::database db(GetDBPath());
        db << "PRAGMA encoding = 'UTF-8';";

        auto query = db << "SELECT id, name, price, barcode, byWeight, category_id FROM products;";

        for (auto&& row : query) {
            size_t id;
            std::string name;
            double price;
            std::optional<std::string> barcode;
            unsigned char byWeightChar;
            size_t category_id; // ÃÂ¢ÃÂÃÂ Ya no es optional, debe existir siempre

            row >> id >> name >> price >> barcode >> byWeightChar >> category_id;

            auto product = std::make_shared<Producto>();
            product->Id = id;
            product->nombre = name;
            product->precio = price;
            product->codigoBarras = barcode.value_or("");
            product->porPeso = (byWeightChar != 0);

            // Buscar el ÃÂÃÂ­tem del ÃÂÃÂ¡rbol correspondiente a la categorÃÂÃÂ­a
            wxTreeItemId parentItemId;
            for (const auto& [itemId, catPtr] : treeItemId_Category_Map) {
                if (catPtr->idCategoria == category_id) {
                    parentItemId = itemId;
                    break;
                }
            }

            // Si no se encontrÃÂÃÂ³ la categorÃÂÃÂ­a (por algÃÂÃÂºn error), colgar del root (id=1)
            if (!parentItemId.IsOk()) {
                for (const auto& [itemId, catPtr] : treeItemId_Category_Map) {
                    if (catPtr->idCategoria == 1) { // categorÃÂÃÂ­a raÃÂÃÂ­z "Productos"
                        parentItemId = itemId;
                        break;
                    }
                }
            }

            // Agregar al ÃÂÃÂ¡rbol visual
            wxString nombreWx = wxString::Format("- %s (%s)", wxString::FromUTF8(product->nombre.c_str()), FormatFloatWithCommas(product->precio));
            if (!product->codigoBarras.empty()) nombreWx += wxString::Format(" [%s]", wxString::FromUTF8(product->codigoBarras.c_str()));
            wxTreeItemId newItemId = arbolCategorias->AppendItem(parentItemId, nombreWx);

            treeItemId_Product_Map[newItemId] = product;

            // Enlazar producto a la categorÃÂÃÂ­a
            auto catIt = treeItemId_Category_Map.find(parentItemId);
            if (catIt != treeItemId_Category_Map.end()) catIt->second->productos.push_back(product);
        }
    }
    catch (const std::exception& e) {
        wxString wxError = wxString::FromUTF8(e.what());
        wxString wxFile = wxString::FromUTF8(__FILE__);
        wxString msg = wxString::Format(
            DB_ERROR_LINE_MESSAGE,
            wxFile,
            __LINE__,
            wxError
        );
        wxMessageBox(msg, "Error SQLite", wxOK | wxICON_ERROR, this);
    }
}


//Get Stock From DB:
std::variant<unsigned long long, double> ProductosFrame::GetStockFromDB(size_t idProducto, bool& byWeight) {
    try {
        sqlite::database db(GetDBPath());

        if (byWeight) {
            double stock = 0;
            db << "SELECT quantity FROM stock WHERE product_id = ?;" << idProducto >> stock;
            return stock; // retorna double
        }
        else {
            unsigned long long stock = 0;
            db << "SELECT quantity FROM stock WHERE product_id = ?;" << idProducto >> stock;
            return stock; // retorna unsigned long long
        }
    }
    catch (const sqlite::sqlite_exception& e) {
        wxMessageBox(wxString::Format(_("Error getting stock: %s"), e.what()), "Error", wxOK | wxICON_ERROR, this);
        return 0ull; //0ull significa que es unsigned long long
    }
}
