#include <wx/wx.h>
#include "print/Canvas/CanvasItem/CanvasItem.hpp"
#include <codecvt>
#include <locale>
#include "print/EscPos/EscPosDictionary.hpp"
#include "print/Print_Utils_Getters.hpp"
#include "constants/MESSAGES_ADVICE/ERROR/PRINTING/PRINTING_ERRORS.hpp"
#include "print/Windows/Windows_Print_Functions_.hpp"
#include "print/Linux/Linux_Print_Functions.hpp"

namespace Print_Utils{

    /**
     * Prints just text at the start of X and Y axis
     * \param text Text to print
     * \param alignCenter If user is using EscPos align the text on the left if false or center if true
     */
    inline void PrintJustText(std::string& text, bool alignCenter) {
        try {
            char mode = PrintGetters::GetMode();
            if (text.empty()) {
                wxMessageBox(_("No info to print"), _("No info"), wxICON_ERROR);
                return;
            }
            wxString printerName = PrintGetters::GetSelectedPrinter();
            if (printerName.IsEmpty()) {
                wxMessageBox(PRINTING_ERROR_MESSAGES::INVALID_PRINTER_MESSAGE, PRINTING_ERROR_MESSAGES::PRINT_ERROR, wxICON_ERROR);
                return;
            }
#ifdef _WIN32
            if (mode == 0) {
                std::wstring wPrinterName = printerName.ToStdWstring();
                std::optional<HDC> maybeHDC = Print_Ticket_Windows::GetDeviceContextHandler(wPrinterName);
                if (!maybeHDC.has_value()) return;
                HDC hDC = maybeHDC.value();
                std::wstring wstr;
                int size = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, nullptr, 0);
                if (size > 0) {
                    wstr.resize(size - 1);
                    MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, &wstr[0], size);
                }
                Print_Ticket_Windows::DrawTextOnHDCWindows(hDC, wstr, 5, 5, 22, L"Arial", false, false, false);
                EndPage(hDC);
                EndDoc(hDC);
                DeleteDC(hDC);
            }
#elif __linux__
            if (mode == 0) wxMessageBox(_("No canvas available by the moment. Select EscPos"), _("No available"), wxOK | wxICON_WARNING);
#endif
            else if (mode == 1) {
                std::string commands;
                using namespace ESC_POS_DIC;
                commands.append(reinterpret_cast<const char*>(INIT), sizeof(INIT)); //Inicializamos impresora
                if (alignCenter) commands.append(reinterpret_cast<const char*>(ALIGN_CENTER), sizeof(ALIGN_CENTER));
                else commands.append(reinterpret_cast<const char*>(ALIGN_LEFT), sizeof(ALIGN_LEFT));
                
                commands.append(text);
#ifdef _WIN32
                Print_Ticket_Windows::SendRawDataToPrinterWindows(printerName, commands);
#elif __linux__
                Print_Ticket_Linux::SendRawDataToPrinterLinux(printerName, commands);
#endif
            }
            else wxMessageBox(PRINTING_ERROR_MESSAGES::ERROR_GETTING_MODE_MESSAGE, "Error", wxOK | wxICON_ERROR);
        }
        catch (const std::exception& e) {
            wxMessageBox(PRINTING_ERROR_MESSAGES::ERROR_PRINTING_TICKET_MESSAGE + wxString(e.what()), "Error", wxOK | wxICON_ERROR);
        }
    }

}
