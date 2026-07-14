# Features

TinyWatchy currently targets Watchy V2 hardware with the PCF8563 RTC.

## Time and power management

- The PCF8563 RTC supplies time and wakes the ESP32 according to the configured schedule.
- `WAKEUP_INTERVAL_MINUTES` controls periodic display updates during active hours.
- `SLEEP_START` and `SLEEP_END` define hours in which periodic wake-ups are skipped.
- The accelerometer is disabled when the RTC alarm handler enters the configured sleep period and enabled again during active hours.
- Bluetooth is stopped during every startup. Wi-Fi is connected only for NTP operations and disconnected immediately afterwards.

Power consumption depends on the selected watch face, interaction frequency, Wi-Fi conditions, battery condition, and configured wake interval; it should be measured on the target device rather than treated as a fixed runtime guarantee.

## Time synchronization

- Time can be synchronized manually from the NTP menu option.
- Scheduled NTP synchronization is controlled by `NTP_SYNC_ENABLED`, `NTP_SYNC_HOUR`, `NTP_SYNC_DAY1`, and `NTP_SYNC_DAY2`.
- A successful scheduled synchronization is recorded in NVS to avoid repeating it on the same date.
- `TIMEZONE` uses the POSIX timezone format and is applied when RTC time is converted for display.

## User interface

- Watch faces can be selected at runtime and the selection is stored in NVS.
- Included faces are HelloFace, JetBrains, Default, and UwU.
- The menu provides manual NTP sync, alarms, accelerometer information, battery voltage, watch-face selection, RTC drift configuration, UI testing, and firmware information.
- The physical button layout can be selected with `BUTTON_MAP`.

## Sensors and alarms

- BMA423 acceleration data supports wrist-orientation filtering, wake-up interrupts, and step counting.
- User alarms are stored in NVS and scheduled together with system wake-up alarms.
- Battery voltage is converted to an approximate percentage for watch-face display.

## Extensibility

- Watch faces implement `AbstractFace` and are registered by `Screen`.
- Menu features implement `AbstractOption` and are registered in the page structure owned by `Menu`.
- Fonts and monochrome images are generated into `src/resources.h` from source assets.
