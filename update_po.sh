#!/bin/bash
# Script to update .po files from messages.pot
# Usage: ./update_po.sh en es es_MX

# Check if at least one language is provided
if [ $# -lt 1 ]; then
    echo "Usage: $0 language1 [language2 ...]"
    exit 1
fi

POT_FILE="locale/messages.pot"

# Check if the .pot file exists
if [ ! -f "$POT_FILE" ]; then
    echo "$POT_FILE not found. Generate the .pot file first."
    exit 1
fi

# Loop through all provided languages
for lang in "$@"; do
    PO_FILE="locale/$lang/LC_MESSAGES/messages.po"

    if [ ! -f "$PO_FILE" ]; then
        echo "File $PO_FILE not found, skipping."
        continue
    fi

    sed -i '1s/^\xEF\xBB\xBF//' "$PO_FILE"

    echo "Updating $PO_FILE..."
    msgmerge --update --backup=none "$PO_FILE" "$POT_FILE"

    # Remove BOM if it exists
    sed -i '1s/^\xEF\xBB\xBF//' "$PO_FILE"

    echo "$PO_FILE updated and BOM removed if it existed."
done

echo "All specified languages processed."
