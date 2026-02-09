#!/bin/bash
set -e

# Ruta del directorio de recursos
RES_DIR="$HOME/Documentos/postienda/res"
cd "$RES_DIR"

PNG="favicon.png"
XPM="favicon.xpm"
VAR_NAME="app_icon_xpm"

echo "Generando icono XPM para wxWidgets (Linux)"

# Verificar PNG
if [ ! -f "$PNG" ]; then
    echo "✗ Error: no existe $PNG"
    exit 1
fi

# Eliminar XPM anterior
rm -f "$XPM"

# Convertir PNG -> XPM (ImageMagick)
if command -v magick >/dev/null 2>&1; then
    magick "$PNG" -resize 48x48 "$XPM"
elif command -v convert >/dev/null 2>&1; then
    convert "$PNG" -resize 48x48 "$XPM"
else
    echo "✗ Error: ImageMagick no está instalado"
    exit 1
fi

# Corregir la definición para wxWidgets
# static char * foo[] =
# static const char * const app_icon_xpm[] =
sed -i \
's/static char \*\s*[a-zA-Z_][a-zA-Z0-9_]*\[\]/static const char * const '"$VAR_NAME"'[]/' \
"$XPM"

echo "XPM generado correctamente"
echo "Variable: $VAR_NAME"
echo "Tamaño: 48x48"
echo ""
echo "Primera línea del archivo:"
head -n 1 "$XPM"
