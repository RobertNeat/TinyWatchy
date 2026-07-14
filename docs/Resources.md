# Resources

Fonts and images are converted into C++ data stored in the generated `src/resources.h`. The generated file is intentionally ignored by Git; source assets and configuration are tracked instead.

Install PlatformIO dependencies before running the generator because it builds and uses Adafruit GFX `fontconvert` from `.pio/libdeps/esp32dev/Adafruit GFX Library/fontconvert/`.

## Generator requirements

`resources/generate.sh` requires:

- Bash;
- ImageMagick commands `identify` and `convert`;
- `xxd`, `jq`, `make`, and a C compiler;
- FreeType development headers.

Run it from the `resources/` directory:

```bash
bash generate.sh
```

On Windows, use WSL, a Git Bash environment containing all required Unix tools, or a Linux container. The `convert.exe` included with Windows is not ImageMagick.

## Fonts

1. Put the `.ttf` file in `resources/fonts/source-fonts/`.
2. Put its license in `resources/fonts/source-fonts/licenses/`.
3. Add the filename and required pixel sizes to `resources/fonts/font_config.json`:

   ```json
   {
     "example_font_regular.ttf": [10, 16]
   }
   ```

4. Run the generator.

Names are derived from the snake_case filename and size. For example, `example_font_regular.ttf` at size `16` becomes:

```cpp
resources::EXAMPLE_FONT_REGULAR_16
```

## Images

1. Put a `.png` file under `resources/images/`. Subdirectories are allowed.
2. Use a unique snake_case basename; directory names are not included in the generated symbol, so duplicate basenames would collide.
3. Run the generator.

The generator dithers and remaps images to the two-color `eink_2color.png` palette. A file named `example.png` produces:

```cpp
resources::EXAMPLE_IMAGE
EXAMPLE_IMAGE_WIDTH
EXAMPLE_IMAGE_HEIGHT
```

Prefer images designed for a 1-bit display to keep the result predictable.

## Private resources

The generator also processes `resources/fonts/private/` when present. Private asset directories and the generated header are ignored by Git; do not rely on them for watch faces intended to build on every clone.
