#include "gui/InfoDrawerFrame/InfoDrawerFrame.hpp"

void InfoDrawerFrame::OnSearch(wxCommandEvent& event) {
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
    wxString Reason = GetReason();
    unsigned long long purchaseId = GetPurchaseID();
    double drawerAfterMinAmount = GetDrawerAfterMinAmount();
    double drawerAfterMaxAmount = GetDrawerAfterMaxAmount();

    DrawerAction action = DRAWER_ANY;
    int selection = actionChoice->GetSelection();
    if (selection != wxNOT_FOUND) action = (DrawerAction)(intptr_t)actionChoice->GetClientData(selection);

    std::string reason = std::string(Reason.mb_str(wxConvUTF8));
    std::string worker = std::string(Worker.mb_str(wxConvUTF8));
    std::string start = StartDateTime.ToStdString();
    std::string end = EndDateTime.ToStdString();

    //When ID is not 0 means that when are searching by id only
    if (id != 0) {
        prevButton->Disable(); nextButton->Disable();
        GetDrawerHistoryById(id);
    }
    else if (id == 0 && purchaseId != 0) { //Se busca solo por purchaseId
        prevButton->Disable(); nextButton->Disable();
        GetDrawerHistoryByPurchaseId(purchaseId);
    }
    else {
        currentOffset = 0; //Start a new searching
        //Save the data to instance members so when OnLoadPrev or OnLoadNext are called keep searching the same data but with differente given pages from DB
		this->startDateTime = start; this->endDateTime = end; this->minAmount = minAmount; this->maxAmount = maxAmount; this->worker = worker; this->lastDrawerAction = action; this->reason = reason; this->drawerAfterMinAmount = drawerAfterMinAmount; this->drawerAfterMaxAmount = drawerAfterMaxAmount;
        GetDrawerHistories(start, end, minAmount, maxAmount, worker, action, reason, drawerAfterMinAmount, drawerAfterMaxAmount);
        //GetTotalByFilter(start, end, minAmount, maxAmount, worker, method);
    }
}

void InfoDrawerFrame::OnLoadPrev(wxCommandEvent&) {
    if (currentOffset >= PAGE_SIZE) {
        currentOffset -= PAGE_SIZE;
        GetDrawerHistories(startDateTime, endDateTime, minAmount, maxAmount, worker, lastDrawerAction,reason, drawerAfterMinAmount, drawerAfterMaxAmount,currentOffset);
    }
}

void InfoDrawerFrame::OnLoadNext(wxCommandEvent&) {
    currentOffset += PAGE_SIZE;
    GetDrawerHistories(startDateTime, endDateTime, minAmount, maxAmount, worker, lastDrawerAction, reason, drawerAfterMinAmount, drawerAfterMaxAmount, currentOffset);
}

void InfoDrawerFrame::OnSize(wxSizeEvent& event) {
    event.Skip(); // deja que wxWidgets ajuste la ventana
    CallAfter([this]() { // CallAfter para asegurar que la ventana ya se haya redimensionado completamente
        AjustarColumnasListCtrl();
        if (mainPanel) mainPanel->Layout();
        });
}
