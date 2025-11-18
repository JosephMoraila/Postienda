#include "gui/ProductosFrame/ProductosFrame.h"
#include "constants/DB/DB.h"
#include "utils/MathUtils.hpp"


//Search Categories:
void ProductosFrame::SearchCategories(wxString termino, std::vector<wxTreeItemId>& itemCategorias) {
    std::string terminoUtf8 = std::string(termino.ToUTF8());
    try {
        sqlite::database db(GetDBPath());
        db << "PRAGMA encoding = 'UTF-8';";

        db << "SELECT id FROM categories WHERE name LIKE ?;"
            << ("%" + terminoUtf8 + "%")
            >> [&](int id) {
            for (auto& parItemCategory : treeItemId_Category_Map) {
                std::shared_ptr <Categoria> category = parItemCategory.second;
                if (category->idCategoria == id) itemCategorias.push_back(parItemCategory.first);
            }
            };

    }
    catch (const sqlite::sqlite_exception& e) {
        wxMessageBox(wxString::Format("Error SQLite: %s", e.what()), "Error", wxOK | wxICON_ERROR);
    }
    catch (const std::exception& e) {
        wxMessageBox(wxString::Format("Standart error: %s", e.what()), "Error", wxOK | wxICON_ERROR);
    }
}



//Search products:
void ProductosFrame::SearchProducts(wxString termino, std::vector<wxTreeItemId>& itemProductos) {
    std::string terminoUtf8 = std::string(termino.ToUTF8());
    try {
        sqlite::database db(GetDBPath());
        db << "PRAGMA encoding = 'UTF-8';";

        db << "SELECT id FROM products WHERE name LIKE ?;"
            << ("%" + terminoUtf8 + "%")
            >> [&](int id) {
            for (auto& parItemProducto : treeItemId_Product_Map) {
                std::shared_ptr <Producto> Product = parItemProducto.second;
                if (Product->Id == id) itemProductos.push_back(parItemProducto.first);
            }
            };

    }
    catch (const sqlite::sqlite_exception& e) {
        wxMessageBox(wxString::Format("Error SQLite: %s", e.what()), "Error", wxOK | wxICON_ERROR);
    }
    catch (const std::exception& e) {
        wxMessageBox(wxString::Format("Standart error: %s", e.what()), "Error", wxOK | wxICON_ERROR);
    }
}



// FunciÃÂ³n auxiliar para agregar producto al ÃÂ¡rbol filtrado
void ProductosFrame::AddProductToFilteredTree(const std::shared_ptr<Producto>& product, size_t category_id) {
    // Buscar el item de la categorÃÂ­a padre
    wxTreeItemId parentItemId;
    for (const auto& [itemId, catPtr] : treeItemId_Category_Map) {
        if (catPtr->idCategoria == category_id) {
            parentItemId = itemId;
            break;
        }
    }

    if (!parentItemId.IsOk()) return;

    wxString nombreWx = wxString::Format("- %s (%s)", wxString::FromUTF8(product->nombre.c_str()), FormatFloatWithCommas(product->precio));
    if (!product->codigoBarras.empty()) nombreWx += wxString::Format(" [%s]", wxString::FromUTF8(product->codigoBarras.c_str()));
    

    wxTreeItemId newItemId = arbolCategorias->AppendItem(parentItemId, nombreWx);
    treeItemId_Product_Map[newItemId] = product;

    // Resaltar producto encontrado
    arbolCategorias->SetItemBold(newItemId, true);
    arbolCategorias->SetItemTextColour(newItemId, *wxRED);
    arbolCategorias->SetItemBackgroundColour(newItemId, wxColour(255, 255, 180));

    // Enlazar a categorÃÂ­a
    auto catIt = treeItemId_Category_Map.find(parentItemId);
    if (catIt != treeItemId_Category_Map.end()) catIt->second->productos.push_back(product);
}


void ProductosFrame::LoadFilteredTreeFromDB(const wxString& searchTerm) {
    try {
        sqlite::database db(GetDBPath());
        db << "PRAGMA encoding = 'UTF-8';";

        std::string terminoUtf8 = std::string(searchTerm.ToUTF8());
        std::string searchPattern = "%" + terminoUtf8 + "%";

        // Conjunto para almacenar IDs de categorÃÂ­as que necesitamos mostrar
        std::set<size_t> categoryIdsToShow;
        std::set<size_t> productIdsToShow;

        // Buscar categorÃÂ­as que coinciden
        db << "SELECT id FROM categories WHERE name LIKE ?;"
            << searchPattern
            >> [&](size_t id) {
            categoryIdsToShow.insert(id);
            };

        // Buscar productos que coinciden por nombre, cÃÂ³digo de barras o precio
        // Buscar por nombre
        db << "SELECT id, category_id FROM products WHERE name LIKE ?;"
            << searchPattern
            >> [&](size_t productId, size_t categoryId) {
            productIdsToShow.insert(productId);
            categoryIdsToShow.insert(categoryId);
            };

        // Buscar por cÃÂ³digo de barras
        db << "SELECT id, category_id FROM products WHERE barcode LIKE ?;"
            << searchPattern
            >> [&](size_t productId, size_t categoryId) {
            productIdsToShow.insert(productId);
            categoryIdsToShow.insert(categoryId);
            };

        // Buscar por precio (si el tÃÂ©rmino es un nÃÂºmero)
        try {
            // Intentar convertir el tÃÂ©rmino a nÃÂºmero para buscar por precio
            std::string precioStr = terminoUtf8;
            // Eliminar comas si las hay
            precioStr.erase(std::remove(precioStr.begin(), precioStr.end(), ','), precioStr.end());

            if (!precioStr.empty() && std::all_of(precioStr.begin(), precioStr.end(),
                [](char c) { return std::isdigit(c) || c == '.'; })) {
                double precio = std::stod(precioStr);

                // Buscar productos con precio exacto o similar (ÃÂ±0.01)
                db << "SELECT id, category_id FROM products WHERE ABS(price - ?) < 0.01;"
                    << precio
                    >> [&](size_t productId, size_t categoryId) {
                    productIdsToShow.insert(productId);
                    categoryIdsToShow.insert(categoryId);
                    };
            }
        }
        catch (...) {
            // Si no se puede convertir a nÃÂºmero, simplemente ignorar la bÃÂºsqueda por precio
        }

        // Agregar todas las categorÃÂ­as ancestras necesarias
        std::set<size_t> allCategoryIds = categoryIdsToShow;
        for (size_t catId : categoryIdsToShow) {
            // Subir hasta la raÃÂ­z
            size_t currentId = catId;
            while (currentId != 1) { // 1 es el ID de la raÃÂ­z
                std::optional<size_t> parentId;
                db << "SELECT parent_id FROM categories WHERE id = ?;"
                    << currentId
                    >> parentId;

                if (parentId.has_value()) {
                    allCategoryIds.insert(parentId.value());
                    currentId = parentId.value();
                }
                else {
                    break;
                }
            }
        }

        // Crear la raÃÂ­z
        wxTreeItemId rootId = arbolCategorias->AddRoot(_("Products"));
        auto rootCat = std::make_shared<Categoria>(Categoria{ 1, "Products", {}, {} });
        treeItemId_Category_Map[rootId] = rootCat;

        // Cargar categorÃÂ­as en orden jerÃÂ¡rquico
        std::unordered_map<size_t, std::shared_ptr<Categoria>> idToCategoria;
        idToCategoria[1] = rootCat;

        auto queryCategories = db << "SELECT id, name, parent_id FROM categories ORDER BY parent_id, id;";

        for (auto&& row : queryCategories) {
            size_t id;
            std::string name;
            std::optional<size_t> parent_id;
            row >> id >> name >> parent_id;

            // Solo procesar si esta categorÃÂ­a debe mostrarse
            if (allCategoryIds.find(id) == allCategoryIds.end()) continue;

            if (id == 1) continue; // Saltar raÃÂ­z

            auto categoria = std::make_shared<Categoria>();
            categoria->idCategoria = id;
            categoria->nombre = name;

            wxTreeItemId parentItemId;

            if (parent_id.has_value()) {
                auto itParent = idToCategoria.find(parent_id.value());
                if (itParent != idToCategoria.end()) {
                    parentItemId = FindTreeItemByCategoria(itParent->second);
                    if (!parentItemId.IsOk()) parentItemId = rootId;
                    itParent->second->subcategorias.push_back(categoria);
                }
                else parentItemId = rootId;
            }
            else parentItemId = rootId;

            wxString nombreWx = wxString::FromUTF8(categoria->nombre.c_str());
            wxTreeItemId newItemId = arbolCategorias->AppendItem(parentItemId, nombreWx);
            treeItemId_Category_Map[newItemId] = categoria;
            idToCategoria[id] = categoria;

            // Resaltar si coincide con la bÃÂºsqueda
            if (categoryIdsToShow.find(id) != categoryIdsToShow.end() && id != 1) {
                arbolCategorias->SetItemBold(newItemId, true);
                arbolCategorias->SetItemTextColour(newItemId, *wxRED);
                arbolCategorias->SetItemBackgroundColour(newItemId, wxColour(255, 255, 180));
            }
        }

        // Cargar productos que coinciden (por nombre, cÃÂ³digo de barras o precio)
        std::set<size_t> loadedProductIds;

        // Cargar por nombre
        auto queryProductsByName = db << "SELECT id, name, price, barcode, byWeight, category_id FROM products WHERE name LIKE ?;" << searchPattern;

        for (auto&& row : queryProductsByName) {
            size_t id;
            std::string name;
            double price;
            std::optional<std::string> barcode;
            unsigned char byWeightChar;
            size_t category_id;

            row >> id >> name >> price >> barcode >> byWeightChar >> category_id;

            if (loadedProductIds.find(id) != loadedProductIds.end()) continue;
            loadedProductIds.insert(id);

            // Solo mostrar si la categorÃÂ­a padre estÃÂ¡ visible
            if (allCategoryIds.find(category_id) == allCategoryIds.end()) continue;

            auto product = std::make_shared<Producto>();
            product->Id = id;
            product->nombre = name;
            product->precio = price;
            product->codigoBarras = barcode.value_or("");
            product->porPeso = (byWeightChar != 0);

            AddProductToFilteredTree(product, category_id);
        }

        // Cargar por cÃÂ³digo de barras
        auto queryProductsByBarcode = db << "SELECT id, name, price, barcode, byWeight, category_id FROM products WHERE barcode LIKE ?;"
            << searchPattern;

        for (auto&& row : queryProductsByBarcode) {
            size_t id;
            std::string name;
            double price;
            std::optional<std::string> barcode;
            unsigned char byWeightChar;
            size_t category_id;

            row >> id >> name >> price >> barcode >> byWeightChar >> category_id;

            if (loadedProductIds.find(id) != loadedProductIds.end()) continue;
            loadedProductIds.insert(id);

            if (allCategoryIds.find(category_id) == allCategoryIds.end()) continue;

            auto product = std::make_shared<Producto>();
            product->Id = id;
            product->nombre = name;
            product->precio = price;
            product->codigoBarras = barcode.value_or("");
            product->porPeso = (byWeightChar != 0);

            AddProductToFilteredTree(product, category_id);
        }

        // Cargar por precio (si aplica)
        try {
            std::string precioStr = terminoUtf8;
            precioStr.erase(std::remove(precioStr.begin(), precioStr.end(), ','), precioStr.end());

            if (!precioStr.empty() && std::all_of(precioStr.begin(), precioStr.end(),
                [](char c) { return std::isdigit(c) || c == '.'; })) {
                double precio = std::stod(precioStr);

                auto queryProductsByPrice = db << "SELECT id, name, price, barcode, byWeight, category_id FROM products WHERE ABS(price - ?) < 0.01;"
                    << precio;

                for (auto&& row : queryProductsByPrice) {
                    size_t id;
                    std::string name;
                    double price;
                    std::optional<std::string> barcode;
                    unsigned char byWeightChar;
                    size_t category_id;

                    row >> id >> name >> price >> barcode >> byWeightChar >> category_id;

                    if (loadedProductIds.find(id) != loadedProductIds.end()) continue;
                    loadedProductIds.insert(id);

                    if (allCategoryIds.find(category_id) == allCategoryIds.end()) continue;

                    auto product = std::make_shared<Producto>();
                    product->Id = id;
                    product->nombre = name;
                    product->precio = price;
                    product->codigoBarras = barcode.value_or("");
                    product->porPeso = (byWeightChar != 0);

                    AddProductToFilteredTree(product, category_id);
                }
            }
        }
        catch (...) {
            // Ignorar errores de conversiÃÂ³n
        }

        arbolCategorias->ExpandAll();
    }
    catch (const std::exception& e) {
        wxString wxError = wxString::FromUTF8(e.what());
        wxString wxFile = wxString::FromUTF8(__FILE__);
        wxString msg = wxString::Format(
            _("Error loading filtered tree\n\n""File: %s\nLine: %d\n\n""Error message:\n%s"),wxFile,__LINE__,wxError);
        wxMessageBox(msg, "Error SQLite", wxOK | wxICON_ERROR, this);
    }
}