#include "Configuration.h"
#include "DebugLog.hpp"
#include <PgmStringTools.hpp>
#include <EnumTools.hpp>
#include <SD.h>

namespace config {

namespace strings {
// Config filenames
const char filename_config[] PROGMEM = "config.json";
const char filename_alarms[] PROGMEM = "alarms.json";

// Keys
const char key_brightness[] PROGMEM = "brightness";
const char key_tz[] PROGMEM = "tz";
const char key_dst[] PROGMEM = "adjust_dst";
const char key_military_time[] PROGMEM = "military_time";
const char key_show_seconds[] PROGMEM = "show_seconds";
const char key_show_leading_zero[] PROGMEM = "show_leading_zero";
const char key_show_suffix[] PROGMEM = "show_suffix";
const char key_snooze[] PROGMEM = "snooze";
const char key_wifi[] PROGMEM = "wifi";
const char key_ssid[] PROGMEM = "ssid";
const char key_password[] PROGMEM = "password";
const char key_zip[] PROGMEM = "zip";

const char key_start[] PROGMEM = "start";
const char key_stop[] PROGMEM = "stop";
const char key_interval[] PROGMEM = "interval";
const char key_days[] PROGMEM = "days";

// Values
const char value_pt[] PROGMEM = "PT";
const char value_mt[] PROGMEM = "MT";
const char value_ct[] PROGMEM = "CT";
const char value_et[] PROGMEM = "ET";
}

MAKE_ENUM_MAP(timezone_map, Timezone,
              MAPPING(Timezone::ET, strings::value_et),
              MAPPING(Timezone::CT, strings::value_ct),
              MAPPING(Timezone::MT, strings::value_mt),
              MAPPING(Timezone::PT, strings::value_pt)
)

bool ClockConfig::onKey(String &key, json::JsonParser &parser) {
  STR_EQ_INIT(key.c_str())
  STR_CASE_EQ_DO(strings::key_brightness, {
    bool success = parser.get(brightness_);
    if (brightness_ <= 0) {
      brightness_ = 0;
    } else if (brightness_ > 15) {
      brightness_ = 15;
    }
    return success;
  })
  STR_CASE_EQ_DO(strings::key_tz, {
    String tz;
    bool success = parser.get(tz);
    tz.toUpperCase();
    tz_ = pgm_string_to_enum(tz.c_str(), Timezone::ET, timezone_map);
    return success;
  })
  STR_EQ_RET(strings::key_dst, parser.get(adjust_dst_))
  STR_EQ_RET(strings::key_military_time, parser.get(use_military_time_))
  STR_EQ_RET(strings::key_show_seconds, parser.get(show_seconds_))
  STR_EQ_RET(strings::key_show_leading_zero, parser.get(show_leading_zero_))
  STR_EQ_RET(strings::key_snooze, parser.get(snooze_))
  STR_EQ_RET(strings::key_wifi, parser.get(wifi_))
  STR_EQ_RET(strings::key_zip, parser.get(zip_))
  return false;
}

uint8_t ClockConfig::GetBrightness() const {
  return brightness_;
}

Timezone ClockConfig::GetTz() const {
  return tz_;
}

bool ClockConfig::IsAdjustDst() const {
  return adjust_dst_;
}

bool ClockConfig::UseMilitaryTime() const {
  return use_military_time_;
}

const String &ClockConfig::GetZip() const {
  return zip_;
}

const Wifi &ClockConfig::GetWifi() const {
  return wifi_;
}

int ClockConfig::GetSnooze() const {
  return snooze_;
}

bool ClockConfig::ShowSeconds() const {
  return show_seconds_;
}

bool ClockConfig::ShowLeadingZero() const {
  return show_leading_zero_;
}

bool Wifi::onKey(String &key, json::JsonParser &parser) {
  STR_EQ_INIT(key.c_str())
  STR_EQ_RET(strings::key_ssid, parser.get(ssid_))
  STR_EQ_RET(strings::key_password, parser.get(password_))
  return false;
}

const String &Wifi::GetSsid() const {
  return ssid_;
}

const String &Wifi::GetPassword() const {
  return password_;
}

const ClockConfig &Configuration::GetClockConfig() const {
  return clock_config_;
}

bool Configuration::load() {
  String filename;
  File file;  bool success_config, success_alarms;

  if (!SD.begin(PIN_SD_CS)) {
    return false;
  }

  // Config
  {
    filename = read_prog_str(strings::filename_config);
    file = SD.open(filename);
    json::JsonParser parser(file);
    success_config = parser.get(clock_config_);
    if (!success_config) {
      DEBUG("Error parsing ", filename.c_str())
    }
    file.close();
  }

  return success_config && success_alarms;
}

} // config