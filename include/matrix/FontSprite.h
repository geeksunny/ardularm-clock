#ifndef FONTSPRITE_H
#define FONTSPRITE_H

#include <Arduino.h>
#include <MD_Parola.h>

namespace sprite {

class FontSprite {
    public:
      explicit FontSprite(MD_Parola *display, uint8_t ascii_start, uint8_t ascii_end = 0);
      void setAsciiCodes(uint8_t ascii_start, uint8_t ascii_end);
      void processAnimation();
      bool displayAnimate();
      void display(uint8_t z, textPosition_t align, uint16_t speed = 0, uint16_t pause = 0,
                   textEffect_t effectIn = PA_PRINT, textEffect_t effectOut = PA_NO_EFFECT);

    protected:
      MD_Parola *display_;
      uint8_t asciiStart_;
      uint8_t asciiEnd_;
      uint8_t frameCount_;
      uint8_t currentFrame_;
      uint8_t z_ = 0;
      char currentChar_ = 0;
      uint8_t speed_ = 0;
      time_t nextTick_ = 0;
};

} // sprite

#endif //FONTSPRITE_H
