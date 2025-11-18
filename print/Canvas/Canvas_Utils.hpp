#include <wx/wx.h>
#include "print/Canvas/CanvasItem/CanvasItem.hpp"

/**
    @namespace Canvas_Utils
    @brief 
**/
namespace Canvas_Utils{

    /**
    @brief  Same as DrawFormattedText but adaptted for real purchase info when text has a format (used in purchase info only)
    @param  products - Products text
    @param  item - Item (purchase info item) that contains the meta data
    @retval - Formatted text
    **/
    inline wxString FormatProductsLikeOriginal(const wxString& products, CanvasItem& item) {
        wxArrayString lines = wxSplit(products, '\n');
        wxString formattedText;

        // Crear DC para medir texto
        wxScreenDC dc;
        wxFont font(item.fontSize, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
        dc.SetFont(font);

        for (size_t lineIndex = 0; lineIndex < lines.GetCount(); lineIndex++) {
            const wxString& line = lines[lineIndex];

            wxCoord lineW, lineH;
            dc.GetTextExtent(line, &lineW, &lineH);

            // Buscar espacios para distribuir (IGUAL QUE DrawFormattedText)
            wxArrayString parts;
            wxString currentPart;
            bool inSpaces = false;

            for (size_t i = 0; i < line.length(); i++) {
                wxChar c = line[i];
                if (c == ' ') {
                    // Cuando encuentra un espacio Y ya hay texto acumulado, guarda la parte
                    if (!inSpaces && !currentPart.IsEmpty()) {
                        parts.Add(currentPart);
                        currentPart.Clear();
                    }
                    inSpaces = true;
                }
                else {
                    inSpaces = false;
                    currentPart += c;
                }
            }

            // Agregar la Ãºltima parte
            if (!currentPart.IsEmpty()) {
                parts.Add(currentPart);
            }

            // Si hay mÃºltiples partes (columnas), distribuirlas
            if (parts.GetCount() >= 2) {
                auto [originalW, originalH] = item.GetSizetext(&item);
                int totalWidth = originalW + item.textRightWidth;

                // Calcular ancho de cada parte
                wxArrayInt partWidths;
                int totalPartsWidth = 0;

                for (const wxString& part : parts) {
                    wxCoord w, h;
                    dc.GetTextExtent(part, &w, &h);
                    partWidths.Add(w);
                    totalPartsWidth += w;
                }

                // Calcular espaciado entre partes
                int gaps = parts.GetCount() - 1;
                int gapSpace = (totalWidth - totalPartsWidth) / gaps;

                // Construir el texto formateado con espacios
                for (size_t i = 0; i < parts.GetCount(); i++) {
                    formattedText += parts[i];

                    if (i < parts.GetCount() - 1) {
                        // Agregar espacios proporcionales al gap (IGUAL QUE DrawFormattedText)
                        int numSpaces = gapSpace / 5; // AproximaciÃ³n (depende del ancho de espacio)
                        formattedText += wxString(' ', numSpaces > 0 ? numSpaces : 1);
                    }
                }
            }
            else {
                // Sin columnas
                formattedText += line;
            }

            // Agregar salto de lÃ­nea si no es la Ãºltima lÃ­nea
            if (lineIndex < lines.GetCount() - 1) {
                formattedText += '\n';
            }
        }

        return formattedText;
    }

    /**
    @brief  Used to avoid under info text above purchase info text
    @param  text - Purchase info text
    @param  fontSize - Purchase info font
    @retval - Purchase info height
    **/
    inline int CalculateTextHeight(const wxString& text, int fontSize) {
        wxScreenDC dc;
        wxFont font(fontSize, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
        dc.SetFont(font);

        wxCoord width, height;
        dc.GetMultiLineTextExtent(text, &width, &height);

        return height;
    }


}