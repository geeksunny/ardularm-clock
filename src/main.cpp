#include <Arduino.h>
#include <DebugLog.hpp>
#include <prog_str.h>
#include <DS3232RTC.h>
#include <LED.h>
#include <WifiTools.h>
#include <NTPClient.h>
#include <Timezone.h>
#include <WiFiUdp.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <font.h>
#include <SPI.h>
#include <sstream>
#include <iomanip>

// MAX7291
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CS_PIN 15

#ifndef LED_WIFI
#define LED_WIFI D3
#endif

#define BLINK_DURATION_QUICK      250
#define BLINK_DURATION_STANDARD   500
#define BLINK_DURATION_LONG       1000

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
    const char clk_am[] PROGMEM = "AM";
    const char clk_pm[] PROGMEM = "PM";
}

// RTC vars
DS3232RTC rtc;
String dayOfTheWeek, monthOfTheYear;
// LED vars
led::LED *wifi_led;
// NTP client vars
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
bool ntpSynced = false;
// Timezone vars
TimeChangeRule dstRule = {TZ_DST_NAME, TZ_DST_WEEK, TZ_DST_DAY, TZ_DST_MONTH, TZ_DST_HOUR, TZ_DST_OFFSET};
TimeChangeRule stdRule = {TZ_STD_NAME, TZ_STD_WEEK, TZ_STD_DAY, TZ_STD_MONTH, TZ_STD_HOUR, TZ_STD_OFFSET};
Timezone tz(dstRule, stdRule);
//MAX7219
MD_Parola Display = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
std::stringstream ss;

void on_wait_wifi_cb() {
    DEBUG("Wifi not yet available...")
    wifi_led->toggle();
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
    setSyncProvider(rtc.get);
    if (timeStatus() != timeSet) {
        DEBUG("Unable to sync with the RTC")
    } else {
        DEBUG("RTC has set the system time.")
    }
    dayOfTheWeek = getDayOfTheWeek(dayOfWeek(now()));
    monthOfTheYear = getMonthOfTheYear(month());
    // MAX7219
    Display.begin();
    Display.setIntensity(0);
    Display.displayClear();
    Display.setFont(ROG);
    Display.setTextAlignment(PA_LEFT);
    Display.print("Wifi...");
    // setup LED
    wifi_led = new led::LED(LED_WIFI);
    wifi_led->setup();
    // Connect to wifi with a custom wait status callback
    wifi_tools::startClient(on_wait_wifi_cb, BLINK_DURATION_STANDARD);
    wifi_led->on();
    Display.print("Sync...");
    // NTP client init
    timeClient.begin(); // Start client
    // TODO: Do we want a time offset here? Or rather use the Timezone lib?
    timeClient.setTimeOffset(0);
    if (!ntpSynced) {
        DEBUG("Attempting to sync time with NTP server.")
        // Get time from server
        while (!timeClient.update()) {} // Wait for update to take place.
        DEBUG("NTP time: ", timeClient.getFormattedTime().c_str())
        setTime(timeClient.getEpochTime()); // Set system time to NTP data
        rtc.set(now()); // Set RTC time using system time
        ntpSynced = true;
    }
}

String getTime(time_t t) {
    ss.str("");
    ss.clear();
    // TODO: read in AM/PM string. store outside function
    //  AM/PM display can only be when not showing seconds.
    int hr = hour(t);
    if (hr < 12) {
        hr -= 12;
    }
    ss << std::setfill('0') << std::setw(2) << hr << ':'
        << std::setfill('0') << std::setw(2) << minute(t) << ':'
        << std::setfill('0') << std::setw(2) << second(t);
    return ss.str().c_str();
}

void loop() {
    // wifi_led->toggle();
    // led::LED::loop();
    String timeStr = getTime(tz.toLocal(now()));
    Display.print(timeStr);
    Serial.print("Time: ");
    Serial.print(timeStr);
    Serial.println();
    delay(1000);
}
