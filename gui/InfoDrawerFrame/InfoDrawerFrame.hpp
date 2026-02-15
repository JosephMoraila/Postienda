#include <wx/wx.h>
#include <wx/datectrl.h>
#include <wx/dateevt.h>
#include <wx/grid.h>
#include <wx/timectrl.h>
#include <wx/listctrl.h>

class InfoDrawerFrame : public wxFrame {
public:
    InfoDrawerFrame(wxWindow* parent);
    void AplicarTema(bool oscuro);

private:
    void Widgets();
    void AjustarColumnasListCtrl();

    /// @name Funciones relacionadas con la interfaz principal
    ///@{

    bool temaOscuro = false;
    wxPanel* mainPanel;
    wxScrolledWindow* scrollWidgets;
    wxPanel* bottomPanel;
    wxListCtrl* list;
    wxTextCtrl* IdInput;
    wxDatePickerCtrl* startDatePicker;
    wxTimePickerCtrl* startTimePicker;
    wxDatePickerCtrl* endDatePicker;
    wxTimePickerCtrl* endTimePicker;
    wxTextCtrl* minAmountInput;
    wxTextCtrl* maxAmountInput;
    wxButton* prevButton;
    wxButton* nextButton;
    wxStaticText* pageLabel;
    wxChoice* actionChoice;
    wxTextCtrl* workerInput;
    wxTextCtrl* reasonInput;
    wxTextCtrl* purchaseIdInput;
    wxTextCtrl* drawerAfterMinAmountInput;
    wxTextCtrl* drawerAfterMaxAmountInput;
    enum DrawerAction {
        DRAWER_ANY,
        DRAWER_WITHDRAWL,
		DRAWER_ADDITION
    };

    ///@}


    /// @name Funciones relacionadas con binds
    ///@{

    void OnSize(wxSizeEvent& event);
    void OnSearch(wxCommandEvent& event);
    void OnLoadPrev(wxCommandEvent&);
    void OnLoadNext(wxCommandEvent&);

    ///@}


    /// @name Widgets Getters
    ///@{

    /**
        @brief  Get number from IdInput
        @retval  - If success returns the input digit id, if failure returns 0
    **/
    unsigned long long GetID();
    /**
        @brief  Get date from startDatePicker
        @retval  - Returns the start date with ISO format (YYYY-MM-DD)
    **/
    wxString GetStartDate();
    /**
        @brief  Returns time from startTimePicker
        @retval  - Returns the start time with ISO format HH:MM:SS
    **/
    wxString GetTimeStart();
    /**
    @brief  Get date from endDatePicker
    @retval  - Returns the start date with ISO format (YYYY-MM-DD)
    **/
    wxString GetEndDate();
    /**
        @brief  Returns time from endTimePicker
        @retval  - Returns the start time with ISO format HH:MM:SS
    **/
    wxString GetTimeEnd();
    /**
        @brief  Returns input money from minAmountInput
        @retval  - If sucess returns input digits, if failure returns -1
    **/
    double GetMinAmount();
    /**
        @brief  Returns input money from maxAmountInput
        @retval  - If sucess returns input digits, if failure returns -1
    **/
    double GetMaxAmount();
    /**
        @brief  Returns text input from workerInput
        @retval  - Returns text input
    **/
    wxString GetWorker();

	wxString GetReason();

    unsigned long long GetPurchaseID();

	double GetDrawerAfterMinAmount();

	double GetDrawerAfterMaxAmount();

    ///@}
};
