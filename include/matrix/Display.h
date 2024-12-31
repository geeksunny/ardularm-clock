#ifndef ESP_ALARM_CLOCK_INCLUDE_MATRIX_DISPLAY_H_
#define ESP_ALARM_CLOCK_INCLUDE_MATRIX_DISPLAY_H_

#include "MD_Parola.h"
#include "Configuration.h"
#include "matrix/FontSprite.h"

namespace display {

namespace sprites {

struct sprite {
  uint8_t ascii_start;
  uint8_t ascii_end;
};

}

class Display {
 public:
  explicit Display();
  void setConfig(config::ClockConfig config);
  void resetZones(bool single_zone);
  void reset();

  void processAnimation();
  void displayText(const char* text, textPosition_t align = PA_LEFT, uint16_t speed = 0, uint16_t pause = 0, textEffect_t effectIn = PA_PRINT, textEffect_t effectOut = PA_NO_EFFECT);
  void setIcon(sprites::sprite sprite);
  void displayIcon(textPosition_t align, uint16_t speed, uint8_t z = 1);
 private:
  MD_Parola display_= MD_Parola(MD_HARDWARE_TYPE, PIN_MD_CS, MD_MAX_DEVICES);
  sprite::FontSprite sprite_ = sprite::FontSprite(&display_, 0, 0);
};

void displayTextWithIcon(Display &display_, const char *pgm_str, const sprites::sprite &sprite_);
void displayWifi(Display &display_);
void displayTime(Display &display_);

} // display

#endif //ESP_ALARM_CLOCK_INCLUDE_MATRIX_DISPLAY_H_
