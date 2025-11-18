#include <wx/wx.h>
#include <wx/listctrl.h>
#include "third_party/SQLite/sqlite3/sqlite_modern_cpp.h"
#include "gui/MainFrame/Purchase_Item.hpp"

class SpecificPurchaseInfoMenu : public wxFrame {
public:
	 /**
     @brief SpecificPurchaseInfoMenu object constructor
	 @param parent - Pointer to the parent window
	 @param id - The unique identifier of the purchase to display
	 **/
	SpecificPurchaseInfoMenu(wxWindow* parent, unsigned long long id);
	void AplicarTema(bool oscuro);
private:
	void Widgets();
	/// @name Funciones relacionadas con la interfaz principal
	///@{
	
	unsigned long long m_purchaseId = 0; /// The unique identifier of the purchase
	bool temaOscuro = false;
	wxPanel* mainPanel = nullptr;
	wxPanel* buttonPanel = nullptr;
	wxListCtrl* list = nullptr;
	wxStaticText* labelTotal = nullptr; ///Label that shows total price
	double totalUI = 0.0; ///> Total price shown in labelTotal
	/**
	 @brief Adjust the widths of the columns in the list control based on the current size of the window.
	**/
	void AjustarColumnasListCtrl();
	///@}

	/// @name Bind functions
	///@{
	
	/**
	 @brief Handles the size event to adjust the list control columns when the window is resized.
	 @param event - The size event.
	**/
	void OnSize(wxSizeEvent& event);
	/**
	 @brief Handles the print ticket button click event to print the purchase ticket.
	 @detail Retrieves from DB the prodcut name, id, and price at purchase where the purchase ID matches m_purchaseId, then fetches with barcodes if they exist, and sends the data to the printing function.
	 @param event - The command event.
	**/
	void OnPrintTicketButtonClicked(wxCommandEvent& event);
	
	///@}

	/// @name DB functions 
	///@{
	
	/**
	 @brief Loads the details of a specific purchase by its ID and populates the list control with the purchase items.
	 @param id - The unique identifier of the purchase to load.
	**/
	void LoadPurchaseDetails(unsigned long long id);
	/**
	 @brief Retrieves the barcode for a given product ID from the database, if it exists.
	 @param  db - Reference to the SQLite database connection
	 @param  productId - The unique identifier of the product
	 @retval - The barcode as a string if it exists, otherwise an empty string
	**/
	std::string GetBarcodeIfExists(sqlite::database& db, unsigned long long productId);
	/**
	 @brief Retrieves the worker's name and total purchase ID associated with the current purchase.
	 @retval  - A tuple containing the worker's name, total purchase ID as a double and the purchase datetime
	 **/
	std::tuple<wxString, double, std::string> GetWorkerTotalDateTimePurchaseID();

	///@}


	/// @name PrintTicket Canvas functions 
	///@{

#ifdef _WIN32
	/**
	 @brief Prints the purchase ticket using Canvas on Windows.
	 @param items - A vector of PurchaseItem objects representing the items in the purchase
	 **/
	void PrintCanvasPurchaseWindows(std::vector<PurchaseItem>& items);
#endif
	 /**
	 @brief Creates a formatted string for printing the purchase ticket products data.
	 @param  PurchaseItems - A vector of PurchaseItem objects representing the items products in the purchase
	 @retval - A formatted wxString ready for printing
	**/
	wxString CreateInfoPurchaseStringToPrint(std::vector<PurchaseItem>& PurchaseItems);

	///@}


	/// @name PrintTicket EscPos functions 
	///@{
	
	 /**
	 @brief Prints the purchase ticket using ESC/POS commands.
	 @param PurchaseItems - A vector of PurchaseItem objects representing the items in the purchase
	**/
	void PrintEscposPurchase(std::vector<PurchaseItem>& PurchaseItems);
	
	///@}
};