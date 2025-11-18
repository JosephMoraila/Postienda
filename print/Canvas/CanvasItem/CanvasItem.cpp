#include "print/Canvas/CanvasItem/CanvasItem.hpp"

wxRect CanvasItem::GetBoundingBox(wxDC& dc) {
    if (type == TEXT || type == PURCHASE_INFO) {
        auto [textW, textH] = GetSizetext();
        textW += std::max(0, textRightWidth); //A su anchura maxima se le agrega el numero mas alto en max

        return wxRect(pos.x, pos.y, textW, textH);
    }
    else {
        return wxRect(pos.x - 15, pos.y - 15, imgWidth + 30, imgHeight + 30);
    }
}

std::pair<int, int> CanvasItem::GetSizetext(CanvasItem* selectedItem) {
    //If selectedItem is nullptr it means the functions is being call by a brother function, if it's provided means that is being called by an instance
    CanvasItem* item = selectedItem ? selectedItem : this;
    wxScreenDC dc;
    wxFont font(item->fontSize, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    dc.SetFont(font);

    wxArrayString lines = wxSplit(item->text, '\n');
    int maxWidth = 0;
    int totalHeight = 0;

    for (const wxString& line : lines) {
        wxCoord w, h;
        dc.GetTextExtent(line, &w, &h); //Tomamos las dimensiones de cada lÃÂ­nea
        if (w > maxWidth) maxWidth = w; //Si si ancho es mÃÂ¡s grande que el ancho anterior entonces el ancho anterior toma ese valor
        totalHeight += h; //Y aqui solamente inrementamos el alto
    }
    return std::make_pair(maxWidth, totalHeight);
}

std::array<wxPoint, 4> CanvasItem::GetTextVertices(CanvasItem* selectedItem)
{
    CanvasItem* item = selectedItem ? selectedItem : this;

    // Crear un contexto de dibujo consistente
    wxScreenDC screenDC;
    wxFont font(item->fontSize, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    screenDC.SetFont(font);

    // Medir el texto
    wxArrayString lines = wxSplit(item->text, '\n');
    int maxWidth = 0;
    int totalHeight = 0;

    for (const wxString& line : lines) {
        wxCoord w, h;
        screenDC.GetTextExtent(line, &w, &h);
        if (w > maxWidth) maxWidth = w;
        totalHeight += h;
    }

    // Coordenadas base (posiciÃÂ³n del texto en el canvas)
    int x1 = item->pos.x;
    int y1 = item->pos.y;
    int x2 = x1 + maxWidth;
    int y2 = y1 + totalHeight;

    // Retornar los cuatro vÃÂ©rtices (en sentido horario)
    return {
        wxPoint(x1, y1), // A - esquina superior izquierda
        wxPoint(x2, y1), // B - esquina superior derecha
        wxPoint(x1, y2), // C - esquina inferior izquierda
        wxPoint(x2, y2)  // D - esquina inferior derecha
    };
}
