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
    constexpr int16_t titleCenterY = 82;
    constexpr int16_t descriptionCenterY = 118;

    _display->setTextColor(GxEPD_BLACK);
    drawCenteredText(_screenInfo.title, &resources::JETBRAINS_MONO_REGULAR_15, titleCenterY);
    drawCenteredText(_screenInfo.description, &resources::JETBRAINS_MONO_THIN_10, descriptionCenterY);
}

void Screen::drawCenteredText(const std::string &text, const GFXfont *font, int16_t centerY) {
    int16_t x;
    int16_t y;
    uint16_t width;
    uint16_t height;
    String printable(text.c_str());

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
}

const std::vector<std::unique_ptr<AbstractFace>> &Screen::getFaces() const {
    return _faces;
}
