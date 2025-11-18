#pragma once
#include <wx/wx.h>

/**
    @struct PurchaseItem
	@brief Struct to hold information about a purchased item product.
**/
struct PurchaseItem {
	wxString productName; /// The name of the product
	wxString barcode; /// The barcode of the product
	double priceAtPurchase; /// The price of the product at the time of purchase
	double quantity; /// The quantity of the product purchased
};