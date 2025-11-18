#pragma once
#ifdef _WIN32
#include <wx/wx.h>
#include <windows.h>
#include <optional>

/**
    @namespace Print_Ticket
    @brief Functions that print on canvas mode in Windows to the printer
**/
namespace Print_Ticket_Windows{

    /**
        @brief Create Device Context handler to print
        @param  printerName - Printer name
        @retval  - If succesful returns the handler, if not, nullopt
    **/
    inline std::optional<HDC> GetDeviceContextHandler(const std::wstring& printerName) {
        DOCINFO di = { sizeof(DOCINFO) };
        di.lpszDocName = L"Ticket POS";
        // Crear contexto de dispositivo (HDC). Se le pasa en el primer parÃÂ¡metro "WINSPOOL" para indicar que es una impresora de Windows.
        //En el segundo parÃÂ¡metro se pasa el nombre de la impresora.
        //Y en los otros dos parÃÂ¡metros se pasan NULL porque no se usan en este caso ya que son para impresoras de red o configuraciones especÃÂ­ficas.
        //Retorna un manejador al contexto del dispositivo (HDC) que representa la impresora especificada.
        HDC hDC = CreateDC(L"WINSPOOL", printerName.c_str(), NULL, NULL);
        if (!hDC) {
            wxMessageBox(_("Error creating device context for the selected printer."), _("Printing Error"), wxICON_ERROR);
            return std::nullopt;  // Ã¢ÂÂ No hay valor
        }

        if (StartDoc(hDC, &di) <= 0) {
            wxMessageBox(_("Error starting print document."), _("Printing Error"), wxICON_ERROR);
            DeleteDC(hDC);
            return std::nullopt;
        }

        if (StartPage(hDC) <= 0) {
            wxMessageBox(_("Error starting the print page."), _("Printing Error"), wxICON_ERROR);
            EndDoc(hDC);
            DeleteDC(hDC);
            return std::nullopt;
        }
        return hDC; 
    }

    /**
     @brief Draws the text on the printer paper
     @param hDC       - Device context handler
     @param texto     - Text to print
     @param x         - X axis (horizontal)
     @param y         - Y axis (vertical)
     @param fontSize  - Font size (by default 12)
     @param fontName  - Font name (by default Arial)
     @param bold      - Bold text (by default false)
     @param italic    - Italic text (by default false)
     @param underline - Underline text (by default false)
    **/
	inline void DrawTextOnHDCWindows(HDC hDC, const std::wstring& texto, int x, int y, int fontSize = 12, const std::wstring& fontName = L"Arial", bool bold = false, bool italic = false, bool underline = false) {
        // Elegir peso de la fuente
        int weight = bold ? FW_BOLD : FW_NORMAL;

        // Crear fuente
        HFONT hFont = CreateFont(
            -fontSize, 0, 0, 0, weight,
            italic ? TRUE : FALSE,
            underline ? TRUE : FALSE,
            FALSE, // strikeout
            DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY,
            DEFAULT_PITCH | FF_SWISS,
            fontName.c_str()
        );

        if (!hFont) {
            wxMessageBox(_("Error creating the font for printing."), _("Printing Error"), wxICON_ERROR);
            return;
        }

        // HFONT significa Handle to Font, es un identificador para una fuente en la API de Windows.
        //En este usamos SelectObject para seleccionar la fuente creada (hFont)
        //Y guardamos la fuente anterior en hOldFont para restaurarla despuÃÂ©s.
        HFONT hOldFont = (HFONT)SelectObject(hDC, hFont);

        // Fondo transparente
        SetBkMode(hDC, TRANSPARENT);

        // Medir altura de lÃÂ­nea
        TEXTMETRIC tm; //TEXTMETRIC es una estructura de Windows que almacena informaciÃÂ³n sobre la fuente actualmente seleccionada en un HDC
        GetTextMetrics(hDC, &tm); //GetTextMetrics obtiene las mÃÂ©tricas de la fuente seleccionada en el HDC dado y las almacena en la estructura TEXTMETRIC proporcionada.
        int lineHeight = tm.tmHeight + tm.tmExternalLeading; //Altura de lÃÂ­nea = altura de la fuente + espacio adicional

        // Dibujar cada lÃÂ­nea separada por \n
        int yOffset = 0; // Desplazamiento vertical acumulado para dibujar cada lÃÂ­nea
        size_t start = 0; // PosiciÃÂ³n inicial en el texto para buscar saltos de lÃÂ­nea
        size_t pos; // PosiciÃÂ³n del siguiente '\n' encontrado
        while ((pos = texto.find(L'\n', start)) != std::wstring::npos) { //Mientras se encuentren saltos de lÃÂ­nea
            std::wstring line = texto.substr(start, pos - start); // Extraer la lÃÂ­nea actual
            TextOutW(hDC, x, y + yOffset, line.c_str(), (int)line.length()); // Dibujar la lÃÂ­nea en la posiciÃÂ³n actual
            yOffset += lineHeight; // Incrementar el desplazamiento vertical para la siguiente lÃÂ­nea
            start = pos + 1; // Actualizar la posiciÃÂ³n inicial para la siguiente bÃÂºsqueda
        }
        // ÃÂltima lÃÂ­nea
        std::wstring line = texto.substr(start); // Extraer la ÃÂºltima lÃÂ­nea despuÃÂ©s del ÃÂºltimo salto de lÃÂ­nea
        TextOutW(hDC, x, y + yOffset, line.c_str(), (int)line.length());

        // Restaurar fuente anterior y liberar recursos
        SelectObject(hDC, hOldFont);
        DeleteObject(hFont);
	}


    /**
     @brief Draws an image on a printer paper
     @param hDC     - Device context handler
     @param hBitmap - Image bitmap
     @param x       - X axis (horizontal)
     @param y       - Y axis (vertical)
     @param width   - Image width
     @param height  - Image height
    **/
    inline void DrawImageOnHDCWindows(HDC hDC, HBITMAP hBitmap, int x, int y, int width, int height) {
        BITMAP bmp;
        GetObject(hBitmap, sizeof(BITMAP), &bmp);

        HDC hMemDC = CreateCompatibleDC(hDC);
        HGDIOBJ oldBitmap = SelectObject(hMemDC, hBitmap);

        // **PASO CLAVE 1: Configurar el fondo del HDC a transparente.**
        // Esto es crucial para que AlphaBlend no use el color de fondo opaco (negro) 
        // del contexto de la impresora.
        // SetBkMode(hDC, TRANSPARENT); // Esto puede o no funcionar dependiendo del driver, la solucion completa es la siguiente:

        BLENDFUNCTION blendFunction;
        blendFunction.BlendOp = AC_SRC_OVER;      // Usar la capa de origen sobre la de destino
        blendFunction.BlendFlags = 0;
        blendFunction.SourceConstantAlpha = 255;  // No usar alpha constante (usar el de la imagen)
        blendFunction.AlphaFormat = AC_SRC_ALPHA; // Usar el canal alfa del HBITMAP de origen

        // **PASO CLAVE 2: Usar AlphaBlend (con SetBkMode y SetBkColor)**
        // Aunque SetBkMode/SetBkColor a menudo son ignorados por los drivers de impresiÃÂ³n, 
        // la combinaciÃÂ³n de SetBkMode(hDC, TRANSPARENT) y AlphaBlend es la ÃÂºnica forma 
        // estÃÂ¡ndar de GDI para lograr el efecto "sin fondo" en la impresiÃÂ³n.

        // Guardar el modo y color de fondo actual
        int oldBkMode = GetBkMode(hDC);
        COLORREF oldBkColor = GetBkColor(hDC);

        // Establecer el modo de fondo a transparente
        SetBkMode(hDC, TRANSPARENT);
        // Establecer el color de fondo a blanco por si se usa (aunque transparente deberÃÂ­a anularlo)
        SetBkColor(hDC, RGB(255, 255, 255));

        BOOL success = AlphaBlend(
            hDC, x, y, width, height, // Destino
            hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, // Fuente
            blendFunction
        );

        // Restaurar el modo y color de fondo original
        SetBkMode(hDC, oldBkMode);
        SetBkColor(hDC, oldBkColor);

        // Si AlphaBlend falla (p.ej., si el driver no lo soporta), como fallback usamos StretchBlt
        if (!success) {
            StretchBlt(
                hDC, x, y, width, height,
                hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight,
                SRCCOPY
            );
        }

        SelectObject(hMemDC, oldBitmap);
        DeleteDC(hMemDC);
    }


    /**
    @brief Uses the Windows API to send raw ESC/POS data to the specified printer. USE IF THE PRINTER HAS A DRIVER THAT DOES NOT GRAPHICALLY RENDER THE DATA GRAPHICS.
    @param  printerName - Name of the target printer.
    @param  rawData     - Raw ESC/POS data to send to the printer.
    @retval true if the data was sent successfully, false otherwise.
    **/
    inline bool SendRawDataToPrinterWindows(const wxString& printerName, const std::string& rawData) {
        HANDLE hPrinter; //Manejador de impresora, HANDLE es un puntero opaco usado en la API de Windows para representar recursos del sistema
        DWORD dwBytesWritten = 0; //NÃÂºmero de bytes escritos, DWORD es un entero sin signo de 32 bits, es decir, valores de 0 a 4,294,967,295
        DOC_INFO_1W docInfo; //Estructura que contiene informaciÃÂ³n sobre el documento a imprimir, DOC_INFO_1 es una estructura de la API de Windows que contiene informaciÃÂ³n sobre un documento de impresiÃÂ³n.
        bool bSuccess = false;

        // 1. Abrir la impresora
        // Se usa const_cast para convertir 'const wchar_t*' (de wc_str()) a 'LPWSTR'
        // sin crear una copia de la cadena.
        //OpenPrinter es una funciÃÂ³n de la API de Windows que abre una conexiÃÂ³n a una impresora, 
        //en su primer parÃÂ¡metro recibe el nombre de la impresora, PWSTR es un puntero a una cadena de caracteres anchos (wchar_t) que representa el nombre de la impresora,
        // en el segundo un puntero a un manejador (HANDLE) que se llenarÃÂ¡ con el identificador de la impresora abierta, 
        // y en el tercero opciones adicionales (NULL para valores por defecto).
        if (!OpenPrinter(const_cast<PWSTR>(printerName.wc_str()), &hPrinter, NULL)) {
            wxLogError(_("Error opening printer:") + printerName);
            return false;
        }
        //LPTSTR es un puntero a una cadena de caracteres (char* o wchar_t* dependiendo de si se usa Unicode o no).
        //Lo que se hace despues del = es tomar la cadena literal "Ticket ESC/POS" y convertirla a un puntero LPTSTR usando la macro TEXT
        docInfo.pDocName = (LPTSTR)TEXT("Ticket ESC/POS");
        docInfo.pOutputFile = NULL; //NULL indica que se imprimirÃÂ¡ directamente en la impresora y no se guardarÃÂ¡ en un archivo
        docInfo.pDatatype = (LPTSTR)TEXT("RAW"); //RAW indica que los datos se enviarÃÂ¡n tal cual, sin procesamiento adicional, es usado para enviar comandos especÃÂ­ficos a la impresora

        // 2. Iniciar el documento, LPBYTE es un puntero a un byte (unsigned char*)
        if (StartDocPrinter(hPrinter, 1, (LPBYTE)&docInfo)) { //El primer parÃÂ¡metro es el manejador de la impresora, el segundo es el nivel de informaciÃÂ³n (1 para DOC_INFO_1), y el tercero es un puntero a la estructura DOC_INFO_1
            if (StartPagePrinter(hPrinter)) {
                // 4. Escribir los datos RAW
                bSuccess = WritePrinter(
                    hPrinter, //Manejador de la impresora
                    (void*)rawData.data(), //Con .data() se obtiene un puntero al primer byte de la cadena rawData y con (void*) se convierte a un puntero genÃÂ©rico (void*)
                    (DWORD)rawData.size(), //TamaÃÂ±o de los datos a escribir, se convierte a DWORD
                    &dwBytesWritten //AquÃÂ­ se almacenarÃÂ¡ el nÃÂºmero de bytes realmente escritos
                );
                EndPagePrinter(hPrinter); //Finaliza la pÃÂ¡gina de impresiÃÂ³n actual, es importante para que la impresora sepa que ya no hay mÃÂ¡s datos para esta pÃÂ¡gina
            }

            EndDocPrinter(hPrinter); //Finaliza el documento de impresiÃÂ³n, es importante para liberar recursos y asegurarse de que todo se imprima correctamente
        }

        ClosePrinter(hPrinter); //Cierra la conexiÃÂ³n con la impresora, liberando el manejador

        if (!bSuccess || dwBytesWritten != rawData.size()) { //Verifica si la escritura fue exitosa y si se escribieron todos los bytes
            wxLogError(_("Error sending RAW data to the printer. Bytes written: %lu"), (unsigned long)dwBytesWritten);
            return false;
        }

        return true;
    }

}

#endif