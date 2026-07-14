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
- The watch face sits outside the cyclic menu. `OK` opens the menu, `BACK` returns to the watch face, and the navigation buttons cycle through entries.
- The main menu and each submenu display all entries on one page using a smaller font; the active entry is marked with `<-`.
- Read-only submenu entries are displayed without `<-` and are skipped by cyclic navigation.
- The main menu labels all four physical buttons according to the selected `BUTTON_MAP` (`LEFT`/`RIGHT` or `UP`/`DOWN`, plus `OK` and `BACK`).
- `WATCHFACE > BTN Layout` changes the active button map between `0`, `1`, and `2`; the choice is stored in NVS, while `BUTTON_MAP` remains the default for a fresh installation.
- The menu groups time synchronization and RTC calibration, alarm status and time, watch-face selection, Wi-Fi configuration, device properties, and firmware information.
- Alarm hours and minutes are edited separately; minutes change in ten-minute increments. Changes are stored when leaving the submenu with `BACK`.
- The physical button layout can be selected with `BUTTON_MAP`.

## Sensors and alarms

- BMA423 acceleration data supports wrist-orientation filtering, wake-up interrupts, and step counting.
- User alarms are stored in NVS and scheduled together with system wake-up alarms.
- Battery voltage is converted to an approximate percentage for watch-face display.

## Extensibility

- Watch faces implement `AbstractFace` and are registered by `Screen`.
- `Menu` owns the retained watchface/main-menu/submenu state machine and the fixed category layout.
- Fonts and monochrome images are generated into `src/resources.h` from source assets.
