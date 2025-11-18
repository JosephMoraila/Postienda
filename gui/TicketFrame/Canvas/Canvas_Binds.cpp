#include "gui/TicketFrame/Canvas/Canvas.hpp"
#include "gui/TicketFrame/TicketFrame.hpp"


void Canvas::OnLeftDown(wxMouseEvent& evt) {
    SetFocus();
    wxPoint pos = evt.GetPosition();
    CalcUnscrolledPosition(pos.x, pos.y, &pos.x, &pos.y);

    resizing = false;
    rightWidthResizing = false;
    selectedItem = nullptr;
    resizeStartOffset = 0;

    // Primero desmarcar todos
    for (auto& item : items) item.selected = false;

    // 1ÃÂ¯ÃÂ¸ÃÂÃÂ¢ÃÂÃÂ£ Resize handles
    CanvasItem* handleItem = HitTest(pos, true);
    if (handleItem) {
        selectedItem = handleItem;
        selectedItem->selected = true;
        resizing = true;

        // Guardar el offset inicial basado en el tipo
        if (selectedItem->type == CanvasItem::TEXT || selectedItem->type == CanvasItem::PURCHASE_INFO) {
            /*Sumamos la posicion que podria ser 50 + 13 que es el tamaÃÂÃÂ±o de la letra = 63 (el texto mide desde 50 hasta 63) y lo
            restamos por la posicion del mouse actualmente en su primer toque de rezising quepuede ser 70 - 63 = 7 que es lo que se
            agregÃÂÃÂ³*/
            resizeStartOffset = pos.y - (selectedItem->pos.y + selectedItem->fontSize);
        }
        else if (selectedItem->type == CanvasItem::IMAGE) {
            resizeStartOffset = pos.y - (selectedItem->pos.y + selectedItem->imgHeight);
        }

        Refresh();
        return;
    }

    // 2ÃÂ¯ÃÂ¸ÃÂÃÂ¢ÃÂÃÂ£ Right width handle (cuadrado al final)
    CanvasItem* handleItemRight = HitTest(pos, false, true);
    if (handleItemRight) {
        selectedItem = handleItemRight;
        selectedItem->selected = true;
        rightWidthResizing = true;
        Refresh();
        return;
    }

    // 3ÃÂ¯ÃÂ¸ÃÂÃÂ¢ÃÂÃÂ£ SelecciÃÂÃÂ³n normal
    selectedItem = HitTest(pos);
    if (selectedItem) {
        selectedItem->selected = true;
        dragOffset = pos - selectedItem->pos;
    }

    Refresh();
}

void Canvas::OnLeftUp(wxMouseEvent&) {
    // Al soltar el mouse, hacer el redimensionamiento final de alta calidad
    if ((resizing || rightWidthResizing) && selectedItem && selectedItem->type == CanvasItem::IMAGE) {
        selectedItem->bmp = wxBitmap(selectedItem->originalImage.Rescale(selectedItem->imgWidth, selectedItem->imgHeight, wxIMAGE_QUALITY_HIGH));
        selectedItem->originalImage = selectedItem->bmp.ConvertToImage();
        Refresh();
    }
    resizing = false;
    rightWidthResizing = false;
}

void Canvas::UpdateTooltip(const wxPoint& pos) {
    wxString tip = wxString::Format("X: %d, Y: %d", pos.x, pos.y);
    SetToolTip(tip);
}

void Canvas::OnMouseMove(wxMouseEvent& evt) {
    if (!selectedItem) return;
    wxPoint pos = evt.GetPosition();
    CalcUnscrolledPosition(pos.x, pos.y, &pos.x, &pos.y);

    if (resizing && evt.LeftIsDown()) {
        if (selectedItem->type == CanvasItem::TEXT || selectedItem->type == CanvasItem::PURCHASE_INFO) {
            /*Tomando el ejemplo de la funcion OnLeftDown restamos 75 (la posicion del mouse se moviÃÂÃÂ³) - 50 - 7 = 18 (se moviÃÂÃÂ³ 5 y ese es el nuevo tamaÃÂÃÂ±o de letra) */
            int newHeight = std::max(8, pos.y - selectedItem->pos.y - resizeStartOffset);
            selectedItem->fontSize = newHeight;
            Refresh();
        }
        else if (selectedItem->type == CanvasItem::IMAGE) {
            int newWidth = std::max(20, pos.x - selectedItem->pos.x);
            int newHeight = std::max(20, pos.y - selectedItem->pos.y - resizeStartOffset);

            selectedItem->imgWidth = newWidth;
            selectedItem->imgHeight = newHeight;

            Refresh();
        }
    }
    else if (rightWidthResizing) {
        if (selectedItem->type == CanvasItem::IMAGE) {
            int newWidth = std::max(20, pos.x - selectedItem->pos.x);
            selectedItem->imgWidth = newWidth;
            Refresh();
        }
        if (selectedItem->type == CanvasItem::TEXT || selectedItem->type == CanvasItem::PURCHASE_INFO) {
            wxClientDC dc(this);
            PrepareDC(dc);
            auto [textW, textH] = selectedItem->GetSizetext(selectedItem);
            int newExtension = pos.x - (selectedItem->pos.x + textW);
            int minExtension = 0;
            selectedItem->textRightWidth = std::max(minExtension, newExtension);

            Refresh();
            Update();
        }
    }
    else if (evt.Dragging() && evt.LeftIsDown()) {
        selectedItem->pos = pos - dragOffset;
        Refresh();
    }
}

void Canvas::OnDoubleLeftClick(wxMouseEvent& event) {
    wxPoint pos = event.GetPosition();
    CalcUnscrolledPosition(pos.x, pos.y, &pos.x, &pos.y);
    selectedItem = HitTest(pos);
    if (selectedItem && selectedItem->type == CanvasItem::TEXT) {
        wxString texto = selectedItem->text;
        bool isUnderInfo = selectedItem->underInfo;
        TextDialog dlg(this, _("Edit text"));
        dlg.AplicarTema(temaOscuro);
        dlg.SetText(texto);
        dlg.SetIsUnderInfo(isUnderInfo);
        if (dlg.ShowModal() == wxID_OK) {
            wxString text = dlg.GetText();
            bool underInfo = dlg.GetIsUnderInfo();
            selectedItem->text = text;
            selectedItem->underInfo = underInfo;
        }
        Refresh();
    }
}

void Canvas::OnKeyDown(wxKeyEvent& evt) {
    // Si hay un item seleccionado y se presiona Backspace o Delete
    //if (selectedItem) wxMessageBox(wxString::Format("Tipo: %d", selectedItem->type));
    if (selectedItem && (evt.GetKeyCode() == WXK_BACK || evt.GetKeyCode() == WXK_DELETE)) {
        // Buscar y eliminar el item seleccionado
        for (auto it = items.begin(); it != items.end(); ++it) {
            if (&(*it) == selectedItem) {
                items.erase(it);
                if (selectedItem->type == CanvasItem::PURCHASE_INFO) parentFrame->alreadyExistsPurchaseInfo = false;
                selectedItem = nullptr;
                Refresh();
                return;
            }
        }
    }
    evt.Skip(); // Permite que otros eventos de teclado se procesen normalmente
}