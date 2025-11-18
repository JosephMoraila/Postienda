#include "gui/PreviousPurchasesFrame/PreviousPurchasesFrame.hpp"
#include <wx/clipbrd.h>
#include <wx/tipwin.h>
#include "memory.h"

void PreviousPurchaseFrame::OnSearch(wxCommandEvent& event) {
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

    std::string worker = std::string(Worker.mb_str(wxConvUTF8));
    std::string start = StartDateTime.ToStdString();
    std::string end = EndDateTime.ToStdString();
    
    //wxString msg;
    //msg.Printf("ID ingresado: %llu\n", id);
    //wxMessageBox(msg, "BÃÂÃÂºsqueda", wxOK | wxICON_INFORMATION, this);

    //When ID is not 0 means that when are searching by id only
    if (id != 0) {
        prevButton->Disable(); nextButton->Disable();
        GetPurchaseById(id);
    }
    else { 
        currentOffset = 0; //Start a new searching
        //Save the data to instance members so when OnLoadPrev or OnLoadNext are called keep searching the same data but with differente given pages from DB
        this->startDateTime = start; this->endDateTime = end; this->minAmount = minAmount; this->maxAmount = maxAmount; this->worker = worker;
        GetPurchases(start, end, minAmount, maxAmount, worker); 
    }
}

void PreviousPurchaseFrame::OnLoadPrev(wxCommandEvent&) {
    if (currentOffset >= PAGE_SIZE) {
        currentOffset -= PAGE_SIZE;
        GetPurchases(startDateTime, endDateTime, minAmount, maxAmount, worker, currentOffset);
    }
}

void PreviousPurchaseFrame::OnLoadNext(wxCommandEvent&) {
    currentOffset += PAGE_SIZE;
    GetPurchases(startDateTime, endDateTime, minAmount, maxAmount, worker, currentOffset);
}



void PreviousPurchaseFrame::OnSize(wxSizeEvent& event){
    event.Skip(); // deja que wxWidgets ajuste la ventana
    CallAfter([this]() { // CallAfter para asegurar que la ventana ya se haya redimensionado completamente
        AjustarColumnasListCtrl();
        if (mainPanel) mainPanel->Layout();
        });
}

void PreviousPurchaseFrame::OnRightClick(wxListEvent& event) {
    long index = event.GetIndex(); // fila clickeada
    if (index == -1) return;

    wxMenu menu;
    menu.Append(1, _("Copy row"));
    menu.Append(2, _("View purchase"));

    int selection = GetPopupMenuSelectionFromUser(menu);
    if (selection == 1) OnCopyToClipboardRow(index);
    if (selection == 2) OnSpecificPurchaseRow(index);
}

void PreviousPurchaseFrame::OnCopyToClipboardRow(long& index) {
    // Obtener texto de la fila
    wxString rowData;
    int colCount = list->GetColumnCount();
    for (int col = 0; col < colCount; col++)
    {
        rowData += list->GetItemText(index, col);
        if (col < colCount - 1) rowData += " | ";
    }

    // Copiar al portapapeles
    if (wxTheClipboard->Open())
    {
        wxTheClipboard->SetData(new wxTextDataObject(rowData));
        wxTheClipboard->Close();
        wxTipWindow* tip = new wxTipWindow(this, _("Copied to clipboard"));
    }
}

void PreviousPurchaseFrame::OnSpecificPurchaseRow(long& index) {
    // Obtener ID de la primera columna
    wxString idString = list->GetItemText(index, 0);
    unsigned long long idValue = 0;

    if (!idString.ToULongLong(&idValue)) {
        wxMessageBox("Invalid ID", "Invalid ID", wxOK | wxICON_ERROR, this);
        return;
    }

    // --- Revisar si ya existe una ventana para ese ID ---
    auto it = purchaseWindows.find(idValue);
    if (it != purchaseWindows.end()) {
        SpecificPurchaseInfoMenu* existing = it->second.get();
        if (existing && existing->IsShown()) {
            // Ya existe y estÃÂÃÂ¡ visible
            existing->Raise();
            existing->SetFocus();
            return;
        }
        else purchaseWindows.erase(it);// Estaba oculta o cerrada: la eliminamos
    }

    // --- Crear nueva ventana ---
    std::unique_ptr<SpecificPurchaseInfoMenu> newWindow = std::make_unique<SpecificPurchaseInfoMenu>(this, idValue);
	newWindow->AplicarTema(temaOscuro);

    // Cuando se cierre, eliminar del mapa automÃÂÃÂ¡ticamente
    newWindow->Bind(wxEVT_CLOSE_WINDOW, [this, idValue](wxCloseEvent& evt) {
        // Postergar la eliminaciÃÂÃÂ³n hasta despuÃÂÃÂ©s del evento
        this->CallAfter([this, idValue]() {purchaseWindows.erase(idValue);});
        evt.Skip(); // dejar que wxWidgets destruya la ventana normalmente
        });
    newWindow->Show(); // mostrar la ventana
    purchaseWindows[idValue] = std::move(newWindow);
}
