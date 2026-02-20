#pragma once
#include <wx/wx.h>
#include <wx/datectrl.h>
#include <wx/dateevt.h>
#include <wx/timectrl.h>
#include <wx/listctrl.h>
#include <wx/treectrl.h>
#include <unordered_map>

class ActionsFrame : public wxFrame {
public:
	ActionsFrame(wxWindow* parent);
	void AplicarTema(bool oscuro);

private:
	/// @name Funciones relacionadas con la interfaz principal
	///@{
	
	void Widgets();

	bool temaOscuro = false;
	wxPanel* mainPanel;
	wxPanel* buttonPanel;
	wxPanel* filtersPanel;
	wxDatePickerCtrl* startDatePicker;
	wxTimePickerCtrl* startTimePicker;
	wxDatePickerCtrl* endDatePicker;
	wxTimePickerCtrl* endTimePicker;
	wxTreeCtrl* treeDays = nullptr;
	wxTreeItemId root;
	std::unordered_map<std::string, wxTreeItemId> fechaToItem; ///Mapa inverso para obtener el treeItemId en base a su texto ya que cada texto es unico
	double totalMoneyCash = 0.0;
	double totalMoneyCard = 0.0;
	double manualAdditions = 0.0;
	double manualWithdrawals = 0.0;
	double returnsMoney = 0.0;
	double total = 0.0;
	wxStaticText* toatlMoneyCashLabel;
	wxStaticText* totalMoneyCardLabel;
	wxStaticText* manalAdditionsLabel;
	wxStaticText* manalWithdrawalsLabel;
	wxStaticText* returnsMoneyLabel;
	wxStaticText* totalLabel;

	void UpdateLabelsMoney();
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

	///@}
	

	/// @name Funciones relacionadas con binds
	///@{

	void OnSearch(wxCommandEvent& event);

	void OnPrintTicket(wxCommandEvent& event);
	std::string stringToPrint = ""; ///String that will be printed 

	///@}
	

	/// @name DB search
	///@{
	
	static constexpr int PAGE_SIZE = 5;   /// número de registros por página
	size_t currentOffset = 0;                 /// desplazamiento actual en la base de datos
	std::string startDateTime = ""; 
	std::string endDateTime = "";

	/**
	 * @brief Selecciona entre las fechas de los parametros lo que paso como devoluciones, compras e ingresos y salidas manuales de dinero en caja
	 * \param startDateTime Tiempo de inicio a checar acciones
	 * \param endDateTime Fin de tiempo de checar 
	 */
	void GetActionsBetweenDates(std::string startDateTime, std::string endDateTime);

	///@}
};
