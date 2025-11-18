#include "gui/TicketFrame/Canvas/Canvas.hpp"

CanvasItem* Canvas::HitTest(const wxPoint& pos, bool checkResize, bool checkRightWidthResize) {
    wxClientDC dc(this);
    PrepareDC(dc);
    for (auto it = items.rbegin(); it != items.rend(); ++it) {
        wxRect box = it->GetBoundingBox(dc);

        if (checkResize) {
            wxRect handle = GetResizeHandle(&(*it));
            if (handle.Contains(pos)) return &(*it);
            continue;
        }

        if (checkRightWidthResize) {
            wxRect squareRect = DrawLineWithSquare(&box);
            if (squareRect.Contains(pos)) return &(*it);
            continue;
        }

        if (box.Contains(pos)) {
            return &(*it);
        }
    }
    return nullptr;
}

wxRect Canvas::DrawLineWithSquare(wxRect* box, wxPaintDC* dc, int offset, int squareSize) {
    wxPoint start(box->GetRight(), box->GetTop() + box->GetHeight() / 2);
    wxPoint end(start.x + offset, start.y);

    if (dc) {
        dc->SetPen(wxPen(*wxBLACK, 2));
        dc->SetBrush(*wxBLACK_BRUSH);
        dc->DrawLine(start, end);
        wxRect square(end.x - squareSize / 2, end.y - squareSize / 2, squareSize, squareSize);
        dc->DrawRectangle(square);
        return square;
    }
    else {
        return wxRect(end.x - squareSize / 2, end.y - squareSize / 2, squareSize, squareSize);
    }
}

wxRect Canvas::GetResizeHandle(CanvasItem* item) {
    wxClientDC dc(this);
    PrepareDC(dc);
    wxRect box = item->GetBoundingBox(dc);
    int size = 8;
    return wxRect(box.GetRight(), box.GetBottom(), size, size);
}

void Canvas::DrawFormattedText(wxDC& dc, CanvasItem* item) {
    wxArrayString lines = wxSplit(item->text, '\n');
    int currentY = item->pos.y;
    wxString formattedText; // guardará TODO el texto formateado (todas las líneas)

    for (size_t lineIndex = 0; lineIndex < lines.GetCount(); lineIndex++) {
        const wxString& line = lines[lineIndex];
        wxCoord lineW, lineH;
        dc.GetTextExtent(line, &lineW, &lineH);

        // Buscar espacios múltiples para distribuir
        wxArrayString parts;
        wxString currentPart;
        bool inSpaces = false;
        for (size_t i = 0; i < line.length(); i++) {
            wxChar c = line[i];
            if (c == ' ') {
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
        if (!currentPart.IsEmpty()) {
            parts.Add(currentPart);
        }

        // Si hay múltiples partes (columnas), distribuirlas
        if (parts.GetCount() >= 2) {
            auto [originalW, originalH] = item->GetSizetext(item);
            int totalWidth = originalW + item->textRightWidth;

            // Calcular ancho de cada parte
            wxArrayInt partWidths;
            int totalPartsWidth = 0;
            for (const wxString& part : parts) {
                wxCoord w, h;
                dc.GetTextExtent(part, &w, &h);
                partWidths.Add(w);
                totalPartsWidth += w;
            }

            int gaps = parts.GetCount() - 1;
            int gapSpace = (gaps > 0) ? ((totalWidth - totalPartsWidth) / gaps) : 0;

            int currentX = item->pos.x;
            for (size_t i = 0; i < parts.GetCount(); i++) {
                dc.DrawText(parts[i], wxPoint(currentX, currentY));

                // Construir la versión formateada de la línea
                formattedText += parts[i];
                if (i < parts.GetCount() - 1) {
                    int numSpaces = gapSpace / 5; // aproximación
                    formattedText += wxString(' ', numSpaces > 0 ? numSpaces : 1);
                }
                currentX += partWidths[i] + gapSpace;
            }
        }
        else {
            dc.DrawText(line, wxPoint(item->pos.x, currentY));
            formattedText += line;
        }

        if (lineIndex < lines.GetCount() - 1) formattedText += '\n';
        currentY += lineH;
    }

    // Asignar la versión formateada UNA sola vez (ya es wxString, no usar FromUTF8)
    item->textWithFormat = formattedText;
}

void Canvas::OnPaint(wxPaintEvent&) {
    wxPaintDC dc(this);
    PrepareDC(dc);

    for (auto& item : items) {
        if (item.type == CanvasItem::TEXT) {
            dc.SetTextForeground(item.selected ? *wxRED : *wxBLACK);
            wxFont font(item.fontSize, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
            dc.SetFont(font);
            DrawFormattedText(dc, &item);
        }
        else if (item.type == CanvasItem::IMAGE) {
            if (item.imgWidth != item.bmp.GetWidth() || item.imgHeight != item.bmp.GetHeight()) {
                wxImage tempImg = item.bmp.ConvertToImage();
                tempImg.Rescale(item.imgWidth, item.imgHeight, wxIMAGE_QUALITY_BILINEAR);
                dc.DrawBitmap(wxBitmap(tempImg), item.pos);
            }
            else {
                dc.DrawBitmap(item.bmp, item.pos);
            }
        }
        else if (item.type == CanvasItem::PURCHASE_INFO) {
            dc.SetTextForeground(*wxBLUE);
            wxFont font(item.fontSize, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
            dc.SetFont(font);
            if (item.textRightWidth > 0) DrawFormattedText(dc, &item);
            else dc.DrawText(item.text, item.pos);
        }

        if (item.selected) {
            wxRect box = item.GetBoundingBox(dc);
            dc.SetPen(*wxBLACK_PEN);
            dc.SetBrush(*wxTRANSPARENT_BRUSH);
            dc.DrawRectangle(box);

            wxRect handle = GetResizeHandle(&item);
            dc.SetBrush(*wxBLACK_BRUSH);
            dc.DrawRectangle(handle);
            DrawLineWithSquare(&box, &dc, 20);
        }
    }
}