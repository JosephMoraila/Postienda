#include "gui/InfoProductsFrame/InfoProductsFrame.hpp"

void InfoProductsFrame::OnSize(wxSizeEvent& event) {
    event.Skip(); // deja que wxWidgets ajuste la ventana
    CallAfter([this]() { // CallAfter para asegurar que la ventana ya se haya redimensionado completamente
        AjustarColumnasListCtrl();
        if (mainPanel) mainPanel->Layout();
        });
}


void InfoProductsFrame::OnSearch(wxCommandEvent& event) {
    wxString fechaInicio = GetStartDate();        // YYYY-MM-DD
    wxString inicioHora = GetTimeStart(); // HH:MM:SS
    wxString StartDateTime = fechaInicio + " " + inicioHora; // YYYY-MM-DD HH:MM:SS
    wxString endDate = GetEndDate();
    wxString endTime = GetTimeEnd();
    wxString EndDateTime = endDate + " " + endTime;
    double minAmount = GetMinPrice();
    double maxAmount = GetMaxPrice();
    wxString productNameBarcode = GetNameBarcodeProduct();
    bool byDesc = this->descQuantityCheckbox->IsChecked();

    std::string productNameBarcodeStr = std::string(productNameBarcode.mb_str(wxConvUTF8));
    std::string start = StartDateTime.ToStdString();
    std::string end = EndDateTime.ToStdString();

    currentOffset = 0; //Start a new searching
    //Save the data to instance members so when OnLoadPrev or OnLoadNext are called keep searching the same data but with differente given pages from DB
    this->startDateTime = start; this->endDateTime = end; this->minAmount = minAmount; this->maxAmount = maxAmount; this->productNameBarcode = productNameBarcode; this->byDesc = byDesc;
    GetSoldProducts(start, end, minAmount, maxAmount, productNameBarcodeStr, byDesc);
    
}


void InfoProductsFrame::OnLoadPrev(wxCommandEvent&) {
    if (currentOffset >= PAGE_SIZE) {
        currentOffset -= PAGE_SIZE;
        GetSoldProducts(startDateTime, endDateTime, minAmount, maxAmount, productNameBarcode, byDesc, currentOffset);
    }
}

void InfoProductsFrame::OnLoadNext(wxCommandEvent&) {
    currentOffset += PAGE_SIZE;
    GetSoldProducts(startDateTime, endDateTime, minAmount, maxAmount, productNameBarcode, byDesc, currentOffset);
}