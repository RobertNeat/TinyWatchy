#ifndef TINYWATCHY_HELLOFACE_H
#define TINYWATCHY_HELLOFACE_H

#include "AbstractFace.h"

class HelloFace : public AbstractFace {
public:
    explicit HelloFace(GxEPD2_BW<WatchyDisplay, WatchyDisplay::HEIGHT> *display);

    void draw(const ScreenInfo &screenInfo) override;

    std::string getName() override { return "HelloFace"; };

private:
    void drawTime(const DateTime &time);
    void drawDate(const DateTime &time);
    void drawBattery(const uint8_t &battery);
    void drawMenuTitle(const std::string &title);
    void drawMenuDescription(const std::string &description);
};

#endif //TINYWATCHY_HELLOFACE_H