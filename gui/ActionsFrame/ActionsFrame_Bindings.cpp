#include "gui/ActionsFrame/ActionsFrame.hpp"
#include "utils/DateTimeUtils.hpp"

void ActionsFrame::OnSearch(wxCommandEvent& event) {
    wxString fechaInicio = GetStartDate();        // YYYY-MM-DD
    wxString inicioHora = GetTimeStart(); // HH:MM:SS
    wxString StartDateTime = fechaInicio + " " + inicioHora; // YYYY-MM-DD HH:MM:SS
    wxString endDate = GetEndDate();
    wxString endTime = GetTimeEnd();
    wxString EndDateTime = endDate + " " + endTime;
    std::string start = StartDateTime.ToStdString();
    std::string end = EndDateTime.ToStdString();

    currentOffset = 0; //Start a new searching
    this->startDateTime = start; this->endDateTime = end;

    treeDays->DeleteAllItems();
    root = treeDays->AddRoot("root");

    fechaToItem.clear();

    wxDateTime dtStart, dtEnd;
    dtStart.ParseDateTime(StartDateTime);
    dtEnd.ParseDateTime(EndDateTime);

    wxDateTime current = dtStart;
    while (current <= dtEnd) {
        wxString dia = current.Format("%Y-%m-%d");
        std::string date = dia.ToStdString();
        auto [fechaFormateada, hora] = DateTimeUtils::FormatDateTimeLocalized(date);
        wxTreeItemId item = treeDays->AppendItem(root, fechaFormateada);
        fechaToItem.emplace(date, item);
        current += wxDateSpan::Day();
    }
    GetActionsBetweenDates(start, end);
}
