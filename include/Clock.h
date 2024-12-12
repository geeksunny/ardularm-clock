#ifndef CLOCK_H
#define CLOCK_H

#include <WString.h>
#include <DS3232RTC.h>

namespace clock_ns {

String getTime(time_t t, char separator = ':', bool display_12hr = true);
String getTimeBinary(time_t t);
void setup();
bool isNTPSynced();
bool syncToNTP();

}

#endif //CLOCK_H
