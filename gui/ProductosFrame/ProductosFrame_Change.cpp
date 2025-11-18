#include "gui/ProductosFrame/ProductosFrame.h"

void ProductosFrame::ChangeInfo(const wxTreeItemId& item, const bool& esCategoria) {
	wxTreeItemId parentId = arbolCategorias->GetItemParent(item);
	if (!parentId.IsOk()) return;
	auto itParent = treeItemId_Category_Map.find(parentId);
	if (itParent == treeItemId_Category_Map.end()) return;

	if (esCategoria) ChangeCategoryInfo(item, parentId);
	else {
		std::shared_ptr<Categoria> categoriaPadre = itParent->second;
		if (!categoriaPadre) return;
		ChangeProductInfo(categoriaPadre, item);
	}
}