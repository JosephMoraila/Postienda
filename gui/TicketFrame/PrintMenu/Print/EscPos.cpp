#include "gui/TicketFrame/PrintMenu/PrintMenu.hpp"
#include "print/EscPos/WxImageToEscPos.hpp"
#include "print/EscPos/EscPosDictionary.hpp"
#include "print/Windows/Windows_Print_Functions_.hpp"
#include "print/Print_Utils_Getters.hpp"
#include <Linux/Linux_Print_Functions.hpp>

void PrintMenu::PrintEscPosPrueba(const wxString& printerName) {
    std::vector<ContenidoLinea> items = PrintGetters::LoadEscPosFromFile();
    if (items.empty()) {
        wxMessageBox(_("No items found in ticket"), "Info", wxOK | wxICON_INFORMATION);
        return;
    }
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
                std::string texto = std::string(item.texto.mb_str());
                commands.append(reinterpret_cast<const char*>(ALIGN_CENTER), sizeof(ALIGN_CENTER));
                if (texto != "[PURCHASE_INFO_STARTS]" && texto != "[PURCHASE_INFO_FINISHES]") commands.append(texto + '\n');
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