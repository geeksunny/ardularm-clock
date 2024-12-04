#include "Configuration.h"
#include <PgmStringTools.hpp>
#include <EnumTools.hpp>

namespace config {

namespace strings {
// Config filenames
const char filename_config[] PROGMEM = "config.json";
const char filename_alarms[] PROGMEM = "alarms.json";

// Keys
const char key_tz[] PROGMEM = "tz";
const char key_dst[] PROGMEM = "adjust_dst";
const char key_zip[] PROGMEM = "zip";
const char key_wifi[] PROGMEM = "wifi";
const char key_ssid[] PROGMEM = "ssid";
const char key_password[] PROGMEM = "password";
const char key_snooze[] PROGMEM = "snooze";

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
              MAPPING(Timezone::PT, strings::value_pt),
              MAPPING(Timezone::MT, strings::value_mt),
              MAPPING(Timezone::CT, strings::value_ct),
              MAPPING(Timezone::ET, strings::value_et)
)

bool Configuration::onKey(String &key, json::JsonParser &parser) {
  STR_EQ_INIT(key.c_str())
  STR_CASE_EQ_DO(strings::key_tz, {
    String tz;
    bool success = parser.get(tz);
    tz_ = pgm_string_to_enum(tz.c_str(), Timezone::ET, timezone_map);
    return success;
  })
  STR_EQ_RET(strings::key_dst, parser.get(adjust_dst_))
  STR_EQ_RET(strings::key_wifi, parser.get(wifi_))
  STR_EQ_RET(strings::key_snooze, parser.get(snooze_))
  return false;
}

bool Wifi::onKey(String &key, json::JsonParser &parser) {
  STR_EQ_INIT(key.c_str())
  STR_EQ_RET(strings::key_ssid, parser.get(ssid_))
  STR_EQ_RET(strings::key_password, parser.get(password_))
  return false;
}

} // config