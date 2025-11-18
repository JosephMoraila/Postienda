#!/bin/bash
# Script para generar el archivo .pot a partir del código fuente C++

# Ruta de salida del .pot
mkdir -p locale
OUTPUT="locale/messages.pot"

# Buscar todos los archivos .cpp, .h, .hpp
FILES=$(find . -name "*.cpp" -o -name "*.h" -o -name "*.hpp")

# Ejecutar xgettext
xgettext \
    --language=C++ \
    --keyword=_ \
    --from-code=UTF-8 \
    --no-wrap \
    --output="$OUTPUT" \
    $FILES

sed -i '1s/^\xEF\xBB\xBF//' $OUTPUT

echo "File $OUTPUT done"
