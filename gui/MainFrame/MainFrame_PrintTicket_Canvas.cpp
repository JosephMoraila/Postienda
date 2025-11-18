#include "print/Canvas/CanvasItem/CanvasItem.hpp"
#include "print/Canvas/Canvas_Utils.hpp"
#include "gui/MainFrame/MainFrame.h"
#include "constants/JSON/Ticket/PRINTER_SETTINGS.hpp"
#include "print/Print_Utils_Getters.hpp"
#include "print/Windows/Windows_Print_Functions_.hpp"
#include <fstream>
#include "constants/DB/DB.h"
#include "gui/MainFrame/Purchase_Item.hpp"
#include "utils/MathUtils.hpp"
#include "utils/GetFromFile.h"
#include "utils/DateTimeUtils.hpp"
#include "constants/MESSAGES_ADVICE/ERROR/PRINTING/PRINTING_ERRORS.hpp"
using namespace PRINTING_ERROR_MESSAGES;

void MainFrame::PrintTicket(double pagoCliente, bool cashPayment) {
    bool isPrint = PrintGetters::IsPrintTicket();
    if (!isPrint) return;
    char mode = PrintGetters::GetMode();
#ifdef _WIN32
    if (mode == 0) PrintCanvasPurchaseWindows(pagoCliente, cashPayment);
#elif __linux__
    if (mode == 0) wxMessageBox(_("No canvas available by the moment. Select EscPos"), _("No available"), wxOK | wxICON_WARNING);
#endif
    else if (mode == 1) PrintEscposPurchase(pagoCliente, cashPayment);
    else wxMessageBox(_("Error obtaining print mode"), "Error", wxOK | wxICON_ERROR);;
}

#ifdef _WIN32
void MainFrame::PrintCanvasPurchaseWindows(double& pagoCliente, bool& cashPayment) {
    wxString printerName = PrintGetters::GetSelectedPrinter();
    if (printerName.IsEmpty()) {
        wxMessageBox(PRINTING_ERROR_MESSAGES::INVALID_PRINTER_MESSAGE, PRINTING_ERROR_MESSAGES::PRINT_ERROR, wxICON_ERROR);
        return;
    }
    std::wstring wPrinterName = printerName.ToStdWstring();
    std::optional<HDC> maybeHDC = Print_Ticket_Windows::GetDeviceContextHandler(wPrinterName);
    if (!maybeHDC.has_value()) return;
    HDC hDC = maybeHDC.value();
    std::vector<CanvasItem> items = PrintGetters::LoadCanvasFromFile();
    if(items.empty()) {
        wxMessageBox(NO_ITEMS_TICKET_MESSAGE, PRINTING_ERROR_MESSAGES::PRINT_ERROR, wxICON_ERROR);
        EndPage(hDC);
        EndDoc(hDC);
        DeleteDC(hDC);
        return;
	}

    bool purchaseInfoExists = false;
    int baseY = 0; // posiciÃÂÃÂ³n Y del PURCHASE_INFO
    int currentUnderY = 0; // acumulador vertical

    // Primer bucle: detectar PURCHASE_INFO
    for (CanvasItem& item : items) {
        if (item.type == CanvasItem::PURCHASE_INFO) {
            purchaseInfoExists = true;

            wxString products = GetInfoPurchaseToPrint(pagoCliente, cashPayment);
            if (products.IsEmpty()) {
                wxMessageBox(_("The products could not be obtained."), "Error", wxICON_ERROR);
                EndPage(hDC);
                EndDoc(hDC);
                DeleteDC(hDC);
                return;
            }

            //Si se aplicÃÂÃÂ³ formato ancho a PURCHASE_INFO se formatea 
            if (item.textRightWidth > 0) item.textWithFormat = Canvas_Utils::FormatProductsLikeOriginal(products, item);
            else item.textWithFormat = products; //Si no se mantiene igual con formato estandar

            int textHeight = Canvas_Utils::CalculateTextHeight(item.textWithFormat, item.fontSize);
            baseY = item.pos.y + textHeight;
            currentUnderY = baseY;

            Print_Ticket_Windows::DrawTextOnHDCWindows(hDC,item.textWithFormat.wc_str(),item.pos.x - 10,item.pos.y,item.fontSize, L"Arial",false,false,false);
            currentUnderY = baseY;    // posiciÃÂÃÂ³n inicial de los underInfo
            break; //Salirse del bucle porque ya se encontrÃÂÃÂ³
        }
    }

    for (CanvasItem& item : items) {
        if (item.type == CanvasItem::TEXT) {
            if (item.underInfo && purchaseInfoExists) {
                // Calcular separaciÃÂÃÂ³n adaptativa (basada en fontSize)
                int dynamicSpacing = static_cast<int>((item.fontSize / 16.0) * 16);

                // Dibujar debajo del PURCHASE_INFO
                currentUnderY += dynamicSpacing;

                Print_Ticket_Windows::DrawTextOnHDCWindows(hDC, item.textWithFormat.wc_str(), item.pos.x - 15, currentUnderY, item.fontSize, L"Arial", false, false, false);

                // Avanzar para el siguiente texto, segÃÂÃÂºn tamaÃÂÃÂ±o del texto actual
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

wxString MainFrame::GetInfoPurchaseToPrint(double& pagoCliente, bool& cashPayment) {
    try {
        sqlite::database db(GetDBPath());

        // Obtener el ÃÂÃÂºltimo ID de purchases
        int lastPurchaseID = 0;
        db << "SELECT COALESCE(MAX(id), 0) FROM purchases" >> lastPurchaseID;

        if (lastPurchaseID == 0) {
            wxMessageBox("No purchases recorded.", "Info", wxOK | wxICON_INFORMATION);
            return "";
        }

		//Get date and time of purchase
        std::string dateTimeString;
        db << "SELECT date FROM purchases WHERE id = ?" << lastPurchaseID >> [&](std::string dateTime) {
			dateTimeString = dateTime;
			};

        std::vector<PurchaseItem> items;

        // Query con JOIN para obtener los datos
        auto results = db <<
            "SELECT p.name, p.barcode, pi.price_at_purchase, pi.quantity "
            "FROM purchase_items pi "
            "INNER JOIN products p ON pi.product_id = p.id "
            "WHERE pi.purchase_id = ?"
            << lastPurchaseID;

        double total = 0.0;

        results >> [&](std::string name, std::string barcode, double price, double quantity) {
            PurchaseItem item;
            item.productName = wxString::FromUTF8(name);
            item.barcode = wxString::FromUTF8(barcode);
            item.priceAtPurchase = price;
            item.quantity = quantity;
            items.push_back(item);
            total += price;
            };

        if (items.empty()) {
            wxMessageBox(_("No items were found for this purchase."), "Info", wxOK | wxICON_INFORMATION);
            return "";
        }

        double change = 0.0;
        change = pagoCliente - total;
        if (!cashPayment && change < 0) change = 0; //Como pagoCliente es 0 cuando es por tarjeta, cuando se resta el total queda negativo y en pago con tarjeta no puede haber negativo
        if (!cashPayment) pagoCliente = total; //Como pagoCliente cuando es por tarjeta es 0, el pago es el total

        wxString user = "";
        user = getUserFromJSON<wxString>();
        if (user == "Ninguno") user = "";

        auto [fechaFormateada, hora] = DateTimeUtils::FormatDateTimeLocalized(dateTimeString);

        //Append the products info by row
        wxString mensaje;

		mensaje += wxString::Format("%s\n%s\n", fechaFormateada.wc_str(), hora.wc_str());

        for (const auto& item : items) {
            mensaje += wxString::Format(
                "%s %s $%s\n",
                item.productName,
                item.barcode,
                FormatFloatWithCommas(item.priceAtPurchase).wc_str()
            );
        }

        //Append purchase info
        mensaje += wxString::Format(
            _("Total: $%s\n"
            "Paid: $%s\n"
            "Change: $%s\n"
            "Attended by: %s"),
            FormatFloatWithCommas(total).wc_str(),
            FormatFloatWithCommas(pagoCliente).wc_str(),
            FormatFloatWithCommas(change).wc_str(),
            user
        );
        return mensaje;

    }
    catch (const std::exception& e) {
        wxMessageBox(wxString::Format("Error: %s", e.what()), "Error", wxOK | wxICON_ERROR);
        return "";
    }
}
