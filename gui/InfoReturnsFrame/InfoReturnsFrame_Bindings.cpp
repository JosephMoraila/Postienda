#include "gui/InfoReturnsFrame/InfoReturnsFrame.hpp"

void InfoReturnsFrame::OnSearch(wxCommandEvent& event) {
    unsigned long long id = GetID();
    wxString fechaInicio = GetStartDate();        // YYYY-MM-DD
    wxString inicioHora = GetTimeStart(); // HH:MM:SS
    wxString StartDateTime = fechaInicio + " " + inicioHora; // YYYY-MM-DD HH:MM:SS
    wxString endDate = GetEndDate();
    wxString endTime = GetTimeEnd();
    wxString EndDateTime = endDate + " " + endTime;
    double minAmount = GetMinAmount();
    double maxAmount = GetMaxAmount();
    wxString Worker = GetWorker();
    wxString ProductName = GetProductName();
    unsigned long long purchaseId = GetPurchaseID();
	double minQuantity = GetMinQuantity();
	double maxQuanity = GetMaxQuantity();

    std::string worker = std::string(Worker.mb_str(wxConvUTF8));
    std::string start = StartDateTime.ToStdString();
    std::string end = EndDateTime.ToStdString();
	std::string productName = std::string(ProductName.mb_str(wxConvUTF8));

    if (id != 0) {//When ID is not 0 means that when are searching by id only
        prevButton->Disable(); nextButton->Disable();
        GetReturnById(id);
    }
    else if (id == 0 && purchaseId != 0) {//Se busca solo por purchaseId
        prevButton->Disable(); nextButton->Disable();
        GetReturnsByPurchaseID(purchaseId);
    }
	else { //Se busca por los otros filtros, sin id ni purchaseId
        currentOffset = 0; //Start a new searching
        //Save the data to instance members so when OnLoadPrev or OnLoadNext are called keep searching the same data but with differente given pages from DB
        this->startDateTime = start; this->endDateTime = end; this->minAmount = minAmount; this->maxAmount = maxAmount; this->worker = worker; this->productName = productName; this->minQuantity = minQuantity; this->maxQuantity = maxQuanity;
		GetReturns(start, end, minAmount, maxAmount, worker, productName, minQuantity, maxQuanity);
    }
}


void InfoReturnsFrame::OnSize(wxSizeEvent& event) {
    event.Skip(); // deja que wxWidgets ajuste la ventana
    CallAfter([this]() { // CallAfter para asegurar que la ventana ya se haya redimensionado completamente
        AjustarColumnasListCtrl();
        if (mainPanel) mainPanel->Layout();
        });
}

void InfoReturnsFrame::OnLoadPrev(wxCommandEvent&){
    if (currentOffset >= PAGE_SIZE) {
        currentOffset -= PAGE_SIZE;
        GetReturns(startDateTime, endDateTime, minAmount, maxAmount, worker, productName, minQuantity, maxQuantity, currentOffset);
    }
}

void InfoReturnsFrame::OnLoadNext(wxCommandEvent&) {
    currentOffset += PAGE_SIZE;
    GetReturns(startDateTime, endDateTime, minAmount, maxAmount, worker, productName, minQuantity, maxQuantity, currentOffset);
}
