
#include "gui/TicketFrame/PrintMenu/PrintMenu.hpp"
#include "print/Windows/Windows_Print_Functions_.hpp"
#include "print/Print_Utils_Getters.hpp"

#ifdef _WIN32

void PrintMenu::ImprimirCanvasPruebaWindows(const std::wstring& printerName)
{
    std::optional<HDC> maybeHDC = Print_Ticket_Windows::GetDeviceContextHandler(printerName);
    if (!maybeHDC.has_value()) return;
    HDC hDC = maybeHDC.value();

	std::vector<CanvasItem> items = PrintGetters::LoadCanvasFromFile();
    if(items.empty()) {
        wxMessageBox(_("There are no elements to print on the ticket."), _("Printing Error"), wxICON_ERROR);
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

            auto vertices = item.GetTextVertices(&item);
            wxPoint pointLeftDown = vertices[2];

            Print_Ticket_Windows::DrawTextOnHDCWindows(hDC,item.textWithFormat.wc_str(),item.pos.x - 10, item.pos.y,item.fontSize, L"Arial",false, false, false);

            baseY = pointLeftDown.y;  // parte baja del PURCHASE_INFO
            currentUnderY = baseY;    // posiciÃÂÃÂ³n inicial de los underInfo
            break; //Salirse del bucle porque ya se encontrÃÂÃÂ³
        }
    }

    // Segundo bucle: dibujar los demÃÂÃÂ¡s textos e imÃÂÃÂ¡genes
    for (CanvasItem& item : items) {
        if (item.type == CanvasItem::TEXT) {
            if (item.underInfo && purchaseInfoExists) {
                // Calcular separaciÃÂÃÂ³n adaptativa (basada en fontSize)
                int dynamicSpacing = static_cast<int>((item.fontSize / 16.0) * 6);

                // Dibujar debajo del PURCHASE_INFO
                currentUnderY += dynamicSpacing;

                Print_Ticket_Windows::DrawTextOnHDCWindows(hDC,item.textWithFormat.wc_str(),item.pos.x - 15,currentUnderY,item.fontSize,L"Arial",false, false, false);

                // Avanzar para el siguiente texto, segÃÂÃÂºn tamaÃÂÃÂ±o del texto actual
                currentUnderY += item.fontSize + 2;
            }
            else if (!item.underInfo) {
                // Texto normal
                Print_Ticket_Windows::DrawTextOnHDCWindows(hDC,item.textWithFormat.wc_str(),item.pos.x - 15,item.pos.y - 10, item.fontSize,L"Arial",false, false, false);
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
    // Terminar pÃÂÃÂ¡gina y documento
    EndPage(hDC);
    EndDoc(hDC);
    DeleteDC(hDC);
}

HBITMAP PrintMenu::WxImageToHBITMAPWithAlpha(const wxImage& image) {
    if (!image.IsOk()) return nullptr;

    wxImage tempImage = image.Copy();

    // Dejamos que la imagen mantenga su transparencia si la tiene.
    // **NO HACEMOS CONVERSION A ESCALA DE GRISES O MEZCLA A BLANCO AQUÃÂÃÂ.**

    int width = tempImage.GetWidth();
    int height = tempImage.GetHeight();

    // Creamos el bitmap GDI+ en formato ARGB para manejar el canal alfa.
    Bitmap* gdipBitmap = new Bitmap(width, height, PixelFormat32bppARGB);

    BitmapData bmpData;
    Rect rect(0, 0, width, height);
    gdipBitmap->LockBits(&rect, ImageLockModeWrite, PixelFormat32bppARGB, &bmpData);

    unsigned char* dest = (unsigned char*)bmpData.Scan0;
    unsigned char* src = tempImage.GetData();
    // Obtenemos el canal alfa de wxImage (si existe) o asumimos opaco (255).
    unsigned char* alpha = tempImage.HasAlpha() ? tempImage.GetAlpha() : nullptr;

    // Copiar pÃÂÃÂ­xeles en formato BGRA (con canal alfa real)
    for (int y = 0; y < height; y++) {
        unsigned char* destRow = dest + y * bmpData.Stride;
        for (int x = 0; x < width; x++) {
            int idx = y * width + x;
            unsigned char r = src[idx * 3 + 0];
            unsigned char g = src[idx * 3 + 1];
            unsigned char b = src[idx * 3 + 2];
            unsigned char a = alpha ? alpha[idx] : 0; // Leer el alpha o usar 255

            // Formato BGRA
            destRow[x * 4 + 0] = b;
            destRow[x * 4 + 1] = g;
            destRow[x * 4 + 2] = r;
            destRow[x * 4 + 3] = a; // Usar el canal alfa real
        }
    }

    gdipBitmap->UnlockBits(&bmpData);

    HBITMAP hBitmap = nullptr;
    // Usar Color(0, 0, 0, 0) para el color transparente/predeterminado
    gdipBitmap->GetHBITMAP(Color(0, 0, 0, 0), &hBitmap);
    delete gdipBitmap;

    return hBitmap;
}

void PrintMenu::GetPrinterNamesWindows() {
    // --- Windows: usar EnumPrinters para listar impresoras ---
    DWORD needed = 0, returned = 0; //Dword es un entero sin signo de 32 bits
    /*needed: almacenarÃÂÃÂ¡ cuÃÂÃÂ¡ntos bytes necesita el sistema para guardar la informaciÃÂÃÂ³n de todas las impresoras.
    returned: almacenarÃÂÃÂ¡ cuÃÂÃÂ¡ntas impresoras encontrÃÂÃÂ³ Windows.*/
    EnumPrinters(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS, NULL, 2, NULL, 0, &needed, &returned);
    /*EnumPrinters es una funciÃÂÃÂ³n de la WinAPI que obtiene informaciÃÂÃÂ³n sobre las impresoras instaladas.
    PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS: indica que queremos impresoras locales y de red conectadas.
    NULL: no se especifica un servidor remoto (usa la computadora local).
    2: nivel de detalle (usa la estructura PRINTER_INFO_2, que contiene nombre, puerto, estado, etc.).
    NULL: no damos aÃÂÃÂºn un bÃÂÃÂºfer donde guardar la informaciÃÂÃÂ³n.
    0: tamaÃÂÃÂ±o actual del bÃÂÃÂºfer (0, porque aÃÂÃÂºn no tenemos uno).
    &needed: aquÃÂÃÂ­ Windows escribirÃÂÃÂ¡ cuÃÂÃÂ¡ntos bytes necesita para devolver la informaciÃÂÃÂ³n.
    &returned: cantidad de impresoras devueltas (serÃÂÃÂ¡ 0 en esta primera llamada).
    Esta primera llamada no obtiene los datos; solo pregunta a Windows:*/

    std::vector<BYTE> buffer(needed);
    /*Crea un vector dinÃÂÃÂ¡mico (std::vector) de bytes (BYTE = unsigned char) con el tamaÃÂÃÂ±o exacto que Windows indicÃÂÃÂ³ en needed.
    Este vector servirÃÂÃÂ¡ como bÃÂÃÂºfer (zona de memoria temporal) donde se almacenarÃÂÃÂ¡ la informaciÃÂÃÂ³n de las impresoras.*/

    if (EnumPrinters(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS, NULL, 2, buffer.data(), needed, &needed, &returned)) {
        PRINTER_INFO_2* info = reinterpret_cast<PRINTER_INFO_2*>(buffer.data()); //Data pasa el puntero al primer elemento del vector
        /*
        Convierte (reinterpret_cast) el bloque de bytes (BYTE*) en un puntero a un arreglo de estructuras PRINTER_INFO_2.
        */
        //Reinterpret_cast se usa para conversiones de tipos que no estÃÂÃÂ¡n relacionadas, como de puntero a entero o entre punteros de diferentes tipos. Es decir, le dice que interprete los bits de un tipo como si fueran de otro, sin cambiar los bits en sÃÂÃÂ­.
        for (DWORD i = 0; i < returned; ++i) this->printerNames.Add(info[i].pPrinterName);
    }

}
#endif