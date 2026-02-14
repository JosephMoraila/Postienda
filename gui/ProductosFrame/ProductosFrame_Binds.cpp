#include "gui/ProductosFrame/ProductosFrame.h"
#include "utils/ValidateStringInput.h"
#include "gui/ProductosFrame/ProductoDialog/ProductoDialog.h"

void ProductosFrame::OnAgregarCategoria(wxCommandEvent& event) {
    wxArrayTreeItemIds selections;
    arbolCategorias->GetSelections(selections);
	wxTreeItemId parentId = selections[0]; //Tomamos el primer elemento seleccionado como padre
	// Si no hay selecciÃÂÃÂ³n, se usa el nodo raÃÂÃÂ­z
	if (!parentId.IsOk()) parentId = arbolCategorias->GetRootItem();
	if (treeItemId_Category_Map.find(parentId) == treeItemId_Category_Map.end()) {
		wxMessageBox(_("You cannot add a category within a product."), "Error");
		return;
	}
	wxString nombre = wxGetTextFromUser(_("Enter category name:"), _("New category"));

	// Usar ValidateStringInput para limpiar y validar
	std::string nombreLimpio = LimpiarYValidarNombre(nombre);
	if (nombreLimpio.empty()) {
		wxMessageBox(_("Invalid name. Category was not created"), "Error");
		return;
	}
	nombreLimpio = TruncarNombre(nombreLimpio, 50);
	//Toma una cadena para convertirla en tipo wxString
	wxString nombreFinal = wxString::FromUTF8(nombreLimpio);
	std::shared_ptr<Categoria> parentCategory = treeItemId_Category_Map[parentId];
	size_t parentCategoryId = parentCategory->idCategoria;
	if (ExistsCategoryNameInSameLevel(nombreFinal, parentCategoryId)) {
		wxMessageBox(wxString::Format(_("There is already a category called '%s' at this level."), nombreFinal),"Error",wxOK | wxICON_WARNING,this);
		return;
	}
	Addcategory(parentId, nombreFinal);
}

void ProductosFrame::OnAgregarProducto(wxCommandEvent& event) {
    wxArrayTreeItemIds selections;
    arbolCategorias->GetSelections(selections);
    wxTreeItemId parentId = selections[0]; //Tomamos el primer elemento seleccionado como padre
	// Si no hay selecciÃÂÃÂ³n, se usa el nodo raÃÂÃÂ­z
	if (!parentId.IsOk()) parentId = arbolCategorias->GetRootItem();
	if (treeItemId_Product_Map.find(parentId) != treeItemId_Product_Map.end()) {
		wxMessageBox(_("You cannot add a product inside another product."), "Error");
		return;
	}
	ProductoDialog dlg(this, parentId, this);
	// Aplicar tema oscuro si estÃÂÃÂ¡ activado
	dlg.AplicarTema(temaOscuro);

	//Solo procede si el usuario presiona "OK"
	if (dlg.ShowModal() == wxID_OK) {
		wxMessageBox(_("Product added successfully."), _("Success"), wxOK | wxICON_INFORMATION);
	}
}

void ProductosFrame::OnEliminar(wxCommandEvent& event) {
    wxArrayTreeItemIds selections;
    arbolCategorias->GetSelections(selections);

    if (selections.IsEmpty()) {
        wxMessageBox(_("There are no items selected to delete."), "Error");
        return;
    }

    // Filtrar la categoría raíz de las selecciones
    wxTreeItemId rootId = arbolCategorias->GetRootItem();
    wxArrayTreeItemIds selectionsFiltered;

    for (size_t i = 0; i < selections.GetCount(); i++) {
        if (selections[i] != rootId) {
            selectionsFiltered.Add(selections[i]);
        }
    }

    // Verificar si quedaron items válidos después de filtrar
    if (selectionsFiltered.IsEmpty()) {
        return;
    }

    // Usar selectionsFiltered en lugar de selections desde aquí
    if (selectionsFiltered.size() == 1) {
        wxTreeItemId selectedId = selectionsFiltered[0];
        bool esCategoria = (treeItemId_Category_Map.find(selectedId) != treeItemId_Category_Map.end());
        bool esProducto = (treeItemId_Product_Map.find(selectedId) != treeItemId_Product_Map.end());

        if (!esCategoria && !esProducto) {
            wxMessageBox(_("Selected item is not valid."), "Error");
            return;
        }

        if (esCategoria) {
            auto categoriaAEliminar = treeItemId_Category_Map[selectedId];
            int respuesta = wxMessageBox(
                wxString::Format(_("Are you sure you want to delete the '%s' category?"),
                    wxString::FromUTF8(categoriaAEliminar->nombre.c_str())),
                _("Confirm Deletion"),
                wxYES_NO | wxICON_QUESTION
            );
            if (respuesta != wxYES) return;

            wxTreeItemId parentId = arbolCategorias->GetItemParent(selectedId);
            DeleteCategory(selectedId);

            if (parentId.IsOk()) {
                LimpiarCategoriasVaciasRecursivo(parentId);
            }
        }
        else if (esProducto) {
            wxTreeItemId categoriaId = arbolCategorias->GetItemParent(selectedId);
            wxString nombreProductoVisual = arbolCategorias->GetItemText(selectedId);
            wxString nombreCategoriaVisual = arbolCategorias->GetItemText(categoriaId);
            wxString nombreProductoLimpioWX = ObtenerNombreProductoLimpioWX(nombreProductoVisual);

            int respuesta = wxMessageBox(
                _("Are you sure you want to delete the product '") + nombreProductoLimpioWX +
                _("' from the category '") + nombreCategoriaVisual + "'?",
                _("Confirm deletion"),
                wxYES_NO | wxICON_QUESTION,
                this
            );
            if (respuesta != wxYES) return;

            DeleteProduct(selectedId);
        }
    }
    else {
        // Eliminación múltiple
        int respuesta = wxMessageBox(
            _("Are you sure you want to delete the selected items?"),
            _("Confirm Deletion"),
            wxYES_NO | wxICON_QUESTION
        );
        if (respuesta != wxYES) return;

        std::set<wxTreeItemId> categoriasAVerificar;

        for (const auto& selectedId : selectionsFiltered) {  // ✅ Usar selectionsFiltered
            bool esCategoria = (treeItemId_Category_Map.find(selectedId) != treeItemId_Category_Map.end());
            bool esProducto = (treeItemId_Product_Map.find(selectedId) != treeItemId_Product_Map.end());

            wxTreeItemId parentId = arbolCategorias->GetItemParent(selectedId);

            if (esCategoria) {
                DeleteCategory(selectedId);
                if (parentId.IsOk()) {
                    categoriasAVerificar.insert(parentId);
                }
            }
            else if (esProducto) {
                DeleteProduct(selectedId);
            }
        }

        for (const auto& catId : categoriasAVerificar) {
            if (catId.IsOk()) {
                LimpiarCategoriasVaciasRecursivo(catId);
            }
        }
    }
}

//Buscar:

void ProductosFrame::OnBuscar(wxCommandEvent& event) {
	wxString termino = textoBusqueda->GetValue().Trim().Trim(false);
	wxString cleanTermino = LimpiarYValidarNombreWx(termino);
	if (cleanTermino.Trim().IsEmpty()) {
		OnLimpiarBusqueda(event);
		return;
	}
	if (termino.Length() >= 1) { // MÃÂÃÂ­nimo 1 carÃÂÃÂ¡cter
		// Limpiar ÃÂÃÂ¡rbol actual
		arbolCategorias->DeleteAllItems();
		// Limpiar mapas porque se van a recargar en base a la bÃÂÃÂºsqueda
		treeItemId_Category_Map.clear();
		treeItemId_Product_Map.clear();

		// Recargar solo items filtrados
		LoadFilteredTreeFromDB(cleanTermino);
	}
}

void ProductosFrame::OnLimpiarBusqueda(wxCommandEvent& event) {
	textoBusqueda->SetValue("");
	// Recargar todo el ÃÂÃÂ¡rbol desde la base de datos
	arbolCategorias->DeleteAllItems();
	treeItemId_Category_Map.clear();
	treeItemId_Product_Map.clear();
	LoadCategoriesFromDB();
	LoadProductsFromDB();
}