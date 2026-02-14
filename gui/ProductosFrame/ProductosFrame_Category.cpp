#include "gui/ProductosFrame/ProductosFrame.h"
#include "utils/ValidateStringInput.h"

void ProductosFrame::Addcategory(const wxTreeItemId& parentId, const wxString& nombre) {
	if (!parentId.IsOk()) return;
	wxTreeItemId newItem = arbolCategorias->AppendItem(parentId, nombre);
	arbolCategorias->Expand(parentId);
	//Obtiene el puntero a la categorÃÂ­a padre desde el mapa
	std::shared_ptr<Categoria> padre = treeItemId_Category_Map[parentId];
	auto nueva = std::make_shared<Categoria>();
	if (padre) {
		size_t fatherId = padre->idCategoria;
		std::string newCategoryName = std::string(nombre.ToUTF8());
		size_t newCatId = InsertCategoryToDB(newCategoryName, fatherId);
		nueva->nombre = newCategoryName;
		nueva->idCategoria = newCatId;
		padre->subcategorias.push_back(nueva);
		treeItemId_Category_Map[newItem] = nueva;
	}

}

void ProductosFrame::DeleteCategory(const wxTreeItemId& categoryId) {
	if (!categoryId.IsOk() || !arbolCategorias) return;
	auto it = treeItemId_Category_Map.find(categoryId);
	if (it == treeItemId_Category_Map.end()) return;

	std::shared_ptr<Categoria> categoriaActual = it->second;
	// Eliminar recursivamente las subcategorÃÂ­as
	wxTreeItemIdValue cookie;
	wxTreeItemId child = arbolCategorias->GetFirstChild(categoryId, cookie);
	while (child.IsOk()) {
		wxTreeItemId siguiente = arbolCategorias->GetNextChild(categoryId, cookie);
		DeleteCategory(child); // Llamada recursiva para eliminar subcategorÃÂ­as
		child = siguiente;
	}
	if (categoriaActual) {
		categoriaActual->productos.clear();        // Borra productos
		//categoriaActual->subcategorias.clear(); 
		/*No se necesita porque las subcategorÃÂ­as ya se eliminan recursivamente,
		y cada categorÃÂ­a solo tiene un padre. Se deja comentado por claridad.*/
	}
	DeleteReferencesCategory(categoryId);
	treeItemId_Category_Map.erase(categoryId);
	DeleteCategoryFromDB(categoriaActual);
	arbolCategorias->Delete(categoryId);
}

void ProductosFrame::DeleteReferencesCategory(const wxTreeItemId& categoryId) {
	auto categoriaAEliminar = treeItemId_Category_Map[categoryId];
	size_t categoriaId = categoriaAEliminar->idCategoria;

	// Buscar su categorÃÂ­a padre (si existe) y eliminar refencia
	wxTreeItemId parentId = arbolCategorias->GetItemParent(categoryId);
	auto itPadre = treeItemId_Category_Map.find(parentId);
	if (itPadre != treeItemId_Category_Map.end()) {
		std::vector<std::shared_ptr<Categoria>>& subcategoriesFather = itPadre->second->subcategorias;
		subcategoriesFather.erase(
			std::remove_if(subcategoriesFather.begin(), subcategoriesFather.end(),
				[&](const std::shared_ptr<Categoria>& subcategory) { return subcategory->idCategoria == categoriaId; }),
			subcategoriesFather.end()
		);
	}
}

void ProductosFrame::ChangeCategoryInfo(const wxTreeItemId& item, const wxTreeItemId& parentId) {
	std::shared_ptr<Categoria> categoria = treeItemId_Category_Map[item];
	std::string nombreCategoria = categoria->nombre;
	wxString nombreCategoriaWX = wxString::FromUTF8(nombreCategoria);

	// Pedir al usuario un nuevo nombre para la categorÃÂ­a, el nombre por defecto es el nombre actual de la categorÃÂ­a
	wxString nombreNuevo = wxGetTextFromUser(_("Enter the category name:"), _("N­ew category"), nombreCategoriaWX);
	if (nombreNuevo.IsEmpty()) return; // Usuario cancelÃÂ³
	wxString nombreLimpioWX = LimpiarYValidarNombreWx(nombreNuevo);
	std::string nombreLimpio = nombreLimpioWX.ToUTF8().data();
	if (!EsNombreValido(nombreLimpio)) return;
	wxString nombreFinal = TruncarNombreWxString(nombreLimpioWX, 50);

	// Verificar si ya existe una categorÃÂ­a con ese nombre en el mismo nivel
	std::shared_ptr<Categoria> padre = treeItemId_Category_Map[parentId];
	size_t categoriaIdPadre = padre->idCategoria;
	if (ExistsCategoryNameInSameLevel(nombreFinal, categoriaIdPadre)) {
		wxMessageBox(_("There is already a category with that name at this level."), ("Duplicate Category"), wxOK | wxICON_WARNING);
		return;
	}
	categoria->nombre = nombreFinal.ToUTF8().data(); // Guarda en std::string UTF-8
	UpdateCategoryNameInDB(nombreFinal.ToUTF8().data(), categoria->idCategoria, nombreCategoria);
	arbolCategorias->SetItemText(item, nombreFinal);
}

void ProductosFrame::LimpiarCategoriasVaciasRecursivo(const wxTreeItemId& categoryId) {
	if (!categoryId.IsOk()) return;

	wxTreeItemId rootId = arbolCategorias->GetRootItem();
	wxTreeItemId parentId = arbolCategorias->GetItemParent(categoryId);

	// Protecciones: NO eliminar si es raíz O si no tiene padre válido
	if (categoryId == rootId || !parentId.IsOk()) {
		return;
	}

	// Verificar si la categoría está vacía (sin hijos)
	if (!arbolCategorias->ItemHasChildren(categoryId)) {
		auto it = treeItemId_Category_Map.find(categoryId);
		if (it == treeItemId_Category_Map.end()) {
			return;  // Categoría no encontrada en el mapa
		}

		auto categoria = it->second;

		// Eliminar de la base de datos
		DeleteCategoryFromDB(categoria);

		// Eliminar referencias del padre
		DeleteReferencesCategory(categoryId);

		// Eliminar del mapa
		treeItemId_Category_Map.erase(categoryId);

		// Eliminar del árbol visual
		arbolCategorias->Delete(categoryId);

		// Verificar recursivamente el padre
		LimpiarCategoriasVaciasRecursivo(parentId);
	}
}