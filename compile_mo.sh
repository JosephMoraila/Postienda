#!/bin/bash
# Script to compile .po files into .mo files for the specified languages
# Removes BOM from .po before compiling

if [ "$#" -lt 1 ]; then
    echo "Usage: $0 LANG1 [LANG2 ...]"
    exit 1
fi

for lang in "$@"; do
    PO_FILE="locale/$lang/LC_MESSAGES/messages.po"
    MO_FILE="locale/$lang/LC_MESSAGES/messages.mo"

    if [ ! -f "$PO_FILE" ]; then
        echo "PO file not found for language '$lang': $PO_FILE"
        continue
    fi

    # Remove BOM if it exists
    sed -i '1s/^\xEF\xBB\xBF//' "$PO_FILE"

    echo "Compiling $PO_FILE to $MO_FILE..."
    msgfmt "$PO_FILE" -o "$MO_FILE"

    if [ $? -eq 0 ]; then
        echo "Compiled successfully: $MO_FILE"
    else
        echo "Error compiling $PO_FILE"
    fi
done

echo "All specified languages processed."
