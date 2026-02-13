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

    // Se obtiene el ID de la categorÃÂ­a padre del producto
    wxTreeItemId categoriaId = arbolCategorias->GetItemParent(productId);
    if (!categoriaId.IsOk()) {
        wxMessageBox(_("Error: Category father could not be obtained"), "Error", wxOK | wxICON_ERROR, this);
        return;
    }

    // Obtener nombres tal como aparecen en el ÃÂ¡rbol visual
    wxString nombreProductoVisual = arbolCategorias->GetItemText(productId);
    wxString nombreCategoriaVisual = arbolCategorias->GetItemText(categoriaId);

    // Extraer solo el nombre del producto (antes del precio y cÃÂ³digo de barras)
    wxString nombreProductoLimpioWX = ObtenerNombreProductoLimpioWX(nombreProductoVisual);

    // Confirmar eliminaciÃÂ³n con el usuario
    int respuesta = wxMessageBox(_("Are you sure you want to delete the product '") + nombreProductoLimpioWX + _("'­from the category '") + nombreCategoriaVisual + "'?", _("Confirm deletion"),wxYES_NO | wxICON_QUESTION,this);

    if (respuesta != wxYES) {
        return;
    }

    // Se obtiene el iterador del mapa de items para encontrar la categorÃÂ­a
    auto it = treeItemId_Category_Map.find(categoriaId);
    if (it == treeItemId_Category_Map.end()) {
        wxMessageBox(_("Error: Category not found on item map"), "Error", wxOK | wxICON_ERROR, this);
        return;
    }

    //Se toma la categorÃÂ­a del mapa de items y se guarda en un puntero inteligente de tipo shared_ptr
    std::shared_ptr<Categoria> categoria = it->second;
    if (!categoria) {
        wxMessageBox(_("Error: Pointer to father category is null"), "Error", wxOK | wxICON_ERROR, this);
        return;
    }

    // ELIMINAR DEL ÃÂRBOL VISUAL PRIMERO
    arbolCategorias->Delete(productId);

    ///Se toma ID de la categorÃÂ­a padre para pasarlo al SQL
    size_t idCategoriaPadre = categoria->idCategoria;

    // Se obtiene el vector de productos de la categorÃÂ­a, se usa & para trabajar directamente en el vector sin hacer una copia
    auto& productos = categoria->productos;
    //Se recorre el vector de productos para encontrar el producto a eliminar
    for (auto itProducto = productos.begin(); itProducto != productos.end(); ++itProducto) {
        wxString nombreProductoVectorWX((*itProducto)->nombre.c_str(), wxConvUTF8);
        //Si el nombre del producto coincide con el nombre del producto seleccionado en el ÃÂ¡rbol, false significa que la comparaciÃÂ³n no es sensible a mayÃÂºsculas/minÃÂºsculas
        if (nombreProductoVectorWX.IsSameAs(nombreProductoLimpioWX, false)) {
            /// Guardar una copia del producto antes de borrarlo porque el iterador se invalidarÃÂ¡ al borrar de el vector de productos
            Producto prodAEliminar = *(*itProducto);
            // Eliminar el producto del vector
            productos.erase(itProducto);
            treeItemId_Product_Map.erase(productId);
            /// Eliminar de la tabla temporal pasandole el producto a eliminar y el ID de la categorÃÂ­a padre
            DeleteProductFromDB(prodAEliminar, idCategoriaPadre);
            break;
        }

    }
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