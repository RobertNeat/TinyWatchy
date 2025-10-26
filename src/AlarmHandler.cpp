#include "AlarmHandler.h"
#if PRIVATE == 1
#include "Private/AlarmHandlerPrivate.h"
#endif
#include "defines_private.h"
#include "defines.h"

AlarmHandler::AlarmHandler(SmallRTC *smallRTC, BMA423 *accel, bool *accelStatus, ArduinoNvs *nvs) :
    _smallRTC(smallRTC), _accel(accel), _accelStatus(accelStatus), _nvs(nvs)
{
    _alarms[alarmTimeToIndex(SLEEP_START)] = {SLEEP_START, 0, true};
    _alarms[alarmTimeToIndex(SLEEP_END)] = {SLEEP_END, 0, true};
    
    // Add automatic wakeup alarms every WAKEUP_INTERVAL_MINUTES (e.g., 12:00, 12:05, 12:10, etc.)
    // Only during wake hours (not during sleep time)
    for (uint8_t hour = 0; hour < 24; hour++) {
        // Skip sleep hours
        bool isDuringSleep = false;
        if (SLEEP_START > SLEEP_END) {
            // Sleep crosses midnight (e.g., 23:00 to 05:00)
            isDuringSleep = (hour >= SLEEP_START || hour < SLEEP_END);
        } else {
            // Normal sleep hours (e.g., 01:00 to 09:00)
            isDuringSleep = (hour >= SLEEP_START && hour < SLEEP_END);
        }
        
        if (!isDuringSleep) {
            // Add alarms every WAKEUP_INTERVAL_MINUTES during wake hours
            for (uint8_t minute = 0; minute < 60; minute += WAKEUP_INTERVAL_MINUTES) {
                _alarms[alarmTimeToIndex(hour, minute)] = {hour, minute, true};
            }
        }
    }
}

void AlarmHandler::handle(ScreenInfo const *screenInfo) {
#if PRIVATE == 1
    AlarmHandlerPrivate privateHandler(_smallRTC, _accel);
    privateHandler.handle(screenInfo);
#endif

    loadUserAlarm();

    uint16_t currentIndex = alarmTimeToIndex(screenInfo->time.hour, screenInfo->time.minute);
    try {
        const Alarm &alarm = _alarms.at(currentIndex);

        if (!alarm.system) {
            digitalWrite(VIB_MOTOR_PIN, HIGH);
            gpio_hold_en((gpio_num_t)VIB_MOTOR_PIN);
        }
    } catch (std::out_of_range&) {}

    if (screenInfo->humanInSleep && *_accelStatus) {
        _accel->disableAccel();
        *_accelStatus = false;
    } else if (!screenInfo->humanInSleep && !*_accelStatus) {
        _accel->enableAccel();
        *_accelStatus = true;
    }
}

AlarmHandler::Alarm AlarmHandler::getNextAlarm(const uint16_t &currentIndex) {
    for (std::pair<uint16_t, Alarm> alarm : _alarms) {
        if (alarm.first > currentIndex) {
            return alarm.second;
        }
    }

    return _alarms.begin()->second;
}

uint16_t AlarmHandler::alarmTimeToIndex(uint8_t hour, uint8_t minute) {
    return minute + hour*100;
}

void AlarmHandler::setNextAlarm(const DateTime &screenTime) {
    _smallRTC->clearAlarm();

    loadUserAlarm();

    DateTime utcTime;
    _smallRTC->read((tmElements_t &) utcTime);

    uint16_t currentIndex = alarmTimeToIndex(screenTime.hour, screenTime.minute);
    Alarm nextAlarm = getNextAlarm(currentIndex);

    DateTime timeDiff = screenTime - utcTime;
    DateTime alarmTimeTmp = {
        .minute = nextAlarm.minute,
        .hour = nextAlarm.hour,
    };
    DateTime alarmTime = alarmTimeTmp - timeDiff;

    Serial.printf("Wake up: %d:%d, current index: %d, localhour: %d:%d, utchour: %d:%d, time diff: %d:%d, alarm time: %d:%d\n",
        nextAlarm.hour, nextAlarm.minute,
        currentIndex,
        screenTime.hour, screenTime.minute,
        utcTime.hour, utcTime.minute,
        timeDiff.hour, timeDiff.minute,
        alarmTime.hour, alarmTime.minute
    );

    _smallRTC->atTimeWake(alarmTime.hour, alarmTime.minute, true);
}

void AlarmHandler::loadUserAlarm() {
    if (_userAlarmLoaded) return;

    std::vector<uint8_t> alarmTime = _nvs->getBlob("alarm");
    if (!alarmTime.empty()) {
        _alarms[alarmTimeToIndex(alarmTime[0], alarmTime[1])] = {alarmTime[0], alarmTime[1], false};
        _userAlarmLoaded = true;
    }
}
