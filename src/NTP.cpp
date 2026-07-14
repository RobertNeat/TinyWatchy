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

#include "NTP.h"
#include "WiFiHelper.h"
#include "NTPClient.h"

NTP::NTP(SmallRTC *smallRTC, ArduinoNvs *nvs) : _smallRTC(smallRTC), _nvs(nvs) {}

bool NTP::sync() {
    return syncWithStatus() == SyncResult::COMPLETED;
}

NTP::SyncResult NTP::syncWithStatus() {
    SyncResult result;
    time_t epochTime = getTime(&result);

    if (epochTime == 0) {
        return result;
    }

    tmElements_t time;
    _smallRTC->doBreakTime(epochTime, time);
    _smallRTC->set(time);

    return SyncResult::COMPLETED;
}

time_t NTP::getTime(SyncResult *result) {
    if (!WiFiHelper::connect()) {
        WiFiHelper::disconnect();
        if (result) *result = SyncResult::WIFI_ERROR;
        return 0;
    }

    if (_nvs) _nvs->setString("last_ip", WiFi.localIP().toString());

    WiFiUDP ntpUDP;
    NTPClient timeClient(ntpUDP, NTP_SERVER);

    timeClient.begin();

    const bool updated = timeClient.forceUpdate();

    timeClient.end();

    time_t epochTime = timeClient.getEpochTime();
    
    // Disconnect WiFi immediately after getting time to save power
    WiFiHelper::disconnect();

    if (!updated || epochTime < 946684800) {
        if (result) *result = SyncResult::SYNC_ERROR;
        return 0;
    }

    if (result) *result = SyncResult::COMPLETED;
    return epochTime;
}
