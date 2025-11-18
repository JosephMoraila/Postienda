#include "print/EscPos/LineStructure/LineStructure.hpp"
#include "gui/MainFrame/MainFrame.h"
#include "print/Print_Utils_Getters.hpp"
#include "print/EscPos/WxImageToEscPos.hpp"
#include "print/EscPos/EscPosDictionary.hpp"
#include "print/Windows/Windows_Print_Functions_.hpp"
#include "constants/MESSAGES_ADVICE/ERROR/PRINTING/PRINTING_ERRORS.hpp"
#include <Linux/Linux_Print_Functions.hpp>
using namespace PRINTING_ERROR_MESSAGES;

void MainFrame::PrintEscposPurchase(double& pagoCliente, bool& cashPayment) {
    wxString printerName = PrintGetters::GetSelectedPrinter();
    if (printerName.IsEmpty()) {
        wxMessageBox(PRINTING_ERROR_MESSAGES::INVALID_PRINTER_MESSAGE, PRINTING_ERROR_MESSAGES::PRINT_ERROR, wxICON_ERROR);
        return;
    }

    std::vector<ContenidoLinea> items = PrintGetters::LoadEscPosFromFile();
    if (items.empty()) {
        wxMessageBox(NO_ITEMS_TICKET_MESSAGE, "Info", wxOK | wxICON_INFORMATION);
        return;
    }
    bool alreadyPurchaseIInfo = false; // Because purchase info example are multiple lines, we need to avoid print the purchase info multiple times, only once
    int width = PrintGetters::GetWidth();
    constexpr double DPI = 203.0;
    int widthDots = static_cast<int>(width * DPI / 25.4);
    using namespace ESC_POS_DIC;
    std::string commands;
    commands.append(reinterpret_cast<const char*>(INIT), sizeof(INIT)); //Inicializamos impresora
    commands.append(reinterpret_cast<const char*>(ALIGN_CENTER), sizeof(ALIGN_CENTER));

    int lastLine = -1; // Para saber cuÃÂ¡ntos saltos de lÃÂ­nea agregar
    for (const auto& item : items) {
        int type = item.type;
        int line = item.numero;

        // Si hay lÃÂ­neas vacÃÂ­as entre el ÃÂºltimo elemento y el actual, agregarlas
        if (lastLine >= 0 && line > lastLine + 1) {
            int emptyLines = line - lastLine - 1;
            for (int i = 0; i < emptyLines; ++i) commands.append("\n");
        }

        switch (type) {
        case 0: { //TEXTO NORMAL
            std::string texto = std::string(item.texto.mb_str());
            texto += '\n'; // salto de lÃÂ­nea ESC/POS
            commands.append(reinterpret_cast<const char*>(ALIGN_CENTER), sizeof(ALIGN_CENTER));
            commands.append(texto);
            break;
        }
        case 1: {
            std::string imageCommands = WxImageToEscPos::WxImageToEscPosCommandsManual(item.imagen, true, true, widthDots);

            // Eliminar salto final si existe
            if (!imageCommands.empty() && (imageCommands.back() == '\n' || imageCommands.back() == '\r')) imageCommands.pop_back();
            commands.append(imageCommands.data(), imageCommands.size());
            break;
        }
        case 2: { //PURCHASE INFO
            if (alreadyPurchaseIInfo) break; //Purchase info already printed, so break the case
            wxString products = GetInfoPurchaseToPrint(pagoCliente, cashPayment);
            std::string texto = std::string(products.mb_str());
            commands.append(reinterpret_cast<const char*>(ALIGN_CENTER), sizeof(ALIGN_CENTER));
            if (texto != "[PURCHASE_INFO_STARTS]" && texto != "[PURCHASE_INFO_FINISHES]") commands.append(texto + '\n');
            alreadyPurchaseIInfo = true; // We need to print purchase info once
            break;
        }
        }
        lastLine = line;
    }
    commands.append(reinterpret_cast<const char*>(TOTAL_CUT), sizeof(TOTAL_CUT));
#ifdef _WIN32
    Print_Ticket_Windows::SendRawDataToPrinterWindows(printerName, commands);
#elif __linux__
    Print_Ticket_Linux::SendRawDataToPrinterLinux(printerName, commands);
#endif
}