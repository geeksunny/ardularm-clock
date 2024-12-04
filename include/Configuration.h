#ifndef ESP_ALARM_CLOCK_SRC_CONFIGURATION_H_
#define ESP_ALARM_CLOCK_SRC_CONFIGURATION_H_

#include <JSON.h>

namespace config {

enum class Timezone {
  PT, MT, CT, ET
};

class Configuration : json::JsonModel {
 private:
  Timezone tz_;
  bool adjust_dst_;
  String zip_;
  Wifi wifi_;
  int snooze_;

  bool onKey(String &key, json::JsonParser &parser) override;
};

class Wifi : json::JsonModel {
 private:
  String ssid_;
  String password_;

  bool onKey(String &key, JsonParser &parser) override;
};

} // config

#endif //ESP_ALARM_CLOCK_SRC_CONFIGURATION_H_
