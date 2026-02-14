#include "gui/ProductosFrame/ProductosFrame.h"


void ProductosFrame::OnBeginDrag(wxTreeEvent& event) {
    wxArrayTreeItemIds selections;
    arbolCategorias->GetSelections(selections);

    if (selections.IsEmpty()) {
		event.Veto(); //Veto es cancelar el evento, no se inicia el drag
        return;
    }

    // Filtrar la raíz - no se puede mover
    wxTreeItemId rootId = arbolCategorias->GetRootItem();
    m_draggedItems.Clear();

    for (size_t i = 0; i < selections.GetCount(); i++) {
        if (selections[i] != rootId) {
            m_draggedItems.Add(selections[i]);
        }
    }

    if (m_draggedItems.IsEmpty()) {
        wxMessageBox(_("The root category cannot be moved."), _("Invalid selection"), wxOK | wxICON_WARNING);
        event.Veto();
        return;
    }

    event.Allow();  // Permitir el drag
}

void ProductosFrame::OnEndDrag(wxTreeEvent& event) {
	wxTreeItemId itemDst = event.GetItem(); // El item sobre el que se soltó el drag

    if (!itemDst.IsOk()) {
		m_draggedItems.Clear(); // Limpiar la lista de items arrastrados
        return;
    }

    // Verificar que el destino sea una categoría
    bool destinoEsCategoria = (treeItemId_Category_Map.find(itemDst) != treeItemId_Category_Map.end());

    if (!destinoEsCategoria) {
        wxMessageBox(_("You can only move items to a category."), _("Invalid destination"), wxOK | wxICON_WARNING);
        m_draggedItems.Clear();
        return;
    }

    wxTreeItemId rootId = arbolCategorias->GetRootItem();

    // Mover cada item seleccionado
    for (size_t i = 0; i < m_draggedItems.GetCount(); i++) {
        wxTreeItemId itemSrc = m_draggedItems[i];

        // Validar que no se mueva a si mismo
        if (itemSrc == itemDst) {
            continue;
        }

        // Validar que no se mueva una categoría a uno de sus descendientes
        bool srcEsCategoria = (treeItemId_Category_Map.find(itemSrc) != treeItemId_Category_Map.end());
        if (srcEsCategoria && EsDescendiente(itemDst, itemSrc)) {
            wxMessageBox(_("Cannot move a category to one of its subcategories."), _("Invalid move"), wxOK | wxICON_WARNING);
            continue;
        }

        // Validar que no esté ya en esa categoría
        wxTreeItemId categoriaActual = arbolCategorias->GetItemParent(itemSrc);
        if (categoriaActual == itemDst) {
            continue;  // Ya está en esa categoría
        }

        // Mover el item
        MoverItemACategoria(itemSrc, itemDst);
    }

    m_draggedItems.Clear();
}

bool ProductosFrame::EsDescendiente(const wxTreeItemId& posibleDescendiente, const wxTreeItemId& ancestro) {
    wxTreeItemId current = posibleDescendiente;

    while (current.IsOk()) {
        wxTreeItemId parent = arbolCategorias->GetItemParent(current);
        if (!parent.IsOk()) break;

        if (parent == ancestro) {
            return true;
        }

        current = parent;
    }

    return false;
}

void ProductosFrame::MoverItemACategoria(const wxTreeItemId& item, const wxTreeItemId& nuevaCategoria) {
    bool esCategoria = (treeItemId_Category_Map.find(item) != treeItemId_Category_Map.end());
    bool esProducto = (treeItemId_Product_Map.find(item) != treeItemId_Product_Map.end());

    wxTreeItemId categoriaAnterior = arbolCategorias->GetItemParent(item);

    if (esCategoria) {
        // Mover categoría
        auto categoria = treeItemId_Category_Map[item];
        auto nuevaCategoriaPadre = treeItemId_Category_Map[nuevaCategoria];
        auto categoriaAnteriorPadre = treeItemId_Category_Map[categoriaAnterior];

        // Verificar nombre duplicado en el nuevo nivel
        wxString nombreCategoria = wxString::FromUTF8(categoria->nombre);
        if (ExistsCategoryNameInSameLevel(nombreCategoria, nuevaCategoriaPadre->idCategoria)) {
            wxMessageBox(wxString::Format(_("A category named '%s' already exists in the destination."), nombreCategoria),_("Duplicate name"),wxOK | wxICON_WARNING);
            return;
        }


        // Eliminar de subcategorías del padre anterior
        auto& subcategoriasAnterior = categoriaAnteriorPadre->subcategorias;
        subcategoriasAnterior.erase(
            std::remove_if(subcategoriasAnterior.begin(), subcategoriasAnterior.end(),
                [&](const std::shared_ptr<Categoria>& cat) {
                    return cat->idCategoria == categoria->idCategoria;
                }),
            subcategoriasAnterior.end()
        );

        // Agregar a subcategorías del nuevo padre
        nuevaCategoriaPadre->subcategorias.push_back(categoria);

        // Actualizar en base de datos
        UpdateCategoryParentInDB(categoria->idCategoria, nuevaCategoriaPadre->idCategoria);

        // Recrear en el árbol
        wxString nombreVisual = arbolCategorias->GetItemText(item);
        wxTreeItemId nuevoItem = arbolCategorias->AppendItem(nuevaCategoria, nombreVisual);

        // Copiar hijos recursivamente
        CopiarHijosRecursivo(item, nuevoItem);

        // Actualizar mapa
        treeItemId_Category_Map[nuevoItem] = categoria;
        treeItemId_Category_Map.erase(item);

        // Eliminar item anterior
        arbolCategorias->Delete(item);

        // Expandir destino
        arbolCategorias->Expand(nuevaCategoria);
        arbolCategorias->SelectItem(nuevoItem);

        // No eliminar categorías vacías automáticamente
        // LimpiarCategoriasVaciasRecursivo(categoriaAnterior);
    }
    else if (esProducto) {
        //  Mover producto
        auto producto = treeItemId_Product_Map[item];
        auto nuevaCategoriaPadre = treeItemId_Category_Map[nuevaCategoria];
        auto categoriaAnteriorPadre = treeItemId_Category_Map[categoriaAnterior];

        // Verificar nombre duplicado en la nueva categoría
        wxString nombreProducto = wxString::FromUTF8(producto->nombre);
        if (ExistsProductNameInSameCategory(nombreProducto, nuevaCategoriaPadre->idCategoria)) {
            wxMessageBox(wxString::Format(_("A product named '%s' already exists in the destination category."), nombreProducto), _("Duplicate name"),wxOK | wxICON_WARNING);
            return;
        }

        // Eliminar del vector de productos de la categoría anterior
        auto& productosAnterior = categoriaAnteriorPadre->productos;
        productosAnterior.erase(
            std::remove_if(productosAnterior.begin(), productosAnterior.end(),
                [&](const std::shared_ptr<Producto>& prod) {
                    return prod->Id == producto->Id;
                }),
            productosAnterior.end()
        );

        // Agregar al vector de productos de la nueva categoría
        nuevaCategoriaPadre->productos.push_back(producto);

        // Actualizar en base de datos
        UpdateProductCategoryInDB(producto->Id, categoriaAnteriorPadre->idCategoria, nuevaCategoriaPadre->idCategoria);

        // Recrear en el árbol
        wxString nombreVisual = arbolCategorias->GetItemText(item);
        wxTreeItemId nuevoItem = arbolCategorias->AppendItem(nuevaCategoria, nombreVisual);

        // Actualizar mapa
        treeItemId_Product_Map[nuevoItem] = producto;
        treeItemId_Product_Map.erase(item);

        // Eliminar item anterior
        arbolCategorias->Delete(item);

        // Expandir destino
        arbolCategorias->Expand(nuevaCategoria);
        arbolCategorias->SelectItem(nuevoItem);

        // No eliminar categorías vacías automáticamente
        // LimpiarCategoriasVaciasRecursivo(categoriaAnterior);
    }
}

void ProductosFrame::CopiarHijosRecursivo(const wxTreeItemId& src, const wxTreeItemId& dst) {
    wxTreeItemIdValue cookie;
    wxTreeItemId child = arbolCategorias->GetFirstChild(src, cookie);

    while (child.IsOk()) {
        wxString childLabel = arbolCategorias->GetItemText(child);
        wxTreeItemId newChild = arbolCategorias->AppendItem(dst, childLabel);

        // Actualizar mapas
        bool esCategoria = (treeItemId_Category_Map.find(child) != treeItemId_Category_Map.end());
        bool esProducto = (treeItemId_Product_Map.find(child) != treeItemId_Product_Map.end());

        if (esCategoria) {
            auto categoria = treeItemId_Category_Map[child];
            treeItemId_Category_Map[newChild] = categoria;

            // Recursivo para subcategorías
            if (arbolCategorias->ItemHasChildren(child)) {
                CopiarHijosRecursivo(child, newChild);
            }
        }
        else if (esProducto) {
            auto producto = treeItemId_Product_Map[child];
            treeItemId_Product_Map[newChild] = producto;
        }

        child = arbolCategorias->GetNextChild(src, cookie);
    }
}