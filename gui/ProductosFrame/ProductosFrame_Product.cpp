#include "gui/ProductosFrame/ProductosFrame.h"
#include "utils/MathUtils.hpp"
#include "utils/ValidateStringInput.h"
#include "gui/ProductosFrame/ChangeProductDialog/ChangeProductDialog.h"

void ProductosFrame::AddProduct(Producto& product, const wxTreeItemId& parentId) {
	if (!parentId.IsOk()) return;
	size_t idCategory = 0;
	auto it = treeItemId_Category_Map.find(parentId);
	if (it != treeItemId_Category_Map.end()) idCategory = it->second->idCategoria; //Get category id to send to DB to know where to insert the product
	InsertProductToDB(product, idCategory);

	wxString showFormatNewProduct = wxString::Format("- %s (%s)", wxString::FromUTF8(product.nombre), FormatFloatWithCommas(product.precio));
	if(!product.codigoBarras.empty()) showFormatNewProduct += wxString::Format(" [%s]", wxString::FromUTF8(product.codigoBarras));
	wxTreeItemId prodItem = arbolCategorias->AppendItem(parentId, showFormatNewProduct);
	arbolCategorias->Expand(parentId);

	//Save the new product in the map and in the category's product list and in the products map
	std::shared_ptr<Producto> newProductPtr = std::make_shared<Producto>(product);
	treeItemId_Product_Map[prodItem] = newProductPtr;
	if(it != treeItemId_Category_Map.end()) it->second->productos.push_back(newProductPtr);

	//Hace visible el nuevo producto en el ÃÂ¡rbol
	arbolCategorias->Expand(parentId);
}

void ProductosFrame::DeleteProduct(const wxTreeItemId& productId) {
    if (!productId.IsOk() || !arbolCategorias) return;

    wxTreeItemId categoriaId = arbolCategorias->GetItemParent(productId);
    if (!categoriaId.IsOk()) {
        wxMessageBox(_("Error: Category father could not be obtained"), "Error", wxOK | wxICON_ERROR, this);
        return;
    }

    wxString nombreProductoVisual = arbolCategorias->GetItemText(productId);
    wxString nombreProductoLimpioWX = ObtenerNombreProductoLimpioWX(nombreProductoVisual);

    auto it = treeItemId_Category_Map.find(categoriaId);
    if (it == treeItemId_Category_Map.end()) {
        wxMessageBox(_("Error: Category not found on item map"), "Error", wxOK | wxICON_ERROR, this);
        return;
    }

    std::shared_ptr<Categoria> categoria = it->second;
    if (!categoria) {
        wxMessageBox(_("Error: Pointer to father category is null"), "Error", wxOK | wxICON_ERROR, this);
        return;
    }

    // ELIMINAR DEL ÁRBOL VISUAL PRIMERO
    arbolCategorias->Delete(productId);

    size_t idCategoriaPadre = categoria->idCategoria;

    auto& productos = categoria->productos;
    for (auto itProducto = productos.begin(); itProducto != productos.end(); ++itProducto) {
        wxString nombreProductoVectorWX((*itProducto)->nombre.c_str(), wxConvUTF8);
        if (nombreProductoVectorWX.IsSameAs(nombreProductoLimpioWX, false)) {
            Producto prodAEliminar = *(*itProducto);
            productos.erase(itProducto);
            treeItemId_Product_Map.erase(productId);
            DeleteProductFromDB(prodAEliminar, idCategoriaPadre);
            break;
        }
    }

    //Limpiar categorías vacías hacia arriba
    LimpiarCategoriasVaciasRecursivo(categoriaId);
}


void ProductosFrame::ChangeProductInfo(const std::shared_ptr<Categoria>& fatherCategory, const wxTreeItemId& item) {
    wxString nombreProducto = arbolCategorias->GetItemText(item);
    std::string nombreProductoLimpio = ObtenerNombreProductoLimpio(nombreProducto);
    for (std::shared_ptr<Producto>& prod : fatherCategory->productos) {
        if (prod->nombre == nombreProductoLimpio) {
            // Crear el diÃÂ¡logo pasando la referencia
            ChangeProductDialog dialog(this, *prod, item, this, fatherCategory, prod);
            dialog.AplicarTema(temaOscuro); // Aplicar tema al diÃÂ¡logo
            if (dialog.ShowModal() == wxID_OK) {
				wxMessageBox(_("Product updated successfully."), _("Success"), wxOK | wxICON_INFORMATION);
            }
            break; // salir del loop
        }
    }
}