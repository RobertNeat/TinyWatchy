#include "JetBrainsFace.h"
#include "resources.h"

JetBrainsFace::JetBrainsFace(GxEPD2_BW<WatchyDisplay, WatchyDisplay::HEIGHT> *display) : AbstractFace(display) {

}

void JetBrainsFace::draw(const ScreenInfo &screenInfo) {
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

void JetBrainsFace::drawTime(const DateTime &time) {
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
    
    // Use JetBrains Mono Bold 50 for time
    _display->setFont(&resources::JETBRAINS_MONO_BOLD_50);
    _display->setTextColor(GxEPD_BLACK);
    
    // Get text bounds to center it
    _display->getTextBounds(timeStr, 0, 0, &x, &y, &w, &h);
    
    // Center horizontally, position at top third of screen
    int centerX = (200 - w) / 2;
    int timeY = 60;
    
    _display->setCursor(centerX, timeY);
    _display->print(timeStr);
}

void JetBrainsFace::drawDate(const DateTime &time) {
    int16_t x, y;
    uint16_t w, h;
    
    // Format date as DD:MM:YYYY
    String dateStr = "";
    if (time.day < 10) {
        dateStr += "0";
    }
    dateStr += String(time.day);
    dateStr += ":";
    if (time.month < 10) {
        dateStr += "0";
    }
    dateStr += String(time.month);
    dateStr += ":";
    dateStr += String(time.year);
    
    // Use JetBrains Mono Regular 15 for date
    _display->setFont(&resources::JETBRAINS_MONO_REGULAR_15);
    _display->setTextColor(GxEPD_BLACK);
    
    // Get text bounds to center it
    _display->getTextBounds(dateStr, 0, 0, &x, &y, &w, &h);
    
    // Center horizontally, position below time
    int centerX = (200 - w) / 2;
    int dateY = 90;
    
    _display->setCursor(centerX, dateY);
    _display->print(dateStr);
}

void JetBrainsFace::drawBattery(const uint8_t &battery) {
    int16_t x, y;
    uint16_t w, h;
    
    // Format battery as XX%
    String batteryStr = String(battery) + "%";
    
    // Use JetBrains Mono Thin 12 for battery
    _display->setFont(&resources::JETBRAINS_MONO_THIN_12);
    _display->setTextColor(GxEPD_BLACK);
    
    // Get text bounds to center it
    _display->getTextBounds(batteryStr, 0, 0, &x, &y, &w, &h);
    
    // Center horizontally, position below date
    int centerX = (200 - w) / 2;
    int batteryY = 120;
    
    _display->setCursor(centerX, batteryY);
    _display->print(batteryStr);
}

void JetBrainsFace::drawMenuTitle(const std::string &title) {
    int16_t x, y;
    uint16_t w, h;
    String text(title.c_str());
    
    _display->setFont(&resources::JETBRAINS_MONO_THIN_12);
    _display->setTextColor(GxEPD_BLACK);
    _display->getTextBounds(text, 0, 0, &x, &y, &w, &h);

    _display->setCursor(static_cast<int16_t>((200-w)/2), 165);
    _display->print(title.c_str());
}

void JetBrainsFace::drawMenuDescription(const std::string &description) {
    int16_t x, y;
    uint16_t w, h;
    String text(description.c_str());
    
    _display->setFont(&resources::JETBRAINS_MONO_THIN_10);
    _display->setTextColor(GxEPD_BLACK);
    _display->getTextBounds(text, 0, 0, &x, &y, &w, &h);

    _display->setCursor(static_cast<int16_t>((200-w)/2), 185);
    _display->print(description.c_str());
}