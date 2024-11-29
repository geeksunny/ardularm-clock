#include "Clock.h"
#include <DebugLog.hpp>
#include <prog_str.h>
#include <sstream>
#include <iomanip>
#include <NTPClient.h>
#include <WiFiUdp.h>

namespace clock_ns {

// RTC vars
DS3232RTC rtc;
// NTP vars
bool ntpSynced = false;
// Display vars
String dayOfTheWeek, monthOfTheYear;
std::stringstream ss;

String getTime(time_t t) {
  ss.str("");
  ss.clear();
  int hr = hour(t);
  if (hr < 12) {
    hr -= 12;
  }
  ss << std::setfill('0') << std::setw(2) << hr << ':'
     << std::setfill('0') << std::setw(2) << minute(t) << ':'
     << std::setfill('0') << std::setw(2) << second(t);
  return ss.str().c_str();
}

void setup() {
  // RTC init
  rtc.begin();
  setSyncProvider(rtc.get);
  if (timeStatus() != timeSet) {
    DEBUG("Unable to sync with the RTC")
  } else {
    DEBUG("RTC has set the system time.")
  }
  dayOfTheWeek = dayShortStr(dayOfWeek(now()));
  monthOfTheYear = monthShortStr(month());
}

bool isNTPSynced() {
  return ntpSynced;
}

bool syncToNTP(on_wait_cb_t on_wait_cb) {
  WiFiUDP ntpUDP;
  NTPClient timeClient(ntpUDP, "pool.ntp.org");
  timeClient.begin();
  timeClient.setTimeOffset(0);
  DEBUG("Attempting to sync time with NTP server.")
  // Get time from server
  while (!timeClient.update()) {
    // Wait for update to take place.
    if (on_wait_cb) {
      on_wait_cb();
    }
  }
  DEBUG("NTP time: ", timeClient.getFormattedTime().c_str())
  setTime(timeClient.getEpochTime()); // Set system time to NTP data
  ntpSynced = rtc.set(now()) == 0; // Set RTC time using system time
  return ntpSynced;
}

}
