#include "gui/ReturnsFrame/ReturnsFrame.hpp"
#include "constants/MESSAGES_ADVICE/WARNING/WARNING_MESSAGES.hpp"
using namespace WARNING_MESSAGES;

void ReturnsFrame::OnButtonValidateClick(wxCommandEvent& event) {
	ValidatePurchaseId();
}

void ReturnsFrame::OnEnterPurchaseIdInput(wxCommandEvent& event) {
	ValidatePurchaseId();
}

void ReturnsFrame::OnReturnProduct(wxCommandEvent& event) {
    int filaSeleccionada = grid->GetGridCursorRow(); //Selecciona primer fila seleccionada aunque se hayan seleccionado muchos

    // Validar que haya una fila seleccionada
    if (filaSeleccionada == wxNOT_FOUND || grid->GetNumberRows() == 0) {
        wxMessageBox(_("No product selected"), WARNING, wxOK | wxICON_WARNING, this);
        return;
    }
    wxString idProduct = grid->GetCellValue(filaSeleccionada, 0);
	wxString purchaseItemId = grid->GetCellValue(filaSeleccionada, 1);
    wxString productName = grid->GetCellValue(filaSeleccionada, 2);
    wxString productQuantity = grid->GetCellValue(filaSeleccionada, 3);
	unsigned long long idProductNumber; unsigned long long purchaseItemIdNumber;
    bool successConvertIdProduct = idProduct.ToULongLong(&idProductNumber);
	bool successConvertPurchaseItemId = purchaseItemId.ToULongLong(&purchaseItemIdNumber);
    double quantity = 0.0;
    try {
        quantity = std::stod(productQuantity.ToStdString());
    }
    catch (const std::exception& e) {
        wxMessageBox(_("There was an error getting the quantity"), "Error", wxOK | wxICON_ERROR, this);
        return;
    }
    if (!successConvertIdProduct) {
        wxMessageBox(_("There was an error getting the product ID"), "Error", wxOK | wxICON_ERROR, this);
        return;
    }
    if (!successConvertPurchaseItemId) {
        wxMessageBox(_("There was an error getting the purchase item ID"), "Error", wxOK | wxICON_ERROR, this);
        return;
	}
    bool productExists = ValidateProductID(idProductNumber);
    if (!productExists) {
        wxMessageBox(_("The selected product no longer exists."), _("Product does not exists"), wxOK | wxICON_ERROR, this);
		return;
    }
	bool alreadyReturned = ValidateIfProductIsAlreadyReturned(purchaseItemIdNumber);
    if (alreadyReturned) {
		wxMessageBox(_("The selected product has already been returned."), _("Product already returned"), wxOK | wxICON_ERROR, this);
        return;
	}
    int respuesta = wxMessageBox(wxString::Format(_("Confirm return of the product: %s"), productName), _("Confirm return"), wxYES_NO | wxICON_QUESTION, this);
    if (respuesta != wxYES) return;
    ReturnProductStock(idProductNumber, quantity, purchaseItemIdNumber, productName);
}

void ReturnsFrame::OnClose(wxCloseEvent& event) {
    isDestroying = true; 
    event.Skip();
}


void ReturnsFrame::OnSize(wxSizeEvent& event) {
    event.Skip();

    // Si se está destruyendo, no hacer nada
    if (isDestroying) return;
    if (!productsPanel || !grid || !purchaseIdPanel) return;

    wxSize clientSize = GetClientSize();
    int purchaseIdHeight = purchaseIdPanel->GetSize().GetHeight();
    int availableHeight = clientSize.GetHeight() - purchaseIdHeight - 60;
    int gridHeight = std::max(300, availableHeight);

    grid->SetMinSize(wxSize(-1, gridHeight));
    grid->SetMaxSize(wxSize(-1, gridHeight));

    productsPanel->Layout();
    Layout();
}
