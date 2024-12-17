#ifndef CLOCK_H
#define CLOCK_H

#include <WString.h>
#include <DS3232RTC.h>
#include "Timezone.h"
#include "Configuration.h"

namespace clock_ns {

class Clock {
 public:
  explicit Clock(const config::Timezone &tz);

  String getTime(char separator = ':',
                 bool display_12hr = true,
                 bool show_leading_zero = true,
                 bool show_seconds = true);
  String getTimeBinary(time_t t);
  bool isNTPSynced();
  bool syncToNTP();

 private:
  DS3232RTC rtc_;
  Timezone tz_;
  bool ntpSynced_ = false;
  String dayOfTheWeek_, monthOfTheYear_;
};

}

#endif //CLOCK_H
