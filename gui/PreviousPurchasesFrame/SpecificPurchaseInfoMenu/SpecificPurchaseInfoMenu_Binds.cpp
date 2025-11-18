#include "gui/PreviousPurchasesFrame/SpecificPurchaseInfoMenu/SpecificPurchaseInfoMenu.hpp"
#include "constants/DB/DB.h"
#include "gui/MainFrame/Purchase_Item.hpp"
#include "print/Print_Utils_Getters.hpp"
#include "constants/MESSAGES_ADVICE/ERROR/PRINTING/PRINTING_ERRORS.hpp"
using namespace PRINTING_ERROR_MESSAGES;

void SpecificPurchaseInfoMenu::OnSize(wxSizeEvent& event){
	event.Skip(); // deja que wxWidgets ajuste la ventana
	CallAfter([this]() { // CallAfter para asegurar que la ventana ya se haya redimensionado completamente
		AjustarColumnasListCtrl();
		if (mainPanel) mainPanel->Layout();
		});
}

void SpecificPurchaseInfoMenu::OnPrintTicketButtonClicked(wxCommandEvent& event){
	char mode = PrintGetters::GetMode();
	try {
		sqlite::database db(GetDBPath());
		db << "PRAGMA foreign_keys = ON;";
		db << "PRAGMA encoding = 'UTF-8';";
		auto query = db << "SELECT product_id, product_name, price_at_purchase FROM purchase_items WHERE purchase_id = ?;" << this->m_purchaseId;
		std::vector<PurchaseItem> items; // product_id, product_name, price_at_purchase
		for (auto&& row : query) {
			PurchaseItem item;
			unsigned long long productId;
			std::string productName;
			double priceAtPurchase;
			row >> productId >> productName >> priceAtPurchase;
			std::string barcode = GetBarcodeIfExists(db, productId);
			item.productName = wxString::FromUTF8(productName); item.barcode = wxString::FromUTF8(barcode); item.priceAtPurchase = priceAtPurchase;
			items.push_back(item);
		}
		#ifdef _WIN32
		if (mode == 0) PrintCanvasPurchaseWindows(items);//Canvas mode
		#elif __linux__
		if(mode == 0) wxMessageBox(_("No canvas available by the moment. Select EscPos"), _("No available"), wxOK | wxICON_WARNING);
		#endif
		else if (mode == 1) PrintEscposPurchase(items);//EscPos mode
		else wxMessageBox(ERROR_GETTING_MODE_MESSAGE, "Error", wxOK | wxICON_ERROR, this);
	}
	catch (const std::exception& e) {
		wxMessageBox(ERROR_PRINTING_TICKET_MESSAGE + wxString(e.what()), "Error", wxOK | wxICON_ERROR);
	}
}