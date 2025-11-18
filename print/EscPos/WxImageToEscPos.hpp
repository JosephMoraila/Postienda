// Clase para convertir wxImage a comandos ESC/POS
// AÃÂ±ade esto a tu archivo WindowsPrint.cpp o crea un nuevo archivo ImageToEscPos.cpp

#include <wx/image.h>
#include <vector>
#include <cstdint>
#include <algorithm>

class WxImageToEscPos {
public:
    // ============================================================
    // DETECTAR ANCHO DE IMPRESORA AUTOMÃÂTICAMENTE
    // ============================================================
    enum PrinterWidth {
        WIDTH_58MM = 384,   // Impresoras de 58mm (384 pixels @ 203 DPI)
        WIDTH_80MM = 576,   // Impresoras de 80mm (576 pixels @ 203 DPI)
        WIDTH_UNKNOWN = 576 // Por defecto usar 80mm
    };

    // Detectar ancho basado en el nombre de la impresora o configuraciÃÂ³n
    static int DetectPrinterWidth(const wxString& printerName) {
        wxString lowerName = printerName.Lower();

        // DetecciÃÂ³n basada en palabras clave comunes
        if (lowerName.Contains("58mm") ||
            lowerName.Contains("58") ||
            lowerName.Contains("rp58")) {
            return WIDTH_58MM;
        }

        if (lowerName.Contains("80mm") ||
            lowerName.Contains("80") ||
            lowerName.Contains("rp80") ||
            lowerName.Contains("tm-t88") ||
            lowerName.Contains("tm-t20")) {
            return WIDTH_80MM;
        }

        // Si no se puede detectar, usar 80mm por defecto (mÃÂ¡s comÃÂºn)
        return WIDTH_80MM;
    }

    // Detectar ancho ÃÂ³ptimo para la imagen (ajuste inteligente)
    static int GetOptimalWidth(const wxImage& img, int printerMaxWidth) {
        if (!img.IsOk()) return printerMaxWidth;

        int imgWidth = img.GetWidth();

        // Si la imagen es mÃÂ¡s pequeÃÂ±a que el ancho mÃÂ¡ximo, usar su tamaÃÂ±o original
        if (imgWidth <= printerMaxWidth) {
            return imgWidth;
        }

        // Si es mÃÂ¡s grande, usar el ancho mÃÂ¡ximo de la impresora
        return printerMaxWidth;
    }

    // ============================================================
    // MÃÂTODO 1: ConversiÃÂ³n Simple con Umbral
    // MÃÂ¡s rÃÂ¡pido, adecuado para imÃÂ¡genes con buen contraste
    // ============================================================
    static std::vector<uint8_t> ConvertToRasterSimple(const wxImage& img, uint8_t threshold = 128, int targetWidth = 0) {
        if (!img.IsOk()) {
            return std::vector<uint8_t>();
        }

        wxImage workImage = img;

        // Si targetWidth es 0, usar el ancho original de la imagen
        if (targetWidth > 0 && workImage.GetWidth() != targetWidth) {
            int newHeight = (workImage.GetHeight() * targetWidth) / workImage.GetWidth();
            workImage.Rescale(targetWidth, newHeight, wxIMAGE_QUALITY_HIGH);
        }

        int width = workImage.GetWidth();
        int height = workImage.GetHeight();
        int widthBytes = (width + 7) / 8;

        std::vector<uint8_t> rasterData;
        rasterData.reserve(widthBytes * height);

        unsigned char* pixels = workImage.GetData();

        // Procesar fila por fila
        for (int y = 0; y < height; y++) {
            // Procesar cada grupo de 8 pixels (1 byte)
            for (int xByte = 0; xByte < widthBytes; xByte++) {
                uint8_t byte = 0;

                // Procesar 8 pixels en este byte
                for (int bit = 0; bit < 8; bit++) {
                    int x = xByte * 8 + bit;

                    if (x < width) {
                        int idx = (y * width + x) * 3;
                        uint8_t r = pixels[idx];
                        uint8_t g = pixels[idx + 1];
                        uint8_t b = pixels[idx + 2];

                        // Convertir a escala de grises
                        uint8_t gray = (r * 299 + g * 587 + b * 114) / 1000;

                        // Si es oscuro (< threshold), poner bit en 1 (imprimir)
                        if (gray < threshold) {
                            byte |= (1 << (7 - bit));
                        }
                    }
                }

                rasterData.push_back(byte);
            }
        }

        return rasterData;
    }

    /**
        @brief ConversiÃÂ³n con Dithering Floyd-Steinberg (Mejor calidad visual, recomendado para fotos y degradados)
        @param  img - Image to convert
        @param  targetWidth - Expected width, 0 is use the image original width
        @retval - Datos binarios de la imagen en formato raster (1 bit por pÃÂ­xel)
    **/
    static std::vector<uint8_t> ConvertToRasterDithered(const wxImage& img, int targetWidth = 0) {
        if (!img.IsOk()) {
            return std::vector<uint8_t>();
        }

        wxImage workImage = img;

        if (targetWidth > 0 && workImage.GetWidth() != targetWidth) {
            int newHeight = (workImage.GetHeight() * targetWidth) / workImage.GetWidth();
            workImage.Rescale(targetWidth, newHeight, wxIMAGE_QUALITY_HIGH);
        }

        int width = workImage.GetWidth();
        int height = workImage.GetHeight();
        int widthBytes = (width + 7) / 8; //Calcula nÃÂºmero de bytes por fila (1 bit por pÃÂ­xel Ã¢ÂÂ 8 pÃÂ­xeles por byte, redondeando hacia arriba)

        unsigned char* pixels = workImage.GetData(); //Obtiene un puntero a los datos RGB crudos (3 bytes por pÃÂ­xel: R, G, B).

        // Convertir a escala de grises en buffer temporal
        std::vector<int> grayBuffer(width * height);
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int idx = (y * width + x) * 3;
                uint8_t r = pixels[idx];
                uint8_t g = pixels[idx + 1];
                uint8_t b = pixels[idx + 2];
                grayBuffer[y * width + x] = (r * 299 + g * 587 + b * 114) / 1000;
            }
        }

        // Aplicar dithering Floyd-Steinberg
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int idx = y * width + x;
                int oldPixel = grayBuffer[idx];
                int newPixel = (oldPixel < 128) ? 0 : 255;
                grayBuffer[idx] = newPixel;

                int error = oldPixel - newPixel;

                // Distribuir el error a pÃÂ­xeles vecinos
                if (x + 1 < width) grayBuffer[idx + 1] = std::clamp(grayBuffer[idx + 1] + error * 7 / 16, 0, 255);

                if (y + 1 < height) {
                    if (x > 0) grayBuffer[idx + width - 1] = std::clamp(grayBuffer[idx + width - 1] + error * 3 / 16, 0, 255);
                    grayBuffer[idx + width] = std::clamp(grayBuffer[idx + width] + error * 5 / 16, 0, 255);
                    if (x + 1 < width) grayBuffer[idx + width + 1] = std::clamp(grayBuffer[idx + width + 1] + error * 1 / 16, 0, 255);
                }
            }
        }

        // Convertir a formato raster
        std::vector<uint8_t> rasterData;
        rasterData.reserve(widthBytes * height);

        for (int y = 0; y < height; y++) {
            for (int xByte = 0; xByte < widthBytes; xByte++) {
                uint8_t byte = 0;

                for (int bit = 0; bit < 8; bit++) {
                    int x = xByte * 8 + bit;

                    if (x < width) {
                        int gray = grayBuffer[y * width + x];

                        if (gray < 128) {
                            byte |= (1 << (7 - bit));
                        }
                    }
                }

                rasterData.push_back(byte);
            }
        }

        return rasterData;
    }

    // ============================================================
    // CREAR COMANDO COMPLETO ESC/POS (GS v 0)
    // ============================================================
    static std::string CreateRasterCommand(
        int width,
        int height,
        const std::vector<uint8_t>& rasterData,
        uint8_t density = 0  // 0=normal, 1=doble ancho, 2=doble alto, 3=cuÃÂ¡druple
    ) {
        std::string cmd;

        // GS v 0
        cmd.push_back(0x1D);  // GS
        cmd.push_back(0x76);  // v
        cmd.push_back(0x30);  // 0
        cmd.push_back(density);

        // Ancho en bytes (little-endian)
        int widthBytes = (width + 7) / 8;
        cmd.push_back(widthBytes & 0xFF);
        cmd.push_back((widthBytes >> 8) & 0xFF);

        // Alto en pixels (little-endian)
        cmd.push_back(height & 0xFF);
        cmd.push_back((height >> 8) & 0xFF);

        // Datos de la imagen
        cmd.append(rasterData.begin(), rasterData.end());

        return cmd;
    }

    // ============================================================
    // FUNCIÃÂN COMPLETA: wxImage Ã¢ÂÂ Comandos ESC/POS
    // CON DETECCIÃÂN AUTOMÃÂTICA DE ANCHO
    // ============================================================
    static std::string WxImageToEscPosCommands(const wxImage& img, const wxString& printerName, bool useDithering = true, bool centerImage = true) {
        if (!img.IsOk()) return "";

        // DETECCIÃÂN AUTOMÃÂTICA: Obtener el ancho mÃÂ¡ximo de la impresora
        int printerMaxWidth = DetectPrinterWidth(printerName);

        std::string commands;

        // Centrar imagen si se solicita
        if (centerImage) commands.append("\x1B\x61\x01", 3);  // ESC a 1 (centrar)

        wxImage workImage = img;

        // Ajustar ancho automÃÂ¡ticamente
        int targetWidth = GetOptimalWidth(workImage, printerMaxWidth);

        // Redimensionar si es necesario
        if (workImage.GetWidth() > targetWidth) {
            int newHeight = (workImage.GetHeight() * targetWidth) / workImage.GetWidth();
            workImage.Rescale(targetWidth, newHeight, wxIMAGE_QUALITY_HIGH);
        }

        int finalWidth = workImage.GetWidth();
        int finalHeight = workImage.GetHeight();

        // Convertir imagen a raster
        std::vector<uint8_t> rasterData;
        if (useDithering) rasterData = ConvertToRasterDithered(workImage, 0); // 0 = usar tamaÃÂ±o actual
        else rasterData = ConvertToRasterSimple(workImage, 128, 0);

        // Fragmentar si la imagen es muy alta (mÃÂ¡s de 1024 pixels)
        const int MAX_HEIGHT = 1024;
        if (finalHeight > MAX_HEIGHT) {
            int widthBytes = (finalWidth + 7) / 8;
            int numFragments = (finalHeight + MAX_HEIGHT - 1) / MAX_HEIGHT;

            for (int i = 0; i < numFragments; i++) {
                int fragmentHeight = std::min(MAX_HEIGHT, finalHeight - i * MAX_HEIGHT);
                int startByte = i * MAX_HEIGHT * widthBytes;
                int fragmentSize = fragmentHeight * widthBytes;

                std::vector<uint8_t> fragmentData(
                    rasterData.begin() + startByte,
                    rasterData.begin() + startByte + fragmentSize
                );

                commands += CreateRasterCommand(finalWidth, fragmentHeight, fragmentData, 0);
            }
        }
        else {
            // Imagen completa
            commands += CreateRasterCommand(finalWidth, finalHeight, rasterData, 0);
        }

        // Restaurar alineaciÃÂ³n a la izquierda
        if (centerImage) {
            commands.append("\x1B\x61\x00", 3);  // ESC a 0 (izquierda)
        }

        // Avanzar papel
        commands.append("\n\n", 2);

        return commands;
    }

    /**
        @brief Convierte una imagen wxImage en comandos ESC/POS listos para impresiÃÂ³n.
        @details Esta funciÃÂ³n convierte una imagen a formato binario (raster) compatible con impresoras tÃÂ©rmicas ESC/POS, aplicando opcionalmente dithering y centrado.Si la imagen es muy alta, la divide en fragmentos para evitar errores de buffer.
        @param img           - Imagen wxImage a convertir.
        @param useDithering  - Si es true, aplica dithering FloydÃ¢ÂÂSteinberg para mejor calidad visual.
        @param centerImage   - Si es true, centra la imagen en la impresora con comandos ESC/POS.
        @param maxWidth      - Ancho mÃÂ¡ximo permitido en pÃÂ­xeles (0 para usar el ancho original).
        @return std::string  - Cadena con los comandos ESC/POS listos para enviar a la impresora.
    **/
    static std::string WxImageToEscPosCommandsManual(const wxImage& img, bool useDithering = true, bool centerImage = true, int maxWidth = 0) {
        // Si la imagen no es vÃÂ¡lida, no hacemos nada
        if (!img.IsOk()) return "";

        // Cadena donde se acumularÃÂ¡n todos los comandos ESC/POS
        std::string commands;

        // Si se solicita centrado, enviar comando ESC a 1 (centrar)
        if (centerImage) commands.append("\x1B\x61\x01", 3);  // ESC a 1 = centrar

        // Crear una copia de la imagen original para manipularla
        wxImage workImage = img;

        // Determinar el ancho final objetivo (maxWidth si se especifica)
        int targetWidth = (maxWidth > 0) ? maxWidth : workImage.GetWidth();

        // Si la imagen es mÃÂ¡s ancha que el mÃÂ¡ximo permitido, se redimensiona proporcionalmente
        if (workImage.GetWidth() > targetWidth) {
            int newHeight = (workImage.GetHeight() * targetWidth) / workImage.GetWidth();
            workImage.Rescale(targetWidth, newHeight, wxIMAGE_QUALITY_HIGH);
        }

        // Guardar las dimensiones finales
        int finalWidth = workImage.GetWidth();
        int finalHeight = workImage.GetHeight();

        // Vector que contendrÃÂ¡ los datos de imagen convertidos a blanco/negro
        std::vector<uint8_t> rasterData;

        // Convertir la imagen a formato raster, con o sin dithering
        if (useDithering) {
            // Usa el algoritmo FloydÃ¢ÂÂSteinberg para mejorar la calidad (fotos y degradados)
            rasterData = ConvertToRasterDithered(workImage, 0);
        }
        else {
            // Usa una conversiÃÂ³n simple por umbral fijo (mÃÂ¡s rÃÂ¡pida, menos calidad)
            rasterData = ConvertToRasterSimple(workImage, 128, 0);
        }

        // MÃÂ¡xima altura que puede imprimirse de una sola vez (por limitaciÃÂ³n del buffer de impresora)
        const int MAX_HEIGHT = 1024;

        // Si la imagen supera la altura mÃÂ¡xima, se fragmenta en bloques
        if (finalHeight > MAX_HEIGHT) {
            int widthBytes = (finalWidth + 7) / 8;  // bytes por lÃÂ­nea
            int numFragments = (finalHeight + MAX_HEIGHT - 1) / MAX_HEIGHT; // nÃÂºmero total de fragmentos

            // Procesar cada fragmento por separado
            for (int i = 0; i < numFragments; i++) {
                int fragmentHeight = std::min(MAX_HEIGHT, finalHeight - i * MAX_HEIGHT);
                int startByte = i * MAX_HEIGHT * widthBytes;
                int fragmentSize = fragmentHeight * widthBytes;

                // Extraer el bloque correspondiente de datos raster
                std::vector<uint8_t> fragmentData(
                    rasterData.begin() + startByte,
                    rasterData.begin() + startByte + fragmentSize
                );

                // Generar el comando raster ESC/POS correspondiente para ese fragmento
                commands += CreateRasterCommand(finalWidth, fragmentHeight, fragmentData, 0);
            }
        }
        else {
            // Si no se necesita fragmentar, generar un solo comando raster completo
            commands += CreateRasterCommand(finalWidth, finalHeight, rasterData, 0);
        }

        // Restaurar alineaciÃÂ³n a la izquierda (ESC a 0)
        if (centerImage) {
            commands.append("\x1B\x61\x00", 3);  // ESC a 0 = alineaciÃÂ³n izquierda
        }

        // Agregar saltos de lÃÂ­nea despuÃÂ©s de la imagen para separaciÃÂ³n
        commands.append("\n\n", 2);

        // Retornar los comandos completos listos para enviar a la impresora
        return commands;
    }



    // ============================================================
    // EJEMPLO DE USO
    // ============================================================

    void EjemploConDeteccionAutomatica() {
        wxImage myImage("logo.png", wxBITMAP_TYPE_PNG);
        wxString printerName = "TM-T20II"; // O cualquier nombre de impresora

        // La funciÃÂ³n detecta automÃÂ¡ticamente si es 58mm o 80mm
        std::string commands = WxImageToEscPos::WxImageToEscPosCommands(
            myImage,
            printerName,  // Detecta automÃÂ¡ticamente el ancho
            true,         // dithering
            true          // centrar
        );
    }

    void EjemploSinEspecificarAncho() {
        wxImage myImage("logo.png", wxBITMAP_TYPE_PNG);

        // Si no quieres especificar nada, usa la versiÃÂ³n manual con 0
        // Esto usarÃÂ¡ el tamaÃÂ±o original de la imagen (sin redimensionar)
        std::string commands = WxImageToEscPos::WxImageToEscPosCommandsManual(
            myImage,
            true,  // dithering
            true,  // centrar
            0      // 0 = no redimensionar, usar tamaÃÂ±o original
        );
    }

};