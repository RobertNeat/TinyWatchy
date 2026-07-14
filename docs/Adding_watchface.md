# Adding a watch face

A watch face implements `AbstractFace` and is owned by `Screen`.

## Interface

The required interface is:

```cpp
class ExampleFace : public AbstractFace {
public:
    explicit ExampleFace(
        GxEPD2_BW<WatchyDisplay, WatchyDisplay::HEIGHT> *display
    );

    void draw(const ScreenInfo &screenInfo) override;
    std::string getName() override;
};
```

The base constructor receives the display instance and exposes it to derived classes as `_display`. `getName()` is shown by the watch-face selection menu.

## Available data

`draw()` receives the current `ScreenInfo`:

```cpp
struct ScreenInfo {
    DateTime time{};
    uint32_t steps = 0;
    uint8_t battery = 0;
    std::string title;
    std::string description;
    bool onMainOption = true;
    bool humanInSleep = false;
};
```

A face decides which sensor data fits its design. It should render `title` and `description` when `onMainOption` is false so menu navigation remains usable.

## Drawing

Set the font, text color, and cursor before printing text:

```cpp
_display->setFont(&resources::DIGITAL_DISPLAY_REGULAR_50);
_display->setTextColor(GxEPD_BLACK);
_display->setCursor(5, 68);
_display->print("12:34");
```

Use `getTextBounds()` when text must be centered. The base class provides `drawBatteryIcon()` for the standard battery outline.

Generated bitmaps can be drawn with:

```cpp
_display->drawBitmap(
    x,
    y,
    resources::EXAMPLE_IMAGE,
    EXAMPLE_IMAGE_WIDTH,
    EXAMPLE_IMAGE_HEIGHT,
    GxEPD_BLACK
);
```

See [Resources](Resources.md) for adding fonts and images.

## Registration

1. Add the new `.h` and `.cpp` files under `src/Faces/`.
2. Include the header in `src/Screen.cpp`.
3. Add the face to the constructor after the existing entries:

   ```cpp
   _faces.emplace_back(std::make_unique<ExampleFace>(display));
   ```

4. Regenerate resources if the face added assets.
5. Build the firmware and select the face from **Settings > Watchface**.

The order in `_faces` is the stored numeric selection order. Avoid reordering existing entries unless migration of the NVS value is intentional.
