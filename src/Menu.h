/*

This file is part of TinyWatchy.
Copyright 2023, Michał Szczepaniak <m.szczepaniak.000@gmail.com>

TinyWatchy is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

TinyWatchy is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

*/

#ifndef TINYWATCHY_MENU_H
#define TINYWATCHY_MENU_H

#include <ArduinoNvs.h>
#include <SmallRTC.h>
#include <map>
#include <string>

#include "AccelParser.h"
#include "AlarmHandler.h"
#include "NTP.h"
#include "Screen.h"
#include "Watchy/bma.h"

class Menu {
public:
    Menu(NTP *ntp, BMA423 *accelerometer, SmallRTC *smallRTC, Screen *screen,
         ArduinoNvs *nvs, AlarmHandler *alarmHandler);

    void handleButtonPress();
    void processPendingAction();
    bool hasPendingAction() const;

    std::string getTitle();
    std::string getDescription();
    std::vector<std::string> getMenuItems();
    bool isSubmenu() const;
    bool isMainOption() const;

private:
    enum class Level : uint8_t { WATCHFACE, MAIN_MENU, SUBMENU };
    enum class PendingAction : uint8_t { NONE, NTP_SYNC, RTC_CALIBRATION };
    enum class OperationStatus : uint8_t { IDLE, STARTED, WIFI_ERROR, SYNC_ERROR, COMPLETED };
    enum class AlarmEdit : uint8_t { NONE, HOURS, MINUTES };

    static uint8_t getButtonPressed(const uint64_t &wakeupBit);
    void next();
    void previous();
    void select();
    void back();
    void cycleAlarmValue(int direction);
    void toggleAlarm();
    void loadAlarm(uint8_t &hour, uint8_t &minute) const;
    void saveAlarm(uint8_t hour, uint8_t minute);
    uint8_t submenuSize() const;
    bool isSubmenuItemSelectable(uint8_t index) const;
    void moveSubmenuSelection(int direction);

    std::string mainTitle() const;
    static std::string mainLabel(uint8_t index);
    std::string submenuLabel(uint8_t index) const;
    std::string submenuValue(uint8_t index);
    std::string ntpSchedule() const;
    std::string alarmTime() const;
    std::string orientation() const;
    std::string macAddress() const;
    static std::string statusText(OperationStatus status);

    RTC_DATA_ATTR static Level _level;
    RTC_DATA_ATTR static uint8_t _mainIndex;
    RTC_DATA_ATTR static uint8_t _submenuIndex;
    RTC_DATA_ATTR static AlarmEdit _alarmEdit;
    RTC_DATA_ATTR static PendingAction _pendingAction;
    RTC_DATA_ATTR static OperationStatus _ntpStatus;
    RTC_DATA_ATTR static OperationStatus _rtcStatus;
    static const std::map<uint8_t, std::map<uint8_t, int>> _buttonMap;

    NTP *_ntp;
    BMA423 *_accelerometer;
    SmallRTC *_smallRTC;
    Screen *_screen;
    ArduinoNvs *_nvs;
    AlarmHandler *_alarmHandler;
};

#endif
