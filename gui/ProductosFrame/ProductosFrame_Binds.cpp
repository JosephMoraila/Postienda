#include "gui/ProductosFrame/ProductosFrame.h"
#include "utils/ValidateStringInput.h"
#include "gui/ProductosFrame/ProductoDialog/ProductoDialog.h"

void ProductosFrame::OnAgregarCategoria(wxCommandEvent& event) {
	wxTreeItemId parentId = arbolCategorias->GetSelection();
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
	wxTreeItemId parentId = arbolCategorias->GetSelection();
	// Si no hay selecciÃÂÃÂ³n, se usa el nodo raÃÂÃÂ­z
	if (!parentId.IsOk()) parentId = arbolCategorias->GetRootItem();
	if (treeItemId_Product_Map.find(parentId) != treeItemId_Product_Map.end()) {
		wxMessageBox(_("You cannot add a product inside another product."), "Error");
		return;
	}
	ProductoDialog dlg(this);
	// Aplicar tema oscuro si estÃÂÃÂ¡ activado
	dlg.AplicarTema(temaOscuro);

	//Solo procede si el usuario presiona "OK"
	if (dlg.ShowModal() == wxID_OK) {
		Producto nuevo;
		std::string nombreProducto = dlg.GetNombre().ToUTF8().data();
		std::string nombreLimpiado = LimpiarCaracteresInvalidosOnAddProduct(nombreProducto);
		std::string nombreLimpio = LimpiarYValidarNombre(nombreLimpiado);
		if(nombreLimpiado.empty() && nombreLimpio.length() <= 0) {
			wxMessageBox(_("Invalid name. The product was not created."), "Error");
			return;
		}
		nombreLimpio = TruncarNombre(nombreLimpio, 80);
		wxString nombreFinal = wxString::FromUTF8(nombreLimpio);
		std::shared_ptr<Categoria> parentCategory = treeItemId_Category_Map[parentId];
		size_t parentCategoryId = parentCategory->idCategoria;
		if(ExistsProductNameInSameCategory(nombreFinal, parentCategoryId)) {
			wxMessageBox(wxString::Format(_("There is already a product called '%s' in this category."), nombreFinal), "Error", wxOK | wxICON_WARNING, this);
			return;
		}
		wxString codigoBarrasWx = dlg.GetCodigoBarras();
		std::string codigoBarras;
		if (!codigoBarrasWx.IsEmpty()) {
			codigoBarras = LimpiarCodigoBarras(codigoBarrasWx.ToStdString());
			if (!codigoBarras.empty() && ExistsCodebarGlobal(codigoBarras)) {
				wxMessageBox(wxString::Format(_("The barcode '%s' is already in use by another product."),wxString::FromUTF8(codigoBarras)), _("Duplicated barcode"), wxOK | wxICON_WARNING);
				return;
			}
		}
		bool isByWeight = dlg.EsPorPeso();
		nuevo.nombre = nombreLimpio;
		double price = dlg.GetPrecio();
		double roundPrice = round2(price); // Redondear a dos decimales para evitar problemas en SQL
		nuevo.precio = roundPrice;
		nuevo.codigoBarras = codigoBarras;
		nuevo.porPeso = isByWeight;
		AddProduct(nuevo, parentId);
	}
}

void ProductosFrame::OnEliminar(wxCommandEvent& event) {
	wxTreeItemId selectedId = arbolCategorias->GetSelection();
	if (!selectedId.IsOk()) {
		wxMessageBox(_("There are no items selected to delete."), "Error");
		return;
	}

	bool esCategoria = (treeItemId_Category_Map.find(selectedId) != treeItemId_Category_Map.end());
	bool esProducto = (treeItemId_Product_Map.find(selectedId) != treeItemId_Product_Map.end());

	if (!esCategoria && !esProducto) {
		wxMessageBox(_("Selected item is not valid."), "Error");
		return;
	}

	if (esCategoria) {
		auto categoriaAEliminar = treeItemId_Category_Map[selectedId];
		int respuesta = wxMessageBox(wxString::Format(_("Are you sure you want to delete the '%s' category?"), wxString::FromUTF8(categoriaAEliminar->nombre.c_str())), _("Confirm Deletion"), wxYES_NO | wxICON_QUESTION);
		if (respuesta != wxYES) return; // El usuario cancelÃÂÃÂ³ la eliminaciÃÂÃÂ³n
		else DeleteCategory(selectedId);
		
	}
	else if (esProducto) DeleteProduct(selectedId);

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