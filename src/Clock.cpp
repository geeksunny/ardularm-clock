#include "Clock.h"
#include <DebugLog.hpp>
#include <prog_str.h>
#include <sstream>
#include <iomanip>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "matrix/font.h"

namespace clock_ns {

std::stringstream ss;

static Timezone createTimezone(config::Timezone tz) {
  // TODO: remove DST definition if disabled via cfg.adjust_dst.
  return Timezone(
      TimeChangeRule{"", week_t::Second, dow_t::Sun, month_t::Feb, 2, 60 * tz},
      TimeChangeRule{"", week_t::First, dow_t::Sun, month_t::Nov, 2, 60 * (tz + 1)}
  );
}

Clock::Clock(const config::Timezone &tz) : tz_(createTimezone(tz)) {
  // RTC init
  rtc_.begin();
  setSyncProvider(rtc_.get);
  if (timeStatus() != timeSet) {
    DEBUG("Unable to sync with the RTC")
  } else {
    DEBUG("RTC has set the system time.")
  }
  dayOfTheWeek_ = dayShortStr(dayOfWeek(now()));
  monthOfTheYear_ = monthShortStr(month());
}

String Clock::getTime(char separator, bool display_12hr, bool show_leading_zero, bool show_seconds) {
  time_t t = tz_.toLocal(now());
  ss.str("");
  ss.clear();
  int hr = hour(t);
  if (display_12hr && hr > 12) {
    hr -= 12;
  }
  if (show_leading_zero) {
    ss << std::setfill('0') << std::setw(2);
  }
  ss << hr << separator << std::setfill('0') << std::setw(2) << minute(t);
  if (show_seconds) {
    ss << separator << std::setfill('0') << std::setw(2) << second(t);
  }
  return ss.str().c_str();
}

String Clock::getTimeBinary(time_t t) {
  String time = getTime(' ', false);
  uint8_t len = time.length();
  for (int i = 0; i < len; i++) {
    if (time[i] != ' ') {
      time[i] = time[i] - char(BINARY_CHAR_OFFSET);
    }
  }
  return time;
}

bool Clock::isNTPSynced() {
  return ntpSynced_;
}

bool Clock::syncToNTP() {
  WiFiUDP ntpUDP;
  NTPClient timeClient(ntpUDP, "pool.ntp.org");
  timeClient.begin();
  timeClient.setTimeOffset(0);
  DEBUG("Attempting to sync time with NTP server.")
  // Get time from server
  while (!timeClient.update()) {
    // Wait for update to take place.
  }
  DEBUG("NTP time: ", timeClient.getFormattedTime().c_str())
  setTime(timeClient.getEpochTime()); // Set system time to NTP data
  ntpSynced_ = rtc_.set(now()) == 0; // Set RTC time using system time
  return ntpSynced_;
}

}
