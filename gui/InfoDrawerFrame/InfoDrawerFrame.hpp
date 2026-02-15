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
    wxStaticText* totalIncomeByFilterLabel;
    wxStaticText* totalWithdrawalByFilterLabel;
	wxStaticText* totalOnDrawerByFilterLabel; ///Shows the total amount of money that should be on the drawer based on the search done, so it adds total income and subtracts total withdraws, showing if the drawer should have more or less money than the actual amount on drawer, so the user can know if there is a drawer discrepancy without doing the math by himself
    double totalIncomeByFilter = 0.0;
    double totalWithdrawalByFilter = 0.0;
    double totalOnDrawerByFilter = 0.0;

	void UpdateTotalLabels();

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


    /// @name DB search
    
    size_t totalRecords = 0; ///Used to show how many pages has the search
    std::string startDateTime = "";
    std::string endDateTime = "";
    double minAmount = 0.0;///Used to pass it to GetPurchases when showing more pages from the same searching done in OnSearch when OnLoadPrev or OnLoadNext is called, so keep the same data but showing more pages of a searching
    double maxAmount = 0.0;///Used to pass it to GetPurchases when showing more pages from the same searching done in OnSearch when OnLoadPrev or OnLoadNext is called, so keep the same data but showing more pages of a searching
    std::string worker = "";///Used to pass it to GetPurchases when showing more pages from the same searching done in OnSearch when OnLoadPrev or OnLoadNext is called, so keep the same data but showing more pages of a searching
	DrawerAction lastDrawerAction = DrawerAction::DRAWER_ANY; ///Used to pass it to GetPurchases when showing more pages from the same searching done in OnSearch when OnLoadPrev or OnLoadNext is called, so keep the same data but showing more pages of a searching
    std::string reason = "";
    double drawerAfterMinAmount = 0.0;
    double drawerAfterMaxAmount = 0.0;

    static constexpr int PAGE_SIZE = 50;
    size_t currentOffset = 0; /// desplazamiento actual en la base de datos

    /**
        @brief Shows the given data on the list widget
        @param ide    - Id to display
        @param date   - date to display (Formatted to local language)
        @param total  - Total to display
        @param worker - Worker to display
        @param found  - (Optional). Increases the given parameter adding 1
    **/
    void AddToListFromDB(unsigned long long& ide, std::string& date, double& amount, std::string& worker, bool& isAddition, std::string reason, unsigned long long& purchaseId, double& drawerAfterMinAmount,size_t* found = nullptr);

    /**
    @brief Searchs in table the record that matches the ID, calls AddToListFromDB to show the record on the list widget
    @param id - Id to search
    **/
    void GetDrawerHistoryById(unsigned long long& id);

    void GetDrawerHistoryByPurchaseId(unsigned long long& id);

    /**
        @brief Check if the doubles are not -1 and worker is not empty, so the parameter is searched. Shows multiple records based on parameters
        @param startDateTime - Start time to search
        @param endDateTime   - End time to search
        @param minAmount     - If it's -1 it doesn't consider searching minAmount
        @param maxAmount     - If it's -1 it doesn't consider searching maxAmount
        @param worker        - If it's empty it doesn't consider searching worker
		@param action        - If it's DRAWER_ANY it doesn't consider searching action, if it's DRAWER_WITHDRAWL it searches only withdraws, if it's DRAWER_ADDITION it searches only additions
		@param reason        - If it's empty it doesn't consider searching reason
		@param drawerAfterMinAmount - If it's -1 it doesn't consider searching drawerAfterMinAmount
		@param drawerAfterMaxAmount - If it's -1 it doesn't consider searching drawerAfterMax
        @param offset        - From that offset number record start searching
        @param limit         - Records limit to show
    **/
    void GetDrawerHistories(std::string startDateTime, std::string endDateTime, double minAmount, double maxAmount, std::string worker, DrawerAction action, std::string reason,double drawerAfterMinAmount, double drawerAfterMaxAmount, size_t offset = 0, size_t limit = 50);

    ///@{
};
