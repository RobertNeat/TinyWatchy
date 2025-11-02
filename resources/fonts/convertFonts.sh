#!/bin/bash

fontconvert="../../.pio/libdeps/esp32dev/Adafruit GFX Library/fontconvert/fontconvert"
if [ -e "$fontconvert" ]; then
    echo "fontconvert exists"
else
    echo "fontconvert does not exist, creating it..."
    cd ../../.pio/libdeps/esp32dev/Adafruit\ GFX\ Library/fontconvert/
    make -j$(nproc)
    cd ../../../../../resources/fonts/
    if [ ! -e "$fontconvert" ]; then
        echo "Couldn't compile fontconvert, exiting"
        exit 1
    fi
fi

# Check if jq is installed (needed for JSON parsing)
if ! command -v jq &> /dev/null; then
    echo "Error: jq is not installed. Please install it first:"
    echo "sudo apt-get install jq  # For Debian/Ubuntu"
    echo "sudo dnf install jq      # For Fedora"
    echo "brew install jq          # For macOS"
    exit 1
fi

# Clean up existing size directories
find . -type d -regex './[0-9]+' -exec rm -rf {} +

# Process font configuration
echo "Reading font configuration..."
jq -r 'to_entries | .[] | "\(.key)|\(.value[])"' font_config.json | while IFS='|' read -r font size
do
    # Skip if the font file doesn't exist in source-fonts
    if [ ! -f "source-fonts/$font" ]; then
        echo "Warning: Font file 'source-fonts/$font' not found, skipping..."
        continue
    fi

    echo "Processing $font at size $size"
    
    # Process the font directly from source
    baseFilename="$(basename -- "$font")"
    filename="${baseFilename%.*}"
    filenameUpper="${filename^^}"
    
    ./"$fontconvert" "source-fonts/$font" "$size" | \
        sed 's/\w*Bitmaps/'${filenameUpper}'_'${size}'_BITMAPS/' | \
        sed 's/\w*Glyphs/'${filenameUpper}'_'${size}'_GLYPHS/' | \
        sed 's/GFXfont \w*/GFXfont '${filenameUpper}'_'${size}'/' | \
        grep -v '// Approx.*' | \
        sed 's/PROGMEM //' >> ../resources.h
done
