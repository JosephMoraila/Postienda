#pragma once

#include <cstdint>

namespace ESC_POS_DIC {

    // SÃ­mbolos comunes y su valor hexadecimal

    inline constexpr unsigned char ESC = 0x1B;  /// Escape
    inline constexpr unsigned char FS = 0x1C;   // File Separator
    inline constexpr unsigned char GS = 0x1D;   // Group Separator
    inline constexpr unsigned char DLE = 0x10;  // Data Link Escape
    inline constexpr unsigned char EOT = 0x04;  // End of Transmission
    inline constexpr unsigned char NUL = 0x00;  /// Null
    inline constexpr unsigned char START_HEADING = 0x01; /// START OF HEADING
    inline constexpr unsigned char FF = 0x0C;   // Form Feed (Avanza pÃ¡gina)
    inline constexpr unsigned char CR = 0x0D;   // Carriage Return
    inline constexpr unsigned char LF = 0x0A;   // Line Feed (Salto de lÃ­nea)

    // Para los comandos que son letras/sÃ­mbolos, puedes usar el literal de carÃ¡cter si lo prefieres:
    inline constexpr unsigned char AT = '@';    /// '@' sÃ­mbolo, equivalente a 0x40
    inline constexpr unsigned char EXCLAMATION = '!'; /// ! Sybmol that represents 0x21
    inline constexpr unsigned char EXCLAM = '!'; // '!' sÃ­mbolo, equivalente a 0x21
    inline constexpr unsigned char STAR = '*';   // '*' sÃ­mbolo, equivalente a 0x2A
    inline constexpr unsigned char a = 'a'; /// 'a' symbol, represents 0x61
    inline constexpr unsigned char M = 'M'; /// 'M' symbol represents 0x4D
	inline constexpr unsigned char L = 'L';     // 'L' sÃ­mbolo, equivalente a 0x4C
	inline constexpr unsigned char W = 'W';     // 'W' sÃ­mbolo, equivalente a 0x57
    inline constexpr unsigned char V = 'V'; /// 'V' simbol, represents 0x56

	inline constexpr unsigned char INIT[] = { ESC, AT };/// Inicializa la impresora
	inline constexpr unsigned char PAGE_MODE[] = { ESC, L }; // Cambia a modo pÃ¡gina
    inline constexpr unsigned char ALIGN_CENTER[] = { ESC, a, START_HEADING };
    inline constexpr unsigned char TOTAL_CUT[] = { GS, V, NUL }; //Agregar salto de lÃ­nea final y corte de papel
    inline constexpr unsigned char LITTLE_FONT[] = {ESC, M, START_HEADING}; /// Little font
}
