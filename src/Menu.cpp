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

#include "Menu.h"

#include <Arduino.h>
#include <WiFi.h>
#include <esp_sleep.h>
#include <esp32-hal-adc.h>
#include <esp_arduino_version.h>
#include <cstdio>
#include <sstream>

#include "buttons.h"
#include "defines.h"

#ifndef RELEASE
#define RELEASE "unknown"
#endif

Menu::Level Menu::_level = Menu::Level::WATCHFACE;
uint8_t Menu::_mainIndex = 0;
uint8_t Menu::_submenuIndex = 0;
Menu::AlarmEdit Menu::_alarmEdit = Menu::AlarmEdit::NONE;
Menu::PendingAction Menu::_pendingAction = Menu::PendingAction::NONE;
Menu::OperationStatus Menu::_ntpStatus = Menu::OperationStatus::IDLE;
Menu::OperationStatus Menu::_rtcStatus = Menu::OperationStatus::IDLE;
uint8_t Menu::_buttonLayoutSelection = BUTTON_MAP;
bool Menu::_timeZoneChanged = false;

const std::map<uint8_t, std::map<uint8_t, int>> Menu::_buttonMap = {
    {0, {{RIGHT_BTN_PIN, Button::RIGHT}, {LEFT_BTN_PIN, Button::LEFT},
         {SELECT_BTN_PIN, Button::SELECT}, {BACK_BTN_PIN, Button::BACK}}},
    {1, {{RIGHT_BTN_PIN, Button::SELECT}, {LEFT_BTN_PIN, Button::BACK},
         {SELECT_BTN_PIN, Button::RIGHT}, {BACK_BTN_PIN, Button::LEFT}}},
    {2, {{RIGHT_BTN_PIN, Button::RIGHT}, {LEFT_BTN_PIN, Button::SELECT},
         {SELECT_BTN_PIN, Button::LEFT}, {BACK_BTN_PIN, Button::BACK}}},
    {3, {{RIGHT_BTN_PIN, Button::RIGHT}, {LEFT_BTN_PIN, Button::BACK},
         {SELECT_BTN_PIN, Button::LEFT}, {BACK_BTN_PIN, Button::SELECT}}},
};

Menu::Menu(NTP *ntp, BMA423 *accelerometer, SmallRTC *smallRTC, Screen *screen,
           ArduinoNvs *nvs, AlarmHandler *alarmHandler)
    : _ntp(ntp), _accelerometer(accelerometer), _smallRTC(smallRTC), _screen(screen),
      _nvs(nvs), _alarmHandler(alarmHandler) {}

void Menu::handleButtonPress() {
    const uint64_t wakeupBit = esp_sleep_get_ext1_wakeup_status();
    if (!(wakeupBit & (RIGHT_BTN_MASK | LEFT_BTN_MASK | SELECT_BTN_MASK | BACK_BTN_MASK))) return;

    Accel data;
    if (_accelerometer->getAccel(data) &&
        AccelParser::normalizeOrientation(data) == AccelParser::RIGHT_EDGE) return;

    const int button = _buttonMap.at(getButtonLayout()).at(getButtonPressed(wakeupBit));
    switch (button) {
        case Button::RIGHT: next(); break;
        case Button::LEFT: previous(); break;
        case Button::SELECT: select(); break;
        case Button::BACK: back(); break;
    }
}

bool Menu::hasPendingAction() const { return _pendingAction != PendingAction::NONE; }

bool Menu::consumeTimeZoneChanged() {
    const bool changed = _timeZoneChanged;
    _timeZoneChanged = false;
    return changed;
}

void Menu::processPendingAction() {
    if (_pendingAction == PendingAction::NTP_SYNC) {
        const NTP::SyncResult result = _ntp->syncWithStatus();
        _ntpStatus = result == NTP::SyncResult::COMPLETED ? OperationStatus::COMPLETED :
                     result == NTP::SyncResult::WIFI_ERROR ? OperationStatus::WIFI_ERROR :
                     OperationStatus::SYNC_ERROR;
    } else if (_pendingAction == PendingAction::RTC_CALIBRATION) {
        NTP::SyncResult result;
        time_t ntpTime = _ntp->getTime(&result);
        if (result != NTP::SyncResult::COMPLETED || ntpTime == 0) {
            _rtcStatus = OperationStatus::SYNC_ERROR;
        } else {
            tmElements_t time;
            _smallRTC->doBreakTime(ntpTime, time);
            const bool inProgress = _nvs->getInt("rtc_calibrating", 0);
            if (!inProgress) {
                _smallRTC->beginDrift(time, false);
                _nvs->setInt("rtc_calibrating", 1);
                _rtcStatus = OperationStatus::STARTED;
            } else {
                _smallRTC->endDrift(time, false);
                _nvs->setInt("drift", _smallRTC->getDrift(false));
                _nvs->setInt("drift_fast", _smallRTC->isFastDrift(false));
                _nvs->setInt("rtc_calibrating", 0);
                _rtcStatus = OperationStatus::COMPLETED;
            }
        }
    }
    _pendingAction = PendingAction::NONE;
}

std::string Menu::getTitle() {
    if (_level == Level::WATCHFACE) return {};
    return _level == Level::MAIN_MENU ? "MENU" : mainTitle();
}

std::string Menu::getDescription() {
    return {};
}

std::vector<std::string> Menu::getMenuItems() {
    std::vector<std::string> items;
    if (_level == Level::WATCHFACE) return items;

    if (_level == Level::MAIN_MENU) {
        items.reserve(6);
        for (uint8_t index = 0; index < 6; ++index) {
            std::string row = mainLabel(index);
            if (index == _mainIndex) row += " <-";
            items.push_back(row);
        }
        return items;
    }

    items.reserve(submenuSize());
    for (uint8_t index = 0; index < submenuSize(); ++index) {
        std::string row = submenuLabel(index);
        const std::string value = submenuValue(index);
        if (row.empty()) {
            row = value;
        } else if (!value.empty()) {
            row += ": " + value;
        }
        if (index == _submenuIndex && isSubmenuItemSelectable(index)) row += " <-";
        items.push_back(row);
    }
    return items;
}

bool Menu::isSubmenu() const { return _level == Level::SUBMENU; }

bool Menu::isMainOption() const { return _level == Level::WATCHFACE; }

uint8_t Menu::getButtonLayout() const {
    const int layout = _nvs->getInt("button_layout", BUTTON_MAP);
    return layout >= 0 && layout <= 3 ? static_cast<uint8_t>(layout) : BUTTON_MAP;
}

uint8_t Menu::getButtonPressed(const uint64_t &wakeupBit) {
    if (wakeupBit & RIGHT_BTN_MASK) return RIGHT_BTN_PIN;
    if (wakeupBit & LEFT_BTN_MASK) return LEFT_BTN_PIN;
    if (wakeupBit & SELECT_BTN_MASK) return SELECT_BTN_PIN;
    if (wakeupBit & BACK_BTN_MASK) return BACK_BTN_PIN;
    return 0;
}

void Menu::next() {
    if (_level == Level::WATCHFACE) return;
    if (_level == Level::MAIN_MENU) {
        _mainIndex = (_mainIndex + 1) % 6;
    } else if (_mainIndex == 1 && _submenuIndex == 1 && _alarmEdit != AlarmEdit::NONE) {
        cycleAlarmValue(1);
    } else {
        moveSubmenuSelection(1);
    }
}

void Menu::previous() {
    if (_level == Level::WATCHFACE) return;
    if (_level == Level::MAIN_MENU) {
        _mainIndex = _mainIndex == 0 ? 5 : _mainIndex - 1;
    } else if (_mainIndex == 1 && _submenuIndex == 1 && _alarmEdit != AlarmEdit::NONE) {
        cycleAlarmValue(-1);
    } else {
        moveSubmenuSelection(-1);
    }
}

void Menu::select() {
    if (_level == Level::WATCHFACE) {
        _level = Level::MAIN_MENU;
        return;
    }
    if (_level == Level::MAIN_MENU) {
        _level = Level::SUBMENU;
        _submenuIndex = 0;
        _alarmEdit = AlarmEdit::NONE;
        if (_mainIndex == 2) _buttonLayoutSelection = getButtonLayout();
        if (!isSubmenuItemSelectable(_submenuIndex)) moveSubmenuSelection(1);
        return;
    }

    if (!isSubmenuItemSelectable(_submenuIndex)) return;

    if (_mainIndex == 0 && _submenuIndex == 0) {
        _ntpStatus = OperationStatus::STARTED;
        _pendingAction = PendingAction::NTP_SYNC;
    } else if (_mainIndex == 0 && _submenuIndex == 2) {
        _rtcStatus = OperationStatus::STARTED;
        _pendingAction = PendingAction::RTC_CALIBRATION;
    } else if (_mainIndex == 0 && _submenuIndex == 3) {
        const bool enabled = _nvs->getInt("auto_dst", AUTO_DST_DEFAULT);
        _nvs->setInt("auto_dst", !enabled);
        _timeZoneChanged = true;
    } else if (_mainIndex == 1 && _submenuIndex == 0) {
        toggleAlarm();
    } else if (_mainIndex == 1 && _submenuIndex == 1) {
        _alarmEdit = _alarmEdit == AlarmEdit::NONE ? AlarmEdit::HOURS :
                     _alarmEdit == AlarmEdit::HOURS ? AlarmEdit::MINUTES : AlarmEdit::HOURS;
    } else if (_mainIndex == 2) {
        if (_submenuIndex == 0) {
            const size_t count = _screen->getFaces().size();
            const int current = _nvs->getInt("watchface", 0);
            if (count) _nvs->setInt("watchface", (current + 1) % count);
        } else {
            _buttonLayoutSelection = (_buttonLayoutSelection + 1) % 4;
        }
    }
}

void Menu::back() {
    _alarmEdit = AlarmEdit::NONE;
    if (_level == Level::SUBMENU) {
        if (_mainIndex == 2) _nvs->setInt("button_layout", _buttonLayoutSelection);
        _level = Level::MAIN_MENU;
    } else if (_level == Level::MAIN_MENU) {
        _level = Level::WATCHFACE;
    }
}

void Menu::cycleAlarmValue(int direction) {
    uint8_t hour, minute;
    loadAlarm(hour, minute);
    if (_alarmEdit == AlarmEdit::HOURS) {
        hour = static_cast<uint8_t>((hour + (direction > 0 ? 1 : 23)) % 24);
    } else {
        minute = static_cast<uint8_t>((minute + (direction > 0 ? 10 : 50)) % 60);
    }
    saveAlarm(hour, minute);
}

void Menu::toggleAlarm() {
    uint8_t hour, minute;
    loadAlarm(hour, minute);
    const bool enabled = _nvs->getInt("alarm_enabled", !_nvs->getBlob("alarm").empty());
    if (_nvs->getBlob("alarm").empty()) saveAlarm(hour, minute);
    _nvs->setInt("alarm_enabled", !enabled);
}

void Menu::loadAlarm(uint8_t &hour, uint8_t &minute) const {
    const std::vector<uint8_t> value = _nvs->getBlob("alarm");
    hour = value.size() >= 2 && value[0] < 24 ? value[0] : 7;
    minute = value.size() >= 2 && value[1] < 60 ? value[1] - value[1] % 10 : 0;
}

void Menu::saveAlarm(uint8_t hour, uint8_t minute) {
    uint8_t value[2] = {hour, minute};
    _nvs->setBlob("alarm", value, sizeof(value));
}

uint8_t Menu::submenuSize() const {
    static constexpr uint8_t sizes[] = {4, 2, 2, 3, 4, 3};
    return sizes[_mainIndex];
}

bool Menu::isSubmenuItemSelectable(uint8_t index) const {
    if (_mainIndex == 0) return index == 0 || index == 2 || index == 3;
    if (_mainIndex == 1) return true;
    if (_mainIndex == 2) return true;
    return false;
}

void Menu::moveSubmenuSelection(int direction) {
    const uint8_t size = submenuSize();
    uint8_t candidate = _submenuIndex;
    for (uint8_t checked = 0; checked < size; ++checked) {
        candidate = direction > 0 ? static_cast<uint8_t>((candidate + 1) % size) :
                    static_cast<uint8_t>((candidate + size - 1) % size);
        if (isSubmenuItemSelectable(candidate)) {
            _submenuIndex = candidate;
            return;
        }
    }
}

std::string Menu::mainTitle() const {
    return mainLabel(_mainIndex);
}

std::string Menu::mainLabel(uint8_t index) {
    static const char *titles[] = {"TIME", "ALARM", "WATCHFACE", "WIFI", "PROPERTIES", "FIRMWARE"};
    return titles[index];
}

std::string Menu::submenuLabel(uint8_t index) const {
    static const char *labels[][4] = {
        {"NTP sync", "schedule", "RTC calib.", "Auto DST"},
        {"Status", "Time", "", ""},
        {"Face", "BTN Layout", "", ""},
        {"SSID", "PASSWORD", "HOSTNAME", ""},
        {"MAC", "IP", "Battery", "Orientation"},
        {"", "", "Version", ""},
    };
    return labels[_mainIndex][index];
}

std::string Menu::submenuValue(uint8_t index) {
    if (_mainIndex == 0) {
        if (index == 0) return statusText(_ntpStatus);
        if (index == 1) return ntpSchedule();
        if (index == 2) return statusText(_rtcStatus);
        return _nvs->getInt("auto_dst", AUTO_DST_DEFAULT) ? "ON" : "OFF";
    }
    if (_mainIndex == 1) {
        if (index == 0) {
            const bool enabled = _nvs->getInt("alarm_enabled", !_nvs->getBlob("alarm").empty());
            return enabled ? "ON" : "OFF";
        }
        return alarmTime();
    }
    if (_mainIndex == 2) {
        if (index == 1) return std::to_string(_buttonLayoutSelection);
        int face = _nvs->getInt("watchface", 0);
        if (face < 0 || static_cast<size_t>(face) >= _screen->getFaces().size()) face = 0;
        return _screen->getFaces().at(face)->getName();
    }
    if (_mainIndex == 3) {
        if (index == 0) return WIFI_SSID;
        if (index == 1) return WIFI_PASS;
        return WIFI_HOSTNAME;
    }
    if (_mainIndex == 4) {
        if (index == 0) return macAddress();
        if (index == 1) return std::string(_nvs->getString("last_ip", "0.0.0.0").c_str());
        if (index == 2) {
            const float voltage = analogReadMilliVolts(BATT_ADC_PIN) / 500.0f;
            char text[16];
            snprintf(text, sizeof(text), "%.2f V", voltage);
            return text;
        }
        return orientation();
    }
    if (index == 0) return "ESP32 rev " + std::to_string(ESP.getChipRevision());
    if (index == 1) {
        char version[20];
        snprintf(version, sizeof(version), "Arduino %d.%d.%d", ESP_ARDUINO_VERSION_MAJOR,
                 ESP_ARDUINO_VERSION_MINOR, ESP_ARDUINO_VERSION_PATCH);
        return version;
    }
    return RELEASE;
}

std::string Menu::ntpSchedule() const {
#if NTP_SYNC_ENABLED
    char text[32];
    snprintf(text, sizeof(text), "%02d:00 d%d,%d", NTP_SYNC_HOUR, NTP_SYNC_DAY1, NTP_SYNC_DAY2);
    return text;
#else
    return "OFF";
#endif
}

std::string Menu::alarmTime() const {
    uint8_t hour, minute;
    loadAlarm(hour, minute);
    char text[12];
    const char left = _alarmEdit == AlarmEdit::HOURS ? '[' : ' ';
    const char middleLeft = _alarmEdit == AlarmEdit::HOURS ? ']' : ' ';
    const char middleRight = _alarmEdit == AlarmEdit::MINUTES ? '[' : ' ';
    const char right = _alarmEdit == AlarmEdit::MINUTES ? ']' : ' ';
    snprintf(text, sizeof(text), "%c%02d%c:%c%02d%c", left, hour, middleLeft, middleRight, minute, right);
    return text;
}

std::string Menu::orientation() const {
    Accel data;
    if (!_accelerometer->getAccel(data)) return "sensor error";
    switch (AccelParser::normalizeOrientation(data)) {
        case AccelParser::FACE_UP: return "face up";
        case AccelParser::FACE_DOWN: return "face down";
        case AccelParser::LEFT_EDGE: return "left edge";
        case AccelParser::RIGHT_EDGE: return "right edge";
        case AccelParser::TOP_EDGE: return "top edge";
        case AccelParser::BOTTOM_EDGE: return "bottom edge";
    }
    return "unknown";
}

std::string Menu::macAddress() const {
    const uint64_t chipId = ESP.getEfuseMac();
    char text[18];
    snprintf(text, sizeof(text), "%02X:%02X:%02X:%02X:%02X:%02X",
             static_cast<unsigned>((chipId >> 40) & 0xff), static_cast<unsigned>((chipId >> 32) & 0xff),
             static_cast<unsigned>((chipId >> 24) & 0xff), static_cast<unsigned>((chipId >> 16) & 0xff),
             static_cast<unsigned>((chipId >> 8) & 0xff), static_cast<unsigned>(chipId & 0xff));
    return text;
}

std::string Menu::statusText(OperationStatus status) {
    switch (status) {
        case OperationStatus::STARTED: return "started";
        case OperationStatus::WIFI_ERROR: return "wifi error";
        case OperationStatus::SYNC_ERROR: return "sync error";
        case OperationStatus::COMPLETED: return "completed";
        case OperationStatus::IDLE: return "start";
    }
    return {};
}
