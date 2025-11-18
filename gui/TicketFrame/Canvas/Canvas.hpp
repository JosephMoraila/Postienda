#pragma once
#include <wx/wx.h>
#include <wx/scrolwin.h>
#include "gui/TicketFrame/Canvas/InputTextModal/InputTextModal.hpp"
#include "print/Canvas/CanvasItem/CanvasItem.hpp"
class TicketFrame; /// Forward declaration to avoid circular dependency.

/**

    @class   Canvas
	@brief A scrollable canvas for adding and manipulating text and images.
    @details - 

**/
class Canvas : public wxScrolledWindow {
public:
    /**
        @brief Canvas object constructor
		@param parentFrame - The parent TicketFrame that contains this canvas. So we can access its members.
		@param parent - The parent wxWindow for this canvas (panel)
    **/
    Canvas(TicketFrame* parentFrame, wxWindow* parent);
    /**
		@brief Adds a text item to the canvas at a default position.
		@param text - The text content to add to the canvas.
    **/
    void AddText(const wxString& text, bool isUnderInfo);
    /**
		@brief Adds an image item to the canvas at a default position.
		@param bmp - The bitmap image to add to the canvas.
    **/
    void AddImage(const wxBitmap& bmp);
    /**
		@brief Adds a purchase info item to the canvas at a default position.
    **/
    void AddPurchaseInfo();
    bool temaOscuro = false;
    std::vector<CanvasItem> items; /// List of items on the canvas.
    /**
		@brief Saves the current canvas items to a file.
		@retval - True if the save was successful, false otherwise.
    **/
    bool SaveToFile();
    /**
		@brief Loads canvas and esc pos items from a file.
		@retval - True if the load was successful, false otherwise.
    **/
    bool LoadFromFile();
	TicketFrame* parentFrame; /// Pointer to the parent TicketFrame to access its members.
private:
	CanvasItem* selectedItem = nullptr; /// Currently selected item.
	bool resizing = false; /// True if currently resizing an item.
	bool rightWidthResizing = false;
    int resizeStartOffset = 0; ///Guarda donde se hace click en relacion al borde
	wxPoint dragOffset; /// Offset es la diferencia entre la posicion del mouse y la posicion del item seleccionado.

    /** @name DRAWING
     *  Funciones relacionadas con el dibujo en el canvas.
     *  @{
     */

    /**
    @brief Esta funciÃ³n verifica si una posiciÃ³n dada (pos) golpea algÃºn item en el canvas.
    @param  pos - La posiciÃ³n a verificar.
    @param  checkResize - Si es true, la funciÃ³n primero verifica si la posiciÃ³n golpea el manejador de redimensionamiento (un pequeÃ±o cuadro en la esquina inferior derecha del item) antes de verificar si golpea el item en sÃ­.
    @retval - Un puntero al CanvasItem que fue golpeado, o nullptr si no se golpeÃ³ ningÃºn item.
    **/
    CanvasItem* HitTest(const wxPoint& pos, bool checkResize = false, bool checkRightWidthResize = false);
    /**
    @brief Esta funciÃ³n obtiene el rectÃ¡ngulo del "handle" de redimensionamiento para un item dado. Es el cuadro pequeÃ±o en la esquina inferior derecha del item que permite redimensionarlo.
    @param  item - El item del cual se quiere obtener el rectÃ¡ngulo del manejador de redimensionamiento.
    @retval - El rectÃ¡ngulo del manejador de redimensionamiento.
    **/
    wxRect GetResizeHandle(CanvasItem* item);
    /**
    @brief Maneja el evento de pintura para dibujar los items en el canvas. Pintar es cuando se debe repintar la ventana.
    @param - El evento de pintura (no se usa en este caso).
    **/
    void OnPaint(wxPaintEvent& event);
    /**
    @brief Draws the text with formatted text ans saves it to item->textWithFormat to be saved
    @param dc - Device context to draw
    @param item - Item like text or purchase info to be formatted
    **/
    void DrawFormattedText(wxDC& dc, CanvasItem* item);
    /**
    @brief If DC is provided reurns the line with a square to be drawn, if not,returns the info of the square
    @param  box - Boundung box of the selectem item
    @param  dc - Device context where it will be draw
    @param  offset - Line size
    @param  squareSize - Square size
    @retval Line and square to be drawn if DC provided, otherwise info of the square
    **/
    wxRect DrawLineWithSquare(wxRect* box, wxPaintDC* dc = nullptr, int offset = 20, int squareSize = 8);

    /** @} */  // Fin del grupo DRAWING

     /** @name BINDS
     *  Funciones relacionadas con el binds en el canvas.
     *  @{
     */

    /**
		@brief Si el mouse se presiona sobre un item, selecciona ese item para arrastrarlo o redimensionarlo.
		@param event - The mouse event containing the position and state of the mouse.
    **/
    void OnLeftDown(wxMouseEvent& event);
    /**
        @brief Maneja el evento cuando se suelta el boton izquierdo del mouse. Deselecciona el item seleccionado y finaliza el redimensionamiento.
        @param event - El evento del mouse que contiene la posicion y el estado del mouse.
    **/
    void OnLeftUp(wxMouseEvent& event);
    /**
		@brief Handles mouse move events for dragging and resizing items on the canvas.
		@param event - The mouse event containing the position and state of the mouse.
    **/
    void OnMouseMove(wxMouseEvent& event);
    /**
		@brief Handles double-click events to edit text items.
		@param event - The mouse event containing the position and state of the mouse.
    **/
    void OnDoubleLeftClick(wxMouseEvent& event);
    /**
		@brief Handles key down events for deleting selected items.
		@param evt - The key event containing information about the key press.
    **/
    void OnKeyDown(wxKeyEvent& evt);
    /**
		@brief Updates the tooltip based on the current mouse position.
		@param pos - The current mouse position.
    **/
    void UpdateTooltip(const wxPoint& pos);

    /** @} */  // Fin del grupo BINDS

    wxDECLARE_EVENT_TABLE();
};
