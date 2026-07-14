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

#include "Screen.h"
#include "Faces/HelloFace.h"
#include "Faces/JetBrainsFace.h"
#include "Faces/DefaultFace.h"
#include "Faces/UwUFace.h"
#include "defines.h"
#include "resources.h"
#if PRIVATE == 1
#include "Faces/Private/Include.h"
#endif

Screen::Screen(GxEPD2_BW<WatchyDisplay, WatchyDisplay::HEIGHT> *display, const ScreenInfo &screenInfo, ArduinoNvs *nvs) :
        _display(display), _screenInfo(screenInfo), _nvs(nvs) {
    _faces.emplace_back(std::make_unique<HelloFace>(display));
    _faces.emplace_back(std::make_unique<JetBrainsFace>(display));
    _faces.emplace_back(std::make_unique<DefaultFace>(display));
    _faces.emplace_back(std::make_unique<UwUFace>(display));
#if PRIVATE == 1
    Private::includeFaces(&_faces, display);
#endif
}

void Screen::update(bool partial) {
    _display->setFullWindow();
    _display->fillScreen(GxEPD_WHITE);

    if (_screenInfo.onMainOption) {
        int watchface = _nvs->getInt("watchface", 0);
        if (watchface < 0 || static_cast<size_t>(watchface) >= _faces.size()) {
            _nvs->setInt("watchface", 0);
            watchface = 0;
        }
        _faces.at(watchface)->draw(_screenInfo);
    } else {
        drawMenu();
    }

    // The white background already replaces the previous frame in the buffer;
    // keep partial updates enabled so menu navigation remains responsive.
    _display->display(partial);
}

void Screen::drawMenu() {
    if (!_screenInfo.menuItems.empty()) {
        drawMenuList();
        return;
    }

    constexpr int16_t titleCenterY = 82;
    constexpr int16_t descriptionCenterY = 118;

    _display->setTextColor(GxEPD_BLACK);
    drawCenteredText(_screenInfo.title, &resources::JETBRAINS_MONO_REGULAR_15, titleCenterY);
    drawCenteredText(_screenInfo.description, &resources::JETBRAINS_MONO_THIN_10, descriptionCenterY);
}

void Screen::drawMenuList() {
    constexpr int16_t titleCenterY = 28;
    const bool mainMenu = !_screenInfo.onSubmenu;
    const int16_t firstItemCenterY = mainMenu ? 58 : 68;
    const int16_t itemSpacing = mainMenu ? 23 : 30;

    _display->setTextColor(GxEPD_BLACK);
    drawCenteredText(_screenInfo.title, &resources::JETBRAINS_MONO_REGULAR_15, titleCenterY);

    const GFXfont *itemFont = mainMenu ? &resources::JETBRAINS_MONO_THIN_10 :
                                        &resources::EIGHT_BIT_OPERATOR_PLUS_BOLD_6;
    if (!mainMenu) {
        for (const std::string &item : _screenInfo.menuItems) {
            if (!textFits(item, itemFont, 196)) {
                itemFont = nullptr;
                break;
            }
        }
    }

    for (size_t index = 0; index < _screenInfo.menuItems.size(); ++index) {
        const int16_t centerY = static_cast<int16_t>(firstItemCenterY + index * itemSpacing);
        drawCenteredText(_screenInfo.menuItems[index], itemFont, centerY);
    }

    if (mainMenu) drawMainMenuButtonHints();
}

void Screen::drawMainMenuButtonHints() {
    if (_screenInfo.buttonLayout == 0) {
        drawButtonHint("BACK", false, 9);
        drawButtonHint("OK", true, 9);
        drawButtonHint("LEFT", false, 190);
        drawButtonHint("RIGHT", true, 190);
    } else if (_screenInfo.buttonLayout == 1) {
        drawButtonHint("LEFT", false, 9);
        drawButtonHint("RIGHT", true, 9);
        drawButtonHint("BACK", false, 190);
        drawButtonHint("OK", true, 190);
    } else if (_screenInfo.buttonLayout == 2) {
        drawButtonHint("BACK", false, 9);
        drawButtonHint("UP", true, 9);
        drawButtonHint("OK", false, 190);
        drawButtonHint("DOWN", true, 190);
    } else {
        drawButtonHint("OK", false, 9);
        drawButtonHint("UP", true, 9);
        drawButtonHint("BACK", false, 190);
        drawButtonHint("DOWN", true, 190);
    }
}

void Screen::drawButtonHint(const char *text, bool rightAligned, int16_t centerY) {
    int16_t x;
    int16_t y;
    uint16_t width;
    uint16_t height;
    const String printable(text);

    _display->setTextWrap(false);
    _display->setFont(&resources::EIGHT_BIT_OPERATOR_PLUS_BOLD_6);
    _display->getTextBounds(printable, 0, 0, &x, &y, &width, &height);

    const int16_t cursorX = rightAligned ?
        static_cast<int16_t>(_display->width() - 2 - width - x) : static_cast<int16_t>(2 - x);
    const int16_t cursorY = static_cast<int16_t>(centerY - static_cast<int16_t>(height) / 2 - y);
    _display->setCursor(cursorX, cursorY);
    _display->print(printable);
    _display->setTextWrap(true);
}

void Screen::drawCenteredText(const std::string &text, const GFXfont *font, int16_t centerY) {
    int16_t x;
    int16_t y;
    uint16_t width;
    uint16_t height;
    String printable(text.c_str());

    _display->setTextWrap(false);
    _display->setFont(font);
    _display->getTextBounds(printable, 0, 0, &x, &y, &width, &height);

    const int16_t cursorX = static_cast<int16_t>(
        (static_cast<int16_t>(_display->width()) - static_cast<int16_t>(width)) / 2 - x
    );
    const int16_t cursorY = static_cast<int16_t>(
        centerY - static_cast<int16_t>(height) / 2 - y
    );

    _display->setCursor(cursorX, cursorY);
    _display->print(printable);
    _display->setTextWrap(true);
}

bool Screen::textFits(const std::string &text, const GFXfont *font, uint16_t maximumWidth) {
    int16_t x;
    int16_t y;
    uint16_t width;
    uint16_t height;
    _display->setTextWrap(false);
    _display->setFont(font);
    _display->getTextBounds(String(text.c_str()), 0, 0, &x, &y, &width, &height);
    _display->setTextWrap(true);
    return width <= maximumWidth;
}

const std::vector<std::unique_ptr<AbstractFace>> &Screen::getFaces() const {
    return _faces;
}
