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
#include "matrix/Display.h"

#define BLINK_DURATION_QUICK      250
#define BLINK_DURATION_STANDARD   500
#define BLINK_DURATION_LONG       1000

// Configuration
config::Configuration cfg;
// LED vars
led::LED *wifi_led;
// Clock vars
clock_ns::Clock *clock_;
//MAX7219
display::Display display_ = display::Display();

void on_wait_wifi_cb() {
  DEBUG("Wifi not yet available...")
  wifi_led->toggle();
  led::LED::loop();
  display_.processAnimation();
}

void setup() {
  SETUP_SERIAL(BAUD_RATE, 3000, "Serial console ready.")
  // MAX7219
  display_.resetZones(false);
  // Config
  cfg.load();
  DEBUG("Configuration file loaded from SD card.")
  display_.setConfig(cfg.GetClockConfig());
  // Clock setup
  clock_ = new clock_ns::Clock(cfg.GetClockConfig().GetTz());
  // setup LED
  wifi_led = new led::LED(PIN_LED_WIFI);
  wifi_led->setup();
  // Connect to wifi with a custom wait status callback. Display wifi animation.
  display::displayWifi(display_);
  wifi_tools::startClient(cfg.GetClockConfig().GetWifi().GetSsid().c_str(),
                          cfg.GetClockConfig().GetWifi().GetPassword().c_str(),
                          on_wait_wifi_cb,
                          BLINK_DURATION_QUICK);
  wifi_led->on();
  display_.reset();
  // Sync time from NTP.
  if (!clock_->isNTPSynced()) {
    display::displayTime(display_);
    clock_->syncToNTP();
  }
  // Clear display, remove zone 1
  display_.resetZones(true);
}

void loop() {
  // wifi_led->toggle();
  // led::LED::loop();
  String timeStr = clock_->getTime(':',
                                   !cfg.GetClockConfig().UseMilitaryTime(),
                                   cfg.GetClockConfig().ShowLeadingZero(),
                                   cfg.GetClockConfig().ShowSeconds());
//  String binaryTimeStr = clock_ns::getTimeBinary(time);
  display_.displayText(timeStr.c_str());
  Serial.print("Time: ");
  Serial.print(timeStr);
  Serial.println();
  delay(1000);
}
