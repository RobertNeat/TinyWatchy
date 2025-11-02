# Font Management System

## Directory Structure
- `source-fonts/` - Contains all original `.ttf` font files
- `font_config.json` - Configuration file that defines which fonts should be generated and at what sizes

## How to Add New Fonts

1. Add your `.ttf` font file to the `source-fonts/` directory
2. Edit `font_config.json` to specify which sizes you want for your font:
   ```json
   {
     "your-font.ttf": [8, 12, 22],
     "another-font.ttf": [15, 18, 50]
   }
   ```
3. Run `./generate.sh` from the resources directory

## Accessing Fonts in Code

Generated fonts will be available in `resources.h` with the following naming pattern:
- `FONTNAME_SIZE_BITMAPS`
- `FONTNAME_SIZE_GLYPHS`
- `FONTNAME_SIZE`

Example:
```cpp
// If you have digital7.ttf configured for size 22
// You can access it as:
DIGITAL7_22
```

## Notes
- Font names in the configuration will be converted to uppercase in the generated code
- Special characters and spaces in font names will be removed
- Original font licenses should be kept alongside their respective font files in the `source-fonts` directory
- The script requires the `jq` tool for JSON processing (install with `sudo apt-get install jq` on Debian/Ubuntu)
- Fonts are processed directly from the `source-fonts` directory without creating intermediate directories
- The generated font data is automatically added to `resources.h`


## Flow
source-fotns + font_config.json --> src/resources.h