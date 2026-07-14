# TinyWatchy

TinyWatchy is alternative firmware for the Watchy e-paper watch. It focuses on low power consumption, offline operation, a compact menu, and replaceable watch faces.

The current hardware configuration supports Watchy V2 with the PCF8563 RTC.

## Features

- Four selectable watch faces: HelloFace, JetBrains, Default, and UwU.
- Configurable RTC wake-up interval and sleep schedule.
- Manual and scheduled NTP synchronization over Wi-Fi.
- Wi-Fi is enabled only when needed, and Bluetooth is disabled during startup.
- Accelerometer wake-up, step counting, alarms, RTC drift adjustment, and battery display.
- Extensible watch-face and menu-option interfaces.

See [the feature reference](docs/Features.md) for details.

## Setting up the project on a new machine

1. Install Visual Studio Code and the recommended **PlatformIO IDE** extension.
2. Open the project directory containing `platformio.ini`, not its parent directory.
3. In a PlatformIO Core CLI terminal, install dependencies and generate machine-local Visual Studio Code metadata:

   ```bash
   pio project init --ide vscode --environment esp32dev
   ```

   This recreates `.vscode/c_cpp_properties.json` and `.vscode/launch.json` with paths valid on the current machine. Both files are ignored by Git. Do not copy them between operating systems or maintain their include paths manually.
4. Create the local configuration file from the tracked template:

   ```powershell
   # Windows PowerShell
   Copy-Item src/defines_private.h.template src/defines_private.h
   ```

   ```bash
   # Linux or macOS
   cp src/defines_private.h.template src/defines_private.h
   ```

5. Edit `src/defines_private.h`. It is ignored by Git and contains settings that may differ between devices or installations:

   - `WIFI_SSID`, `WIFI_PASS`, `WIFI_HOSTNAME`, and `WIFI_TIMEOUT`;
   - `TIMEZONE`, `TIMEZONE_STANDARD`, and `AUTO_DST_DEFAULT`; the Polish POSIX rule is `CET-1CEST,M3.5.0/2,M10.5.0/3`;
   - `SLEEP_START`, `SLEEP_END`, and `WAKEUP_INTERVAL_MINUTES`;
   - `NTP_SYNC_ENABLED`, `NTP_SYNC_HOUR`, `NTP_SYNC_DAY1`, and `NTP_SYNC_DAY2`;
   - `BUTTON_MAP`.

   Application code includes `defines.h`, which propagates this configuration and validates its numeric ranges at compile time. Update `src/defines_private.h.template` whenever a configurable value is added.
6. Generate `src/resources.h`:

   ```bash
   cd resources
   bash generate.sh
   cd ..
   ```

   The generator requires Bash, ImageMagick, `xxd`, `jq`, `make`, a C compiler, and FreeType development headers. On Windows, use WSL, a suitably configured Git Bash environment, or a Linux container. Windows `convert.exe` is not ImageMagick. See [the resource guide](docs/Resources.md).
7. If Visual Studio Code still displays stale include errors, run these commands from the Command Palette:

   1. **PlatformIO: Rebuild IntelliSense Index**
   2. **Developer: Reload Window**

8. Build the firmware:

   ```bash
   pio run --environment esp32dev
   ```

9. Connect the watch and upload the firmware:

   ```bash
   pio run --environment esp32dev --target upload
   ```

Build and upload are also available under **PlatformIO: Project Tasks** in Visual Studio Code.

## Documentation

Start with the [documentation index](docs/Index.md). It includes the architecture overview and guides for adding watch faces, menu options, fonts, and images.

## Contributing

Bug reports, feature requests, and pull requests are welcome. Before submitting a change, build the `esp32dev` environment and keep generated or machine-local files out of Git.

## License

TinyWatchy is licensed under GPLv3. See [COPYING](COPYING).

## Acknowledgements

This project uses parts of the original [Watchy firmware](https://github.com/sqfmi/Watchy/), licensed under the MIT License, and the Bosch BMA423 driver.
