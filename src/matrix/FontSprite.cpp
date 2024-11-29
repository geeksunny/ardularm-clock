#include "matrix/FontSprite.h"
#include "DebugLog.hpp"
#include <TimeLib.h>

namespace sprite {
FontSprite::FontSprite(MD_Parola *display, const uint8_t ascii_start, const uint8_t ascii_end) {
  display_ = display;
  setAsciiCodes(ascii_start, ascii_end);
}

void FontSprite::setAsciiCodes(uint8_t ascii_start, uint8_t ascii_end) {
  asciiStart_ = ascii_start;
  asciiEnd_ = ascii_end;
  frameCount_ = (ascii_end > 0) ? ascii_end - ascii_start : 1;
  currentFrame_ = 0;
}

void FontSprite::processAnimation() {
  if (frameCount_ == 1) {
    // Only one frame, no animation to process.
    return;
  }
  if (millis() > nextTick_) {
    DEBUG("processAnimation advancing... char: ", currentChar_, " | ascii: ", asciiStart_ + currentFrame_);
    currentFrame_++;
    if (currentFrame_ > frameCount_) {
      currentFrame_ = 0;
    }
    currentChar_ = char(currentFrame_ + asciiStart_);
    display_->setTextBuffer(z_, &currentChar_);
    nextTick_ += speed_;
    display_->displayReset(z_);
  }
}

bool FontSprite::displayAnimate() {
  processAnimation();
  return display_->displayAnimate();
}

void FontSprite::display(uint8_t z, textPosition_t align, uint16_t speed, uint16_t pause,
                         textEffect_t effectIn, textEffect_t effectOut) {
  z_ = z;
  speed_ = speed;
  currentFrame_ = 0;
  currentChar_ = char(asciiStart_);
  display_->displayZoneText(z, &currentChar_, align, speed, pause, effectIn, effectOut);
  nextTick_ = millis() + speed;
}

} // sprite
