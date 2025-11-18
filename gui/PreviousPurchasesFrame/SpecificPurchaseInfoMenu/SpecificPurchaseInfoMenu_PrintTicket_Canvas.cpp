#include "print/Canvas/CanvasItem/CanvasItem.hpp"
#include "gui/PreviousPurchasesFrame/SpecificPurchaseInfoMenu/SpecificPurchaseInfoMenu.hpp"
#include "print/Print_Utils_Getters.hpp"
#include "print/Windows/Windows_Print_Functions_.hpp"
#include "print/Canvas/Canvas_Utils.hpp"
#include "utils/MathUtils.hpp"
#include "utils/DateTimeUtils.hpp"
#include "constants/MESSAGES_ADVICE/ERROR/PRINTING/PRINTING_ERRORS.hpp"
using namespace PRINTING_ERROR_MESSAGES;

#ifdef _WIN32
void SpecificPurchaseInfoMenu::PrintCanvasPurchaseWindows(std::vector<PurchaseItem>& PurchaseItems) {
    wxString printerName = PrintGetters::GetSelectedPrinter();
    if (printerName.IsEmpty()) {
        wxMessageBox(INVALID_PRINTER_MESSAGE, PRINT_ERROR, wxICON_ERROR);
        return;
    }

    std::wstring wPrinterName = printerName.ToStdWstring();
    std::optional<HDC> maybeHDC = Print_Ticket_Windows::GetDeviceContextHandler(wPrinterName);
    if (!maybeHDC.has_value()) return;
    HDC hDC = maybeHDC.value();
    std::vector<CanvasItem> itemsCanvas = PrintGetters::LoadCanvasFromFile();
    if (PurchaseItems.empty()) {
        wxMessageBox(NO_ITEMS_TICKET_MESSAGE, PRINT_ERROR, wxICON_ERROR);
        EndPage(hDC);
        EndDoc(hDC);
        DeleteDC(hDC);
        return;
    }
    bool purchaseInfoExists = false;
    int baseY = 0; // posiciÃÂ³n Y del PURCHASE_INFO
    int currentUnderY = 0; // acumulador vertical

    // Primer bucle: detectar PURCHASE_INFO
    for (CanvasItem& item : itemsCanvas) {
        if (item.type == CanvasItem::PURCHASE_INFO) {
            purchaseInfoExists = true;

			wxString products = CreateInfoPurchaseStringToPrint(PurchaseItems);
            if (products.IsEmpty()) {
                wxMessageBox(_("The products could not be obtained."), "Error", wxICON_ERROR);
                EndPage(hDC);
                EndDoc(hDC);
                DeleteDC(hDC);
                return;
            }

            //Si se aplicÃÂ³ formato ancho a PURCHASE_INFO se formatea 
            if (item.textRightWidth > 0) item.textWithFormat = Canvas_Utils::FormatProductsLikeOriginal(products, item);
            else item.textWithFormat = products; //Si no se mantiene igual con formato estandar

            int textHeight = Canvas_Utils::CalculateTextHeight(item.textWithFormat, item.fontSize);
            baseY = item.pos.y + textHeight;
            currentUnderY = baseY;

            Print_Ticket_Windows::DrawTextOnHDCWindows(hDC, item.textWithFormat.wc_str(), item.pos.x - 10, item.pos.y, item.fontSize, L"Arial", false, false, false);
            currentUnderY = baseY;    // posiciÃÂ³n inicial de los underInfo
            break; //Salirse del bucle porque ya se encontrÃÂ³
            
        }
    }
    for (CanvasItem& item : itemsCanvas) {
        if (item.type == CanvasItem::TEXT) {
            if (item.underInfo && purchaseInfoExists) {
                // Calcular separaciÃÂ³n adaptativa (basada en fontSize)
                int dynamicSpacing = static_cast<int>((item.fontSize / 16.0) * 16);

                // Dibujar debajo del PURCHASE_INFO
                currentUnderY += dynamicSpacing;

                Print_Ticket_Windows::DrawTextOnHDCWindows(hDC, item.textWithFormat.wc_str(), item.pos.x - 15, currentUnderY, item.fontSize, L"Arial", false, false, false);

                // Avanzar para el siguiente texto, segÃÂºn tamaÃÂ±o del texto actual
                currentUnderY += item.fontSize + 2;
            }
            else if (!item.underInfo) {
                // Texto normal
                Print_Ticket_Windows::DrawTextOnHDCWindows(hDC, item.textWithFormat.wc_str(), item.pos.x - 15, item.pos.y - 10, item.fontSize, L"Arial", false, false, false);
            }
        }
        else if (item.type == CanvasItem::IMAGE) {
            wxImage imagen = item.originalImage;
            if (imagen.IsOk()) {
                wxBitmap bmp(imagen);
                HBITMAP hBmp = (HBITMAP)bmp.GetHBITMAP();
                Print_Ticket_Windows::DrawImageOnHDCWindows(hDC, hBmp, item.pos.x - 35, item.pos.y, item.imgWidth, item.imgHeight);
            }
        }
    }

    EndPage(hDC);
    EndDoc(hDC);
    DeleteDC(hDC);
}
#endif


wxString SpecificPurchaseInfoMenu::CreateInfoPurchaseStringToPrint(std::vector<PurchaseItem>& PurchaseItems) {
    auto [worker, total, dateTime] = GetWorkerTotalDateTimePurchaseID();
    auto [fechaFormateada, hora] = DateTimeUtils::FormatDateTimeLocalized(dateTime);
    //Append the products info by row
    wxString mensaje;
    mensaje += wxString::Format("%s\n%s\n", fechaFormateada.wc_str(), hora.wc_str());
    for (const PurchaseItem& item : PurchaseItems) {
        mensaje += wxString::Format(
            "%s %s $%s\n",
            item.productName,
            item.barcode,
            FormatFloatWithCommas(item.priceAtPurchase).wc_str()
        );
    }
    mensaje += wxString::Format(
        _("Total: %s\n"
		"Attended by: %s"),
        FormatFloatWithCommas(total).wc_str(),
		worker.wc_str()
    );
	return mensaje;
}