#!/bin/bash
# Script to create a new .po file from a .pot template

POT_FILE="locale/messages.pot"

# Check if the .pot file exists
if [ ! -f "$POT_FILE" ]; then
    echo "$POT_FILE not found. Generate the .pot file first."
    exit 1
fi

if [ "$#" -lt 1 ]; then
    echo "Usage: $0 LANG1 [LANG2 ...]"
    exit 1
fi

# Loop through all provided languages
for lang in "$@"; do
    PO_DIR="locale/$lang/LC_MESSAGES"
    PO_FILE="$PO_DIR/messages.po"

    # Create directories if they don't exist
    mkdir -p "$PO_DIR"

    # Check if the .po already exists
    if [ -f "$PO_FILE" ]; then
        echo "$PO_FILE already exists, skipping."
        continue
    fi
    sed -i '1s/^\xEF\xBB\xBF//' "$PO_FILE"
    # Create the .po file from the .pot
    msginit --input="$POT_FILE" --locale="$lang" --output-file="$PO_FILE" --no-translator

    # Remove BOM if it exists
    sed -i '1s/^\xEF\xBB\xBF//' "$PO_FILE"

    echo "$PO_FILE created from $POT_FILE and BOM removed if it existed."
done

echo "All specified languages processed."
