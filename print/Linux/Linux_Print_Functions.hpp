#pragma once
#include <wx/wx.h>
#include <string>
#ifdef __linux__
/**
    @namespace Print_Ticket
    @brief Functions that print on canvas mode in Linux to the printer
**/
namespace Print_Ticket_Linux{

    /**
     * @brief Sends string data to EscPos printer on Linux
     * 
     * @param printerName Printer name to send escpos commands
     * @param rawData String with de data to send
     * @return true if success.
     * @return false if not success.
     */
    inline bool SendRawDataToPrinterLinux(const wxString& printerName, const std::string& rawData) {
        const char* printerDevice = printerName.mb_str(); 
        FILE* printer = fopen(printerDevice, "w");
        if (!printer) return false;

        size_t size = rawData.size();

        size_t written = fwrite(rawData.c_str(), 1, size, printer);
        fclose(printer);

        return written == size;
    }

}

#endif