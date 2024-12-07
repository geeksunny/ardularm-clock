#ifndef CLOCK_H
#define CLOCK_H

#include <WString.h>
#include <DS3232RTC.h>

namespace clock_ns {

typedef void (*on_wait_cb_t)();

String getTime(time_t t, char separator = ':', bool display_12hr = true);
String getTimeBinary(time_t t);
void setup();
bool isNTPSynced();
bool syncToNTP(on_wait_cb_t on_wait_cb = nullptr);

}

#endif //CLOCK_H
