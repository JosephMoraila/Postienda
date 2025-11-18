#pragma once
#include <wx/wx.h>
#include <wx/scrolwin.h>
#include <wx/bitmap.h>
#include <vector>
#include <wx/richtext/richtextctrl.h>
#include "gui/TicketFrame/Canvas/Canvas.hpp"
#include <wx/spinctrl.h> 
#include <wx/zipstrm.h>      // Para wxZipOutputStream y wxZipInputStream
#include <wx/wfstream.h>     // Para wxFileOutputStream y wxFileInputStream
#include <wx/sstream.h>      // Para wxStringOutputStream
#include <wx/mstream.h>  // Para wxMemoryInputStream
#include "print/EscPos/LineStructure/LineStructure.hpp"

class TicketFrame : public wxFrame {
public:
    TicketFrame(wxWindow* parent);
    void AplicarTema(bool oscuro);
	wxSpinCtrl* widthCtrl; ///< Control to adjust the canvas width.
	wxSpinCtrl* heightCtrl; ///< Control to adjust the canvas height.
    wxRichTextCtrl* richInput;
	int widthCanvas = 58; ///< Width of the canvas in mm
	int heightCanvas = 50; ///< Height of the canvas in mm
    bool modoEscPos = false; ///Indica si se esta en modo esc pos
    bool alreadyExistsPurchaseInfo = false; ///< Indica si ya existe un CanvasItem de tipo PURCHASE_INFO en el canvas.
    bool alreadyExistsPurchaseInfoTextInut = false; ///Indica si ya existe purchase info en el text input
    /**
    @brief Checks if the canvas has a wider than TicketFrame, so scrollbars appear
    **/
    void AdjustCanvasScroll();
    /**
        @brief Changes the layout 
    **/
    void ChangeMode();
private:
	Canvas* canvas; ///< The scrollable canvas for adding text and images.
    wxButton* buttonChangeMode;
	wxPanel* mainPanel; ///< Main panel containing all widgets.
    bool temaOscuro = false; ///< Indica si el tema oscuro estÃ¡ activo.
    void Widgets();
    /**
		@brief Maneja el evento de agregar texto al canvas. Abre un diÃ¡logo para que el usuario ingrese el texto y luego lo agrega al canvas si no estÃ¡ vacÃ­o.
		@param  - Evento de comando de wxWidgets.
    **/
    void OnAddText(wxCommandEvent&);
     /**
	 @brief Maneja el evento de agregar una imagen al canvas. Abre un diÃ¡logo de archivo para que el usuario seleccione una imagen, la carga y la agrega al canvas.
	 @param  - Evento de comando de wxWidgets.
     **/
	void OnAddImage(wxCommandEvent&);
    /**
		@brief Maneja el evento de agregar informaciÃ³n de compra al canvas. Si ya existe un CanvasItem de tipo PURCHASE_INFO, muestra un mensaje informando que solo se puede agregar una vez. Si no existe, agrega la informaciÃ³n de compra al canvas y marca que ya existe.
		@param - Evento de comando de wxWidgets.
    **/
    void OnAddPurchaseInfo(wxCommandEvent&);

    /**
     @brief When the window changes its size, OnSize calls AdjustCanvasScroll
     @param event - Window size event 
     **/
	void OnSize(wxSizeEvent& event);
    /**
		@brief Saves the current state of the canvas to a file.
		@param event - Command event triggered by the save action.
    **/
    void OnSave(wxCommandEvent& event);
    /**
	 @brief Loads a previously saved canvas state from a file.
    **/
	void OnLoad();
    /**
     @brief Opens the window of printer settings
     @param event - Event of the button when is pressed
    **/
	void OnSettings(wxCommandEvent& event);
    /**
        @brief Changes the mode to escpos or canvas and also calls ChangeMode to change the layout
        @param event - Event of the button when is pressed
    **/
    void OnChangeToMode(wxCommandEvent& event);
    /**
    @brief Saves the EscPos input items to a JSON file (Canvas save is called before so this functions loads the previous content to avoid rewrite)
    @retval  - True if it was succesful, false otherwise
    **/
    bool SaveEscPosTicket();


    /** @name UTILS
     *  Related functions to utils
     *  @{
     */
public:
    /**
    @brief Aligns content to the center
    @param richText - Rich input to align content
    **/
    void AlignCenterRichInput(wxRichTextCtrl* richText);
private:
    /**
    @brief Extracts the text and images from a rich input and returns a vector with the extracted items
    @param  richInput - Rich input to extract its items
    @retval - ContenidoLinea vector with text and images info
    **/
    std::vector<ContenidoLinea> ExtraerContenidoRichInput(wxRichTextCtrl* richInput);

    /** @} */  // Fin del grupo UTILS
};
