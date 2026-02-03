#include <wx/wx.h>
#include <wx/datectrl.h>
#include <wx/dateevt.h>
#include <wx/grid.h>
#include <wx/timectrl.h>
#include <wx/listctrl.h>

class InfoProductsFrame : public wxFrame {
public:
	InfoProductsFrame(wxWindow* parent);
	void AplicarTema(bool oscuro);

private:
    void Widgets();
    void AjustarColumnasListCtrl();
	/// @name Funciones relacionadas con la interfaz principal
	///@{

	bool temaOscuro = false;
    wxScrolledWindow* scrollWidgets;
    wxPanel* bottomPanel;
    wxListCtrl* list;
    wxDatePickerCtrl* startDatePicker;
    wxTimePickerCtrl* startTimePicker;
    wxDatePickerCtrl* endDatePicker;
    wxTimePickerCtrl* endTimePicker;
    wxTextCtrl* minAmountInput;
    wxTextCtrl* maxAmountInput;
    wxTextCtrl* productNameBarcodeInput;
    wxPanel* mainPanel;
    wxButton* prevButton;
    wxButton* nextButton;
    wxStaticText* pageLabel;

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
    double GetMinPrice();
    /**
        @brief  Returns input money from maxAmountInput
        @retval  - If sucess returns input digits, if failure returns -1
    **/
    double GetMaxPrice();
    /**
        @brief  Returns text input from productNameBarcodeInput
        @retval  - Returns text input
    **/
    wxString GetNameBarcodeProduct();

    ///@}
    

    /// @name DB search
    ///@{

    size_t totalRecords = 0; ///Used to show how many pages has the search
    std::string startDateTime = ""; ///Used to pass it to GetPurchases when showing more pages from the same searching done in OnSearch when OnLoadPrev or OnLoadNext is called, so keep the same data but showing more pages of a searching
    std::string endDateTime = ""; ///Used to pass it to GetPurchases when showing more pages from the same searching done in OnSearch when OnLoadPrev or OnLoadNext is called, so keep the same data but showing more pages of a searching
    double minAmount = 0.0;///Used to pass it to GetPurchases when showing more pages from the same searching done in OnSearch when OnLoadPrev or OnLoadNext is called, so keep the same data but showing more pages of a searching
    double maxAmount = 0.0;///Used to pass it to GetPurchases when showing more pages from the same searching done in OnSearch when OnLoadPrev or OnLoadNext is called, so keep the same data but showing more pages of a searching
    std::string productNameBarcode = "";///Used to pass it to GetPurchases when showing more pages from the same searching done in OnSearch when OnLoadPrev or OnLoadNext is called, so keep the same data but showing more pages of a searching


    static constexpr int PAGE_SIZE = 50;   /// número de registros por página
    size_t currentOffset = 0;                 /// desplazamiento actual en la base de datos

    void GetSoldProducts(std::string startDateTime, std::string endDateTime, double minAmount, double maxAmount, std::string productNameBarcode, size_t offset = 0, size_t limit = 50);

    void AddToListFromDB(std::string& name, double& price, std::string& barcode, double& quantity ,size_t* found = nullptr);

    ///@}
};
