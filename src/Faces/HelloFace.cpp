#include "HelloFace.h"
#include "resources.h"
#include <array>

const std::array<const char*, 12> monthNames = {
    "january", "february", "march", "april", "may", "june",
    "july", "august", "september", "october", "november", "december"
};

HelloFace::HelloFace(GxEPD2_BW<WatchyDisplay, WatchyDisplay::HEIGHT> *display) : AbstractFace(display) {

}

void HelloFace::draw(const ScreenInfo &screenInfo) {
    // Clear the display with white background
    _display->fillScreen(GxEPD_WHITE);
    
    drawTime(screenInfo.time);
    drawDate(screenInfo.time);
    drawBattery(screenInfo.battery);
    
    // Draw menu elements if available
    if (!screenInfo.onMainOption) {
        drawMenuTitle(screenInfo.title);
        drawMenuDescription(screenInfo.description);
    }
}

void HelloFace::drawTime(const DateTime &time) {
    int16_t x, y;
    uint16_t w, h;
    
    // Format time as HH:MM
    String timeStr = "";
    if (time.hour < 10) {
        timeStr += "0";
    }
    timeStr += String(time.hour);
    timeStr += ":";
    if (time.minute < 10) {
        timeStr += "0";
    }
    timeStr += String(time.minute);
    
    // Use MomoSignature Regular 51 for time
    _display->setFont(&resources::MOMO_SIGNATURE_REGULAR_28);
    _display->setTextColor(GxEPD_BLACK);
    
    // Get text bounds to center it
    _display->getTextBounds(timeStr, 0, 0, &x, &y, &w, &h);
    
    // Center horizontally, position at top third of screen
    int centerX = (200 - w) / 2;
    int timeY = 89;
    
    _display->setCursor(centerX, timeY);
    _display->print(timeStr);
}

void HelloFace::drawDate(const DateTime &time) {
    int16_t x, y;
    uint16_t w, h;
    
    // Format date as "DD month YYYY"
    String dateStr = "";
    dateStr += String(time.day);
    dateStr += " ";
    dateStr += monthNames[time.month];  // months are 0-based, perfect for array index
    dateStr += " ";
    dateStr += String(time.year + 1970);  // Convert from 1970-based offset to actual year

    // Use MomoSignature Regular 18 for date
    _display->setFont(&resources::MOMO_SIGNATURE_REGULAR_8);
    _display->setTextColor(GxEPD_BLACK);
    
    // Get text bounds to center it
    _display->getTextBounds(dateStr, 0, 0, &x, &y, &w, &h);
    
    // Center horizontally, position below time
    int centerX = (200 - w) / 2;
    int dateY = 142;
    
    _display->setCursor(centerX, dateY);
    _display->print(dateStr);
}

void HelloFace::drawBattery(const uint8_t &battery) {
    int16_t x, y;
    uint16_t w, h;
    
    // Format battery as XX%
    String batteryStr = "battery "+String(battery) + "%";

    // Use MomoSignature Regular 18 for battery
    _display->setFont(&resources::MOMO_SIGNATURE_REGULAR_13);
    _display->setTextColor(GxEPD_BLACK);
    
    // Get text bounds to center it
    _display->getTextBounds(batteryStr, 0, 0, &x, &y, &w, &h);
    
    // Center horizontally, position below date
    int centerX = (200 - w) / 2;
    int batteryY = 180;
    
    _display->setCursor(centerX, batteryY);
    _display->print(batteryStr);
}

void HelloFace::drawMenuTitle(const std::string &title) {
    int16_t x, y;
    uint16_t w, h;
    String text(title.c_str());
    
    _display->setFont(&resources::MOMO_SIGNATURE_REGULAR_10);
    _display->setTextColor(GxEPD_BLACK);
    _display->getTextBounds(text, 0, 0, &x, &y, &w, &h);

    _display->setCursor(static_cast<int16_t>((200-w)/2), 165);
    _display->print(title.c_str());
}

void HelloFace::drawMenuDescription(const std::string &description) {
    int16_t x, y;
    uint16_t w, h;
    String text(description.c_str());
    
    _display->setFont(&resources::MOMO_SIGNATURE_REGULAR_10);
    _display->setTextColor(GxEPD_BLACK);
    _display->getTextBounds(text, 0, 0, &x, &y, &w, &h);

    _display->setCursor(static_cast<int16_t>((200-w)/2), 185);
    _display->print(description.c_str());
}