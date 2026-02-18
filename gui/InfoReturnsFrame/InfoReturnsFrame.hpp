#include <wx/wx.h>
#include <wx/datectrl.h>
#include <wx/dateevt.h>
#include <wx/grid.h>
#include <wx/timectrl.h>
#include <wx/listctrl.h>

class InfoReturnsFrame : public wxFrame {
public:
    InfoReturnsFrame(wxWindow* parent);
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
    wxTextCtrl* workerInput;
    wxTextCtrl* minQuantityInput;
    wxTextCtrl* maxQuantityInput;
    wxTextCtrl* productNameInput;
    wxTextCtrl* purchaseIdInput;

    wxStaticText* totaMonetReturnByFilterLabel;
    double totaMonetReturnByFilter = 0.0;

	void UpdateTotalMoneyReturnLabel();

    ///@}
    

    /// @name Funciones relacionadas con binds
    ///@{

    void OnSize(wxSizeEvent& event);
    void OnSearch(wxCommandEvent& event);
    void OnLoadPrev(wxCommandEvent&);
    void OnLoadNext(wxCommandEvent&);

    ///@}
    

    /// @name DB Search
    ///@{
    
    static constexpr int PAGE_SIZE = 50;
    size_t currentOffset = 0; /// desplazamiento actual en la base de datos
    size_t totalRecords = 0; ///Used to show how many pages has the search

    std::string startDateTime = "";
    std::string endDateTime = "";
    double minAmount = 0.0;///Used to pass it to GetPurchases when showing more pages from the same searching done in OnSearch when OnLoadPrev or OnLoadNext is called, so keep the same data but showing more pages of a searching
    double maxAmount = 0.0;///Used to pass it to GetPurchases when showing more pages from the same searching done in OnSearch when OnLoadPrev or OnLoadNext is called, so keep the same data but showing more pages of a searching
    std::string worker = "";///Used to pass it to GetPurchases when showing more pages from the same searching done in OnSearch when OnLoadPrev or OnLoadNext is called, so keep the same data but showing more pages of a searching
    std::string productName = "";
    double minQuantity = 0.0;
	double maxQuantity = 0.0;
   
    /**
	 * @brief Adds a record to the list widget
	 * \param ide ID of the return
	 * \param purchaseId ID of the purchase related to the return
	 * \param quantity of the product returned
	 * \param date of the return
	 * \param worker that made the return
	 * \param productName of the product returned
	 * \param found pointer to a size_t variable that will be set to 1 if the record was found and added to the list, or 0 if it was not found, this is used to know if the search by ID was successful or not
     */
    void AddToListFromDB(unsigned long long& ide, unsigned long long& purchaseId, double& price, double& quantity, std::string& date,std::string& worker, std::string& productName,size_t* found = nullptr);

    /**
     * @brief Search a table a record that matches with the ID of the return, calls AddToListFromDB to show the record on the list widget
	 * \param id ID of the return to search
     */
    void GetReturnById(unsigned long long& id);

    /**
	 * @brief Searches retuerned items by purchase ID, calls AddToListFromDB to show the records on the list widget
	 * @param purchaseId ID of the purchase to search returns for
	 */
	void GetReturnsByPurchaseID(unsigned long long& purchaseId);

    /**
	 * @brief Search a table for records that match with the filters, calls AddToListFromDB to show the records on the list widget, this function is used for the main search of the interface, and also for showing more pages of the same search when OnLoadPrev or OnLoadNext is called
	 * \param startDateTime in ISO format (YYYY-MM-DD HH:MM:SS)
	 * \param endDateTime in ISO format (YYYY-MM-DD HH:MM:SS)
	 * \param minAmount is the minimum total amount of the return
	 * \param maxAmount is the maximum total amount of the return
	 * \param worker is the worker that made the return, this is a text search, so it will match with any return that has a worker that contains the text input in workerInput
	 * \param productNam is the name of the product returned, this is a text search, so it will match with any return that has a product name that contains the text input in productNameInput
	 * \param minQuantity is the minimum quantity of the product returned
	 * \param maxQuantity is the maximum quantity of the product returned
	 * \param offset is the offset of the records to show, this is used for pagination, so when OnLoadPrev or OnLoadNext is called, it will show the next or previous page of the same search
	 * \param limit is the maximum number of records to show, this is used for pagination, so when OnLoadPrev or OnLoadNext is called, it will show the next or previous page of the same search
     */
    void GetReturns(std::string startDateTime, std::string endDateTime, double minAmount, double maxAmount, std::string worker, std::string productNam, double minQuantity, double maxQuantity,size_t offset = 0, size_t limit = 50);

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

    wxString GetProductName();

	double GetMinQuantity();

	double GetMaxQuantity();

    /**
        @brief  Get number from purchaseIdInput
        @retval  - If success returns the input digit id, if failure returns 0
    **/
	unsigned long long GetPurchaseID();

    ///@}
};
