#include <Arduino.h>
#include <DebugLog.hpp>
#include <LED.h>
#include <WifiTools.h>

#ifndef LED_WIFI
#define LED_WIFI D3
#endif

#define BLINK_DURATION_QUICK      250

led::LED *wifi_led;

void on_wait_wifi_cb() {
    DEBUG("Wifi not yet available...")
    led::LED::loop();
}

void setup() {
    SETUP_SERIAL(BAUD_RATE, 3000, "Serial console ready.")
    // setup LED
    wifi_led = new led::LED(LED_WIFI);
    wifi_led->setup();
    // Connect to wifi with a custom wait status callback
    wifi_tools::startClient(on_wait_wifi_cb, BLINK_DURATION_QUICK);
    wifi_led->on();
}

void loop() {
    wifi_led->toggle();
    led::LED::loop();
    delay(1000);
}