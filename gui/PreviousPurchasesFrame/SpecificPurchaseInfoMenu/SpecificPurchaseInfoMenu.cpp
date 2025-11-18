#include "gui/PreviousPurchasesFrame/SpecificPurchaseInfoMenu/SpecificPurchaseInfoMenu.hpp"
#include "utils/window/WindowUtils.h"

SpecificPurchaseInfoMenu::SpecificPurchaseInfoMenu(wxWindow* parent, unsigned long long id) : wxFrame(parent, wxID_ANY, wxString::Format(_("Purchase ID: %llu"), id)), m_purchaseId(id){
	Widgets();
	Bind(wxEVT_SIZE, &SpecificPurchaseInfoMenu::OnSize, this);
	LoadPurchaseDetails(id);
}
void SpecificPurchaseInfoMenu::AplicarTema(bool oscuro) {
	temaOscuro = oscuro;
	ApplyTheme(mainPanel, temaOscuro);
	ApplyTheme(buttonPanel, temaOscuro);
	Refresh();
	Update();
}