#include "gui/ProductosFrame/ProductosFrame.h"
#include "gui/ProductosFrame/StockDialog/StockDialog.hpp"

void ProductosFrame::StockProduct(const wxTreeItemId& item) {
    auto itProducto = treeItemId_Product_Map.find(item);
    if (itProducto == treeItemId_Product_Map.end()) return;
    std::shared_ptr<Producto> producto = itProducto->second;
    if (!producto) return;
    size_t idProducto = producto->Id;
    bool byWeight = producto->porPeso;

    std::variant<unsigned long long, double> stock = GetStockFromDB(idProducto, byWeight);

    // ÃÂ¡NO convertir a double! Pasar directamente el variant
    StockDialog dlg(this, stock);  // Pasar el variant directamente
    dlg.AplicarTema(temaOscuro);

    if (dlg.ShowModal() == wxID_OK) {
        std::variant<unsigned long long, double> stockVariant = dlg.GetStock();

        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>; // Deducir el tipo de arg limpiando de referencias y const
            if constexpr (std::is_same_v<T, double>) UpdateStockInDB(idProducto, arg);
            else UpdateStockInDB(idProducto, arg);
            }, stockVariant);
    }
}