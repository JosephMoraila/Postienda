#pragma once
#include <wx/wx.h>
#include <wx/string.h>
#include "gui/TicketFrame/Canvas/Canvas.hpp"
#include "print/EscPos/LineStructure/LineStructure.hpp"
#ifdef _WIN32
#include <windows.h>
#include <string>
#include <winspool.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
using namespace Gdiplus;
#endif


class PrintMenu : public wxDialog {
public:
	PrintMenu(wxWindow* parent, const wxString& title = _("Ticket Settings"));
	void AplicarTema(bool oscuro); ///< Aplica el tema claro u oscuro a la ventana

private:
	wxPanel* panel;
	wxString savedPrinter;
	int canvas_DPI_X; ///< DPI horizontal del canvas (No usado)
	int canvas_DPI_Y; ///< DPI vertical del canvas (No usado)
    wxChoice* m_printerChoice;
	wxCheckBox* checkboxPrintTicket;
	bool temaOscuro = false; ///< Indica si el tema oscuro estÃ¡ activo.
	wxArrayString printerNames; ///Printer names available in the system
	void Widgets();
	void OnTryTickets(wxCommandEvent& event);

	/** @name FILE
	 *  Funciones relacionadas con manejo de archivo
	 *  @{
	 */

	 /**
	  @brief Save the settings when the window is closed
	  @param event - Close event
	  **/
	void OnCloseSave(wxCloseEvent& event);
	/**
	 @brief Saves the selected printer
	 @param printer - Printer name
	**/
	void SaveSettingsPrinterName();
	/**
	 @brief Save the checkbox value if print when purchase
	 @param event - Checkbox event
	**/
	void SaveSettingsPrinterCheckbox(wxCommandEvent& event);
	/**
	@brief Load if print when purchase checkbox
	**/
	void LoadSettingsPrinterCheckbox();

	/** @} */  // Group end FILE

	/** @name EscPos
	 *  EscPos functions
	 *  @{
	 */

	 /**
	  @brief Gets the info from the file, prepare the esc pos commands to send them to them to SendRawDataToPrinter
	  @param printerName - Printer name
	 **/
	void PrintEscPosPrueba(const wxString& printerName);

	 /** @} */  // Group end ESCPOS


	/** @name WindowsPrint
	 *  Windows print functions
	 *  @{
	 */

#ifdef _WIN32

	/**
	 @brief Uses the Windows API to retrieve the list of installed printers and populates the printerNames array.
	 **/
	void GetPrinterNamesWindows();
	/**
     @brief Calls LoadFromFile to load the canvas info and then calls DrawTextOnHDC and DrawImageOnHDC to print the canvas items
     @param printerName - Selected printer name where items will be printed
	**/
	void ImprimirCanvasPruebaWindows(const std::wstring& printerName);
	/**
     @brief Converts a wxImage to a PNG bitmap
     @param  image - Image to convert
     @retval - PNG bitmap converted
	 **/
	HBITMAP WxImageToHBITMAPWithAlpha(const wxImage& image);
#endif
	/** @} */  // Group end WindowsPrint


#ifdef __linux__

	/** @name LinuxPrint
	 *  Linux print functions
	 *  @{
	 */

	 /**
	  * @brief Get printers linux printers, CUP, lp0 and VID.
	  * 
	  */
	void GetPrinterNamesLinux();

	 /** @} */  // Group end LinuxPrint

#endif

};