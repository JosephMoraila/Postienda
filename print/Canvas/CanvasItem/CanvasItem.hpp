#pragma once
#include <wx/wx.h>
#include <array>

/**
    @struct CanvasItem
    @brief Represents an item on the canvas, which can be either text, an image or the purchase info.
**/
struct CanvasItem {
    //Type significa que es un nuevo tipo de dato llamado Type. Luego se define un enum con dos posibles valores: TEXT e IMAGE. y type es una variable de ese tipo enum.
    enum Type { TEXT, IMAGE, PURCHASE_INFO } type; /// Type of the item (text or image). 
    wxString text; /// Original Text content if the item is of type TEXT.
    wxString textWithFormat; ///Text with the formatting text to be displayed
    wxBitmap bmp;
    wxPoint pos; /// Position of the item on the canvas.
    bool underInfo = false; /// Only for text: if true, the text will be positioned under the purchase info.
    int fontSize = 16;
    int imgWidth = 100;
    int imgHeight = 100;
    wxImage originalImage; // Imagen original sin escalar
    bool selected = false;
    int textRightWidth = 0;  ///Width after the right side of the item

    /**
    @brief Esta funciÃ³n obtiene el cuadro delimitador (bounding box) de un CanvasItem. (texto o imagen)
    @param  dc - Contexto de dispositivo utilizado para medir el texto. Contexto es un objeto que encapsula la informaciÃ³n necesaria para dibujar en una superficie.
    @retval - El cuadro delimitador del item.
    **/
    wxRect GetBoundingBox(wxDC& dc);

    /**
        @brief Calculates the width and height of a text
        @param  selectedItem - If selectedItem is nullptr it means the functions is being call by a another function, if it's provided means that is being called by an instance
        @retval - The width and height in that order
    **/
    std::pair<int, int> GetSizetext(CanvasItem* selectedItem = nullptr);

    /**
        @brief Returns the corners of a text
        @param  selectedItem - Item to be found the corners
        @retval - An array of positions of the four corners
    **/
    std::array<wxPoint, 4> GetTextVertices(CanvasItem* selectedItem);
};

