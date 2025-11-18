#include <wx/string.h>
#include <wx/image.h>
#pragma once

/**
    @struct ContenidoLinea
    @brief Guarda la informacion insertada en un rich input incluyendo imagenes
**/
struct ContenidoLinea {
    enum Type { TEXT, IMAGE, PURCHASE_INFO } type;
    int numero; ///Rich input line where the text or image was inserted
    bool esImagen;
    wxString texto;
    wxImage imagen;  // Solo vÃ¡lido si esImagen = true
};