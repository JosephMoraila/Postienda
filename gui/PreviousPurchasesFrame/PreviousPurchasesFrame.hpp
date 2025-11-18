#include <wx/wx.h>
#include <wx/datectrl.h>
#include <wx/dateevt.h>
#include <wx/grid.h>
#include <wx/timectrl.h>
#include <wx/listctrl.h>
#include "gui/PreviousPurchasesFrame/SpecificPurchaseInfoMenu/SpecificPurchaseInfoMenu.hpp"

class PreviousPurchaseFrame : public wxFrame {
public:
	PreviousPurchaseFrame(wxWindow* parent);
    /**
     * @brief Applies light or dark theme to the window and its child widgets.
     * @param oscuro true to enable dark theme, false for light theme.
     */
    void AplicarTema(bool oscuro);
	std::unordered_map<unsigned long long, std::unique_ptr<SpecificPurchaseInfoMenu>> purchaseWindows;///Unordered map to keep track of open SpecificPurchaseInfoMenu windows by purchase ID
private:
    /// @name Funciones relacionadas con la interfaz principal
    ///@{
    bool temaOscuro = false;
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
    wxTextCtrl* workerInput;
    wxPanel* mainPanel;
    wxButton* prevButton;
    wxButton* nextButton;
    wxStaticText* pageLabel;
    void Widgets();
    /**
    @brief Adjust the width of the columns of the control list when the windows changes its size
    **/
    void AjustarColumnasListCtrl();

    ///@}

    /// @name Funciones relacionadas con binds
    ///@{
        
    /**
        @brief Calls the getters. If ID is 0 calls GetPurchaseById, otherwise GetPurchases
        @param event - Event when button is pressed
    **/
    void OnSearch(wxCommandEvent& event);
    /**
        @brief Verifies if currentOffset is less than or equal to 50 (Programmed offset 'PAGE_SIZE'). If true subtracts 50, then calls GetPurchases with the saved searchong data and with the new offset
        @param  - Event when button is pressed
    **/
    void OnLoadPrev(wxCommandEvent&);
    /**
        @brief Adds 50 to currentOffset, then calls GetPurchases with the saved searchong data and with the new offset
        @param  - Event when button is pressed 
    **/
    void OnLoadNext(wxCommandEvent&);
    /**
     * @brief Evento al cambiar el tamaÃ±o de la ventana redimenciona la lista
     * @param event Evento de tamaÃ±o.
     */
    void OnSize(wxSizeEvent& event);

    void OnRightClick(wxListEvent& event);
    /**
        @brief Copy the selected row info to the clipboard
        @param index - Selected row
    **/
    void OnCopyToClipboardRow(long& index);
    /**
        @brief Opens a SpecificPurchaseInfoMenu window showing the details of the selected purchase
		@param index - Selected row
	**/
    void OnSpecificPurchaseRow(long& index);

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

    ///@}


    /// @name DB search
    ///@{
    size_t totalRecords = 0; ///Used to show how many pages has the search
    std::string startDateTime = ""; ///Used to pass it to GetPurchases when showing more pages from the same searching done in OnSearch when OnLoadPrev or OnLoadNext is called, so keep the same data but showing more pages of a searching
    std::string endDateTime = ""; ///Used to pass it to GetPurchases when showing more pages from the same searching done in OnSearch when OnLoadPrev or OnLoadNext is called, so keep the same data but showing more pages of a searching
    double minAmount = 0.0;///Used to pass it to GetPurchases when showing more pages from the same searching done in OnSearch when OnLoadPrev or OnLoadNext is called, so keep the same data but showing more pages of a searching
    double maxAmount = 0.0;///Used to pass it to GetPurchases when showing more pages from the same searching done in OnSearch when OnLoadPrev or OnLoadNext is called, so keep the same data but showing more pages of a searching
    std::string worker = "";///Used to pass it to GetPurchases when showing more pages from the same searching done in OnSearch when OnLoadPrev or OnLoadNext is called, so keep the same data but showing more pages of a searching

    static constexpr int PAGE_SIZE = 50;   /// nÃºmero de registros por pÃ¡gina
    size_t currentOffset = 0;                 /// desplazamiento actual en la base de datos
    
    /**
        @brief Shows the given data on the list widget
        @param ide    - Id to display
        @param date   - date to display (Formatted to local language)
        @param total  - Total to display
        @param worker - Worker to display
        @param found  - (Optional). Increases the given parameter adding 1
    **/
    void AddToListFromDB(unsigned long long& ide, std::string& date, double& total, std::string& worker, size_t* found = nullptr);
    /**
        @brief Searchs in table the record that matches the ID, calls AddToListFromDB to show the record on the list widget
        @param id - Id to search
    **/
    void GetPurchaseById(unsigned long long& id);
    /**
        @brief Check if the doubles are not -1 and worker is not empty, so the parameter is searched. Shows multiple records based on parameters
        @param startDateTime - Start time to search
        @param endDateTime   - End time to search
        @param minAmount     - If it's -1 it doesn't consider searching minAmount
        @param maxAmount     - If it's -1 it doesn't consider searching maxAmount
        @param worker        - If it's empty it doesn't consider searching worker
        @param offset        - From that offset number record start searching
        @param limit         - Records limit to show
    **/
    void GetPurchases(std::string startDateTime, std::string endDateTime, double minAmount, double maxAmount, std::string worker, size_t offset = 0, size_t limit = 50);

    ///@}
};