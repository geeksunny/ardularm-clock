#include <Arduino.h>
#include <DebugLog.hpp>
#include <prog_str.h>
#include <LED.h>
#include <WifiTools.h>
#include <Timezone.h>
#include <MD_MAX72xx.h>
#include <MD_Parola.h>
#include <SPI.h>
#include "Clock.h"
#include "Configuration.h"
#include "matrix/font.h"
#include "matrix/FontSprite.h"

#define BLINK_DURATION_QUICK      250
#define BLINK_DURATION_STANDARD   500
#define BLINK_DURATION_LONG       1000

#define ANIMATE_SPEED_QUICK       50
#define ANIMATE_SPEED_NORMAL      100
#define ANIMATE_SPEED_SLOW        200

// Configuration
config::Configuration cfg;
// LED vars
led::LED *wifi_led;
// Clock vars
clock_ns::Clock *clock_;
//MAX7219
MD_Parola Display = MD_Parola(MD_HARDWARE_TYPE, PIN_MD_CS, MD_MAX_DEVICES);

sprite::FontSprite animation = sprite::FontSprite(&Display, 1, 4);

void on_wait_wifi_cb() {
  DEBUG("Wifi not yet available...")
  wifi_led->toggle();
  led::LED::loop();
  animation.displayAnimate();
}

void setup() {
  SETUP_SERIAL(BAUD_RATE, 3000, "Serial console ready.")
  // MAX7219
  Display.begin(2);
  Display.setZone(0, 0, 2);
  Display.setZone(1, 3, 3);
  Display.setFont(glyphs);
  Display.displayClear();
  // Config
  cfg.load();
  DEBUG("Configuration file loaded from SD card.")
  Display.setIntensity(cfg.GetClockConfig().GetBrightness());
  // Clock setup
  clock_ = new clock_ns::Clock(cfg.GetClockConfig().GetTz());
  // setup LED
  wifi_led = new led::LED(PIN_LED_WIFI);
  wifi_led->setup();
  // Connect to wifi with a custom wait status callback. Display wifi animation.
  Display.displayZoneText(0, "Wifi?", PA_CENTER, ANIMATE_SPEED_QUICK, 0, PA_PRINT);
  animation.display(1, PA_LEFT, ANIMATE_SPEED_QUICK);
  animation.displayAnimate();
  wifi_tools::startClient(on_wait_wifi_cb, BLINK_DURATION_QUICK);
  wifi_led->on();
  Display.displayReset();
  // Sync time from NTP.
  if (!clock_->isNTPSynced()) {
    char icon = char(19);
    Display.displayZoneText(1, &icon, PA_CENTER, 0, 0, PA_PRINT);
    Display.displayZoneText(0, "Time?", PA_CENTER, 0, 0, PA_PRINT);
    Display.displayAnimate();
    clock_->syncToNTP();
  }
  // Clear display, remove zone 1
  Display.displayZoneText(1, nullptr, PA_LEFT, 0, 0, PA_PRINT);
  Display.setZone(0, 0, MD_MAX_DEVICES - 1);
  Display.displayClear();
  Display.setTextAlignment(PA_LEFT);
}

void loop() {
  // wifi_led->toggle();
  // led::LED::loop();
  String timeStr = clock_->getTime(':',
                                   !cfg.GetClockConfig().UseMilitaryTime(),
                                   cfg.GetClockConfig().ShowLeadingZero(),
                                   cfg.GetClockConfig().ShowSeconds());
//  String binaryTimeStr = clock_ns::getTimeBinary(time);
  Display.print(timeStr);
  Serial.print("Time: ");
  Serial.print(timeStr);
  Serial.println();
  delay(1000);
}
