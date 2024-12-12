#ifndef ESP_ALARM_CLOCK_SRC_CONFIGURATION_H_
#define ESP_ALARM_CLOCK_SRC_CONFIGURATION_H_

#include <JSON.h>

namespace config {

enum class Timezone {
  PT, MT, CT, ET
};

class Wifi : public json::JsonModel {
 private:
  String ssid_;
  String password_;

  bool onKey(String &key, json::JsonParser &parser) override;
};

class ClockConfig : public json::JsonModel {
 public:
  ClockConfig() = default;
  uint8_t GetBrightness() const;
  Timezone GetTz() const;
  bool IsAdjustDst() const;
  bool UseMilitaryTime() const;
  const String &GetZip() const;
  const Wifi &GetWifi() const;
  int GetSnooze() const;
 private:
  uint8_t brightness_;
  Timezone tz_;
  bool adjust_dst_;
  bool use_military_time_;
  String zip_;
  Wifi wifi_;
  int snooze_;

  bool onKey(String &key, json::JsonParser &parser) override;
};

class Configuration {
 public:
  Configuration() = default;
  bool load();
  const ClockConfig &GetClockConfig() const;
 private:
  ClockConfig clock_config_;
};

} // config

#endif //ESP_ALARM_CLOCK_SRC_CONFIGURATION_H_
