#include "gui/TicketFrame/TicketFrame.hpp"

std::vector<ContenidoLinea> TicketFrame::ExtraerContenidoRichInput(wxRichTextCtrl* richInput) {
    std::vector<ContenidoLinea> contenido;
    wxRichTextBuffer& buffer = richInput->GetBuffer();
    wxRichTextObjectList::compatibility_iterator node = buffer.GetChildren().GetFirst();
    int numeroLinea = 0;

    bool dentroDeInfoCompra = false; // Flag para rastrear si estamos dentro de PURCHASE_INFO
    bool lastMarcadorInfo = false;

    while (node) {
        wxRichTextParagraph* para = wxDynamicCast(node->GetData(), wxRichTextParagraph);
        if (para) {
            wxRichTextObjectList::compatibility_iterator childNode = para->GetChildren().GetFirst();

            while (childNode) {
                // === PROCESAR IMÃÂGENES ===
                wxRichTextImage* imgObj = wxDynamicCast(childNode->GetData(), wxRichTextImage);
                if (imgObj) {
                    ContenidoLinea lineaImg;
                    lineaImg.type = ContenidoLinea::IMAGE;
                    lineaImg.numero = numeroLinea;
                    lineaImg.esImagen = true;
                    lineaImg.texto = "";

                    wxRichTextImageBlock& imgBlock = imgObj->GetImageBlock();
                    wxImage img;
                    wxMemoryInputStream stream(imgBlock.GetData(), imgBlock.GetDataSize());
                    img.LoadFile(stream, imgBlock.GetImageType());
                    lineaImg.imagen = img;

                    contenido.push_back(lineaImg);
                }

                // === PROCESAR TEXTO ===
                wxRichTextPlainText* txtObj = wxDynamicCast(childNode->GetData(), wxRichTextPlainText);
                if (txtObj) {
                    wxString texto = txtObj->GetText();

                    if (!texto.IsEmpty()) {
                        // Detectar inicio y fin de PURCHASE_INFO
                        if (texto.Contains("[PURCHASE_INFO_STARTS]")) {
                            dentroDeInfoCompra = true;
                        }

                        if (texto.Contains("[PURCHASE_INFO_FINISHES]")) {
                            dentroDeInfoCompra = false;
                            lastMarcadorInfo = true;
                        }

                        ContenidoLinea lineaTxt;
                        lineaTxt.numero = numeroLinea;
                        lineaTxt.esImagen = false;
                        lineaTxt.texto = texto;

                        // Asignar tipo segÃÂºn el contexto
                        if (dentroDeInfoCompra || lastMarcadorInfo) lineaTxt.type = ContenidoLinea::PURCHASE_INFO;
                        else lineaTxt.type = ContenidoLinea::TEXT;

                        if (lastMarcadorInfo) lastMarcadorInfo = false;
                        
                        contenido.push_back(lineaTxt);
                    }
                }

                childNode = childNode->GetNext();
            }
            numeroLinea++;
        }
        node = node->GetNext();
    }

    return contenido;
}

void TicketFrame::AlignCenterRichInput(wxRichTextCtrl* richText) {
    // Seleccionar todo el texto
    richText->SelectAll();

    // Aplicar alineaciÃÂ³n al centro
    richText->ApplyAlignmentToSelection(wxTEXT_ALIGNMENT_CENTRE);

    // Deseleccionar
    richText->SelectNone();

    // Actualizar la visualizaciÃÂ³n
    richText->Refresh();
}
