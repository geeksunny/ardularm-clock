#include "matrix/Display.h"
#include "matrix/font.h"
#include "PgmStringTools.hpp"

#define ANIMATE_SPEED_QUICK       50
#define ANIMATE_SPEED_NORMAL      100
#define ANIMATE_SPEED_SLOW        200

namespace display {

namespace strings {

const char wifi[] PROGMEM = "Wifi?";
const char time[] PROGMEM = "Time?";

}

namespace sprites {

const sprite wifi = {1, 4};
const sprite clock = {19, 0};

}

Display::Display() {
  display_.begin(2);
  display_.setFont(glyphs);
  display_.displayClear();

  display_.setIntensity(1);
}

void Display::setConfig(config::ClockConfig config) {
  display_.setIntensity(config.GetBrightness());
}

void Display::resetZones(bool single_zone) {
  if (single_zone) {
    display_.displayZoneText(1, nullptr, PA_LEFT, 0, 0, PA_PRINT);
    display_.setZone(0, 0, MD_MAX_DEVICES - 1);
  } else {
    display_.setZone(0, 0, 2);
    display_.setZone(1, 3, 3);
  }
  display_.displayClear();
}

void Display::reset() {
  display_.displayReset();
}

void Display::processAnimation() {
  sprite_.displayAnimate();
}

void Display::displayText(const char *text,
                          textPosition_t align,
                          uint16_t speed,
                          uint16_t pause,
                          textEffect_t effectIn,
                          textEffect_t effectOut) {
  display_.displayZoneText(0, text, align, speed, pause, effectIn, effectOut);
  display_.displayAnimate();
}

void Display::setIcon(sprites::sprite sprite) {
  sprite_.setAsciiCodes(sprite.ascii_start, sprite.ascii_end);
}

void Display::displayIcon(textPosition_t align, uint16_t speed, uint8_t z) {
  sprite_.display(z, align, speed);
  sprite_.displayAnimate();
}

void displayTextWithIcon(Display &display_, const char *pgm_str, const sprites::sprite &sprite_) {
  display_.setIcon(sprites::wifi);
  display_.displayText(read_prog_str(pgm_str).c_str(),
                       PA_CENTER,
                       ANIMATE_SPEED_QUICK);
  display_.displayIcon(PA_LEFT, ANIMATE_SPEED_QUICK);
}

void displayWifi(Display &display_) {
  displayTextWithIcon(display_, strings::wifi, sprites::wifi);
}

void displayTime(Display &display_) {
  displayTextWithIcon(display_, strings::time, sprites::clock);
}

}
