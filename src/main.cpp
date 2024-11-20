#include <Arduino.h>
#include <DebugLog.hpp>
#include <prog_str.h>
#include <DS3232RTC.h>
#include <LED.h>
#include <WifiTools.h>

#ifndef LED_WIFI
#define LED_WIFI D3
#endif

#define BLINK_DURATION_QUICK      250

namespace strings {
    // Days of the week, abbreviated
    const char day_sun[] PROGMEM = "Sun";
    const char day_mon[] PROGMEM = "Mon";
    const char day_tue[] PROGMEM = "Tue";
    const char day_wed[] PROGMEM = "Wed";
    const char day_thu[] PROGMEM = "Thu";
    const char day_fri[] PROGMEM = "Fri";
    const char day_sat[] PROGMEM = "Sat";
    // Months, abbreviated
    const char mon_jan[] PROGMEM = "Jan";
    const char mon_feb[] PROGMEM = "Feb";
    const char mon_mar[] PROGMEM = "Mar";
    const char mon_apr[] PROGMEM = "Apr";
    const char mon_may[] PROGMEM = "May";
    const char mon_jun[] PROGMEM = "Jun";
    const char mon_jul[] PROGMEM = "Jul";
    const char mon_aug[] PROGMEM = "Aug";
    const char mon_sept[] PROGMEM = "Sept";
    const char mon_oct[] PROGMEM = "Oct";
    const char mon_nov[] PROGMEM = "Nov";
    const char mon_dec[] PROGMEM = "Dec";
    // Misc
    const char unknown[] PROGMEM = "UNKNOWN";
}

DS3232RTC rtc;
String dayOfTheWeek, monthOfTheYear;
led::LED *wifi_led;

void on_wait_wifi_cb() {
    DEBUG("Wifi not yet available...")
    led::LED::loop();
}

String getDayOfTheWeek(int day) {
    switch (day) {
        case 1:
            return read_prog_str(strings::day_sun);
        case 2:
            return read_prog_str(strings::day_mon);
        case 3:
            return read_prog_str(strings::day_tue);
        case 4:
            return read_prog_str(strings::day_wed);
        case 5:
            return read_prog_str(strings::day_thu);
        case 6:
            return read_prog_str(strings::day_fri);
        case 7:
            return read_prog_str(strings::day_sat);
        default:
            return read_prog_str(strings::unknown);
    }
}

String getMonthOfTheYear(int month) {
    switch (month) {
        case 1:
            return read_prog_str(strings::mon_jan);
        case 2:
            return read_prog_str(strings::mon_feb);
        case 3:
            return read_prog_str(strings::mon_mar);
        case 4:
            return read_prog_str(strings::mon_apr);
        case 5:
            return read_prog_str(strings::mon_may);
        case 6:
            return read_prog_str(strings::mon_jun);
        case 7:
            return read_prog_str(strings::mon_jul);
        case 8:
            return read_prog_str(strings::mon_aug);
        case 9:
            return read_prog_str(strings::mon_sept);
        case 10:
            return read_prog_str(strings::mon_oct);
        case 11:
            return read_prog_str(strings::mon_nov);
        case 12:
            return read_prog_str(strings::mon_dec);
        default:
            return read_prog_str(strings::unknown);
    }
}

void setup() {
    SETUP_SERIAL(BAUD_RATE, 3000, "Serial console ready.")
    // RTC init
    rtc.begin();
    setTime(18, 43, 30, 18, 11, 2024); // Temporary test value
    setSyncProvider(rtc.get);
    if (timeStatus() != timeSet) {
        DEBUG("Unable to sync with the RTC")
    } else {
        DEBUG("RTC has set the system time.")
    }
    dayOfTheWeek = getDayOfTheWeek(dayOfWeek(now()));
    monthOfTheYear = getMonthOfTheYear(month());
    // setup LED
    wifi_led = new led::LED(LED_WIFI);
    wifi_led->setup();
    // Connect to wifi with a custom wait status callback
    wifi_tools::startClient(on_wait_wifi_cb, BLINK_DURATION_QUICK);
    wifi_led->on();
}

void printDigits(int digits) {
    // utility function for digital clock display: prints preceding colon and leading 0
    Serial.print(':');
    if(digits < 10)
        Serial.print('0');
    Serial.print(digits);
}

void digitalClockDisplay() {
    // digital clock display of the time
    Serial.print(hour());
    printDigits(minute());
    printDigits(second());
    Serial.print(' ');
    Serial.print(dayOfTheWeek);
    Serial.print('(');
    Serial.print(day());
    Serial.print(") ");
    Serial.print(monthOfTheYear);
    Serial.print('(');
    Serial.print(month());
    Serial.print(") ");
    Serial.print(year());
    Serial.println();
}

void loop() {
    wifi_led->toggle();
    led::LED::loop();
    digitalClockDisplay();
    delay(1000);
}
