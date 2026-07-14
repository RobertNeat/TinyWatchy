/*

This file is part of TinyWatchy.
Copyright 2023, Michał Szczepaniak <m.szczepaniak.000@gmail.com>

TinyWatchy is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

TinyWatchy is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with TinyWatchy. If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef TINYWATCHY_DEFINES_H
#define TINYWATCHY_DEFINES_H

#include "defines_private.h"

// Pins
#define RIGHT_BTN_PIN 4
#define LEFT_BTN_PIN 26
#define SELECT_BTN_PIN 35
#define BACK_BTN_PIN 25
#define DISPLAY_CS 5
#define DISPLAY_RES 9
#define DISPLAY_DC 10
#define DISPLAY_BUSY 19
#define ACC_INT_1_PIN 14
#define ACC_INT_2_PIN 12
#define VIB_MOTOR_PIN 13
#define RTC_INT_PIN 27
#define BATT_ADC_PIN 34

// HW info
#define RTC_TYPE 2 //PCF8563

// Pin masks
#define RIGHT_BTN_MASK GPIO_SEL_4
#define LEFT_BTN_MASK GPIO_SEL_26
#define SELECT_BTN_MASK GPIO_SEL_35
#define BACK_BTN_MASK GPIO_SEL_25
#define ACC_INT_MASK GPIO_SEL_14
#define RTC_INT_MASK GPIO_SEL_27

// Validate user configuration from defines_private.h at compile time.
#if SLEEP_START < 0 || SLEEP_START > 23
#error "SLEEP_START must be between 0 and 23"
#endif

#if SLEEP_END < 0 || SLEEP_END > 23
#error "SLEEP_END must be between 0 and 23"
#endif

#if WAKEUP_INTERVAL_MINUTES <= 0 || WAKEUP_INTERVAL_MINUTES > 60 || (60 % WAKEUP_INTERVAL_MINUTES) != 0
#error "WAKEUP_INTERVAL_MINUTES must be a positive divisor of 60"
#endif

#if NTP_SYNC_ENABLED != 0 && NTP_SYNC_ENABLED != 1
#error "NTP_SYNC_ENABLED must be 0 or 1"
#endif

#if NTP_SYNC_HOUR < 0 || NTP_SYNC_HOUR > 23
#error "NTP_SYNC_HOUR must be between 0 and 23"
#endif

#if NTP_SYNC_DAY1 < 1 || NTP_SYNC_DAY1 > 7 || NTP_SYNC_DAY2 < 1 || NTP_SYNC_DAY2 > 7
#error "NTP synchronization days must be between 1 (Sunday) and 7 (Saturday)"
#endif

#if BUTTON_MAP != 0 && BUTTON_MAP != 1
#error "BUTTON_MAP must be 0 or 1"
#endif

#if WIFI_TIMEOUT <= 0
#error "WIFI_TIMEOUT must be greater than 0"
#endif

#endif //TINYWATCHY_DEFINES_H
