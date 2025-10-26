#ifndef TINYWATCHY_JETBRAINSFACE_H
#define TINYWATCHY_JETBRAINSFACE_H

#include "AbstractFace.h"

class JetBrainsFace : public AbstractFace {
public:
    explicit JetBrainsFace(GxEPD2_BW<WatchyDisplay, WatchyDisplay::HEIGHT> *display);

    void draw(const ScreenInfo &screenInfo) override;

    std::string getName() override { return "JetBrains"; };

private:
    void drawTime(const DateTime &time);
    void drawDate(const DateTime &time);
    void drawBattery(const uint8_t &battery);
    void drawMenuTitle(const std::string &title);
    void drawMenuDescription(const std::string &description);
};

#endif //TINYWATCHY_JETBRAINSFACE_H