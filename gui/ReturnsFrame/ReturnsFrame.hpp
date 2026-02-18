#pragma once
#include <wx/wx.h>
#include <wx/datectrl.h>
#include <wx/dateevt.h>
#include <wx/timectrl.h>
#include <wx/listctrl.h>
#include <wx/grid.h>
class MainFrame;

class ReturnsFrame : public wxFrame {
public:
	ReturnsFrame(wxWindow* parent, MainFrame* mainFrame);
	void AplicarTema(bool oscuro);
private:
	/// @name Funciones relacionadas con la interfaz principal
	///@{
	
	bool temaOscuro = false;
	bool isDestroying = false;
	void Widgets();
	wxPanel* mainPanel = nullptr;
	wxPanel* purchaseIdPanel = nullptr;
	wxTextCtrl* purchaseIDInput = nullptr;
	wxPanel* productsPanel = nullptr;
	wxButton* returnButton = nullptr;
	wxGrid* grid = nullptr; ///< Grid to display products
	MainFrame* mainFrame = nullptr; ///< Pointer to the main frame to access shared data and functions

	///@}
	

	/// @name Getters
	///@{

	unsigned long long GetPurchaseID();

	///@}
	

	/// @name Funciones relacionadas con binds
	///@{
	
	void OnEnterPurchaseIdInput(wxCommandEvent& event);
	void OnButtonValidateClick(wxCommandEvent& event);

	/**
	 * @brief Takes the data from the grid and converts to call other functions and asks user if they want to return the product, if they confirm it calls the function to return the product.
	 * \param event The event triggered by clicking the return button.
	 */
	void OnReturnProduct(wxCommandEvent& event);
	void OnClose(wxCloseEvent& event);
	void OnSize(wxSizeEvent& event);
	

	///@}


	/// @name Funciones relacionadas con base de datos
	///@{
	
	unsigned long long purchaseID = 0;
	
	void ValidatePurchaseId();

	/**
	 * @brief Checks if the given purchase ID exists in the database.
	 * \param purchaseId The purchase ID to check.
	 * \return True if the purchase ID exists in the database, false otherwise.
	 */
	bool ValidateIfPurchaseIdExistsTable(unsigned long long& purchaseId);

	void GetProductsByPurchaseId(unsigned long long& purchaseId);

	bool ValidateProductID(unsigned long long& productId);

	void ReturnProductStock(unsigned long long& productId, double& quantity, unsigned long long& purchaseItemId, wxString& productName);

	bool ValidateIfProductIsAlreadyReturned(unsigned long long& purchaseItemId);

	void CreateTableReturns();

	///@}
};
