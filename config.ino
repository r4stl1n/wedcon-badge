#include <ArduinoJson.h>
#include <SD.h>

#include "config.h"
#include "led.h"
#include "mode.h"
#include "wifi-scan.h"


static struct ConfigData Config_data;
static StaticJsonDocument<1000> doc;


void Config_Defaults() {
  Config_data.wifi.channel          = WIFI_CHANNEL;
  Config_data.wifi.toast.duration   = WIFI_TOAST_DURATION;
  Config_data.wifi.toast.pause      = WIFI_TOAST_PAUSE;
  Config_data.hold.timeout          = BUTTON_HOLD;
  Config_data.led.luminance.low     = LED_BRIGHTNESS_LOW;
  Config_data.led.luminance.high    = LED_BRIGHTNESS_HIGH;
  Config_data.led.both.hue          = LED_HUE_BOTH;
  Config_data.led.both.saturation   = LED_SATURATION_BOTH;
  Config_data.led.bride.hue         = LED_HUE_BRIDE;
  Config_data.led.bride.saturation  = LED_SATURATION_BRIDE;
  Config_data.led.groom.hue         = LED_HUE_GROOM;
  Config_data.led.groom.saturation  = LED_SATURATION_GROOM;
  Config_data.led.none.saturation   = LED_SATURATION_NONE;
  Config_data.led.none.luminance    = LED_BRIGHTNESS_NONE;
  Config_data.led.toast.hue         = LED_HUE_TOAST;
  Config_data.led.toast.saturation  = LED_SATURATION_TOAST;
  Config_data.led.toast.luminance   = LED_BRIGHTNESS_HIGH;
  Config_data.led.toast.timeout     = LED_TOAST_TIMEOUT;
  Config_data.led.intro.cycles      = LED_INTRO_CYCLES;
  Config_data.proximity.rssi.low    = MODE_PROXIMITY_RSSI_LOW;
  Config_data.proximity.rssi.high   = MODE_PROXIMITY_RSSI_HIGH;
  Config_data.proximity.timeout     = MODE_PROXIMITY_TIMEOUT;
}


bool Config_Load() {
  Config_Defaults();

  File configFile = SPIFFS.open(CONFIG_FILE_NAME, "r");
  if (!configFile) {
    Serial.println("Failed to open config file, using default configuration");
    return false;
  }

  doc.clear();
  DeserializationError configError = deserializeJson(doc, configFile);  
  configFile.close();

  if (configError) {
    Serial.println("Failed to load config file, using default configuration");
    return false;
  }

  serializeJsonPretty(doc, Serial);
  Serial.println();

  Config_data.wifi.channel          = doc[F("wifi")][F("channel")]                 | WIFI_CHANNEL;
  Config_data.wifi.toast.duration   = doc[F("wifi")][F("toast")][F("duration")]    | WIFI_TOAST_DURATION;
  Config_data.wifi.toast.pause      = doc[F("wifi")][F("toast")][F("pause")]       | WIFI_TOAST_PAUSE;
  Config_data.hold.timeout          = doc[F("hold")][F("timeout")]                 | BUTTON_HOLD;
  Config_data.led.luminance.low     = doc[F("led")][F("luminance")][F("low")]      | LED_BRIGHTNESS_LOW;
  Config_data.led.luminance.high    = doc[F("led")][F("luminance")][F("high")]     | LED_BRIGHTNESS_HIGH;
  Config_data.led.both.hue          = doc[F("led")][F("both")][F("hue")]           | LED_HUE_BOTH;
  Config_data.led.both.saturation   = doc[F("led")][F("both")][F("saturation")]    | LED_SATURATION_BOTH;
  Config_data.led.bride.hue         = doc[F("led")][F("bride")][F("hue")]          | LED_HUE_BRIDE;
  Config_data.led.bride.saturation  = doc[F("led")][F("bride")][F("saturation")]   | LED_SATURATION_BRIDE;
  Config_data.led.groom.hue         = doc[F("led")][F("groom")][F("hue")]          | LED_HUE_GROOM;
  Config_data.led.groom.saturation  = doc[F("led")][F("groom")][F("saturation")]   | LED_SATURATION_GROOM;
  Config_data.led.none.saturation   = doc[F("led")][F("none")][F("saturation")]    | LED_SATURATION_NONE;
  Config_data.led.none.luminance    = doc[F("led")][F("none")][F("luminance")]     | LED_BRIGHTNESS_NONE;
  Config_data.led.toast.hue         = doc[F("led")][F("toast")][F("hue")]          | LED_HUE_TOAST;
  Config_data.led.toast.saturation  = doc[F("led")][F("toast")][F("saturation")]   | LED_SATURATION_TOAST;
  Config_data.led.toast.luminance   = doc[F("led")][F("toast")][F("luminance")]    | LED_BRIGHTNESS_HIGH;
  Config_data.led.toast.timeout     = doc[F("led")][F("toast")][F("timeout")]      | LED_TOAST_TIMEOUT;
  Config_data.led.intro.cycles      = doc[F("led")][F("intro")][F("cycles")]       | LED_INTRO_CYCLES;
  Config_data.proximity.rssi.low    = doc[F("proximity")][F("rssi")][F("low")]     | MODE_PROXIMITY_RSSI_LOW;
  Config_data.proximity.rssi.high   = doc[F("proximity")][F("rssi")][F("high")]    | MODE_PROXIMITY_RSSI_HIGH;
  Config_data.proximity.timeout     = doc[F("proximity")][F("timeout")]            | MODE_PROXIMITY_TIMEOUT;

  return true;
}


bool Config_Save() {
  doc.clear();

  doc[F("wifi")][F("channel")]                = Config_data.wifi.channel;
  doc[F("hold")][F("timeout")]                = Config_data.hold.timeout;
  doc[F("wifi")][F("toast")][F("duration")]   = Config_data.wifi.toast.duration;
  doc[F("wifi")][F("toast")][F("pause")]      = Config_data.wifi.toast.pause;
  doc[F("led")][F("luminance")][F("low")]     = Config_data.led.luminance.low;
  doc[F("led")][F("luminance")][F("high")]    = Config_data.led.luminance.high;
  doc[F("led")][F("both")][F("hue")]          = Config_data.led.both.hue;
  doc[F("led")][F("both")][F("saturation")]   = Config_data.led.both.saturation;
  doc[F("led")][F("bride")][F("hue")]         = Config_data.led.bride.hue;
  doc[F("led")][F("bride")][F("saturation")]  = Config_data.led.bride.saturation;
  doc[F("led")][F("groom")][F("hue")]         = Config_data.led.groom.hue;
  doc[F("led")][F("groom")][F("saturation")]  = Config_data.led.groom.saturation;
  doc[F("led")][F("none")][F("saturation")]   = Config_data.led.none.saturation;
  doc[F("led")][F("none")][F("luminance")]    = Config_data.led.none.luminance;
  doc[F("led")][F("toast")][F("hue")]         = Config_data.led.toast.hue;
  doc[F("led")][F("toast")][F("saturation")]  = Config_data.led.toast.saturation;
  doc[F("led")][F("toast")][F("luminance")]   = Config_data.led.toast.luminance;
  doc[F("led")][F("toast")][F("timeout")]     = Config_data.led.toast.timeout;
  doc[F("led")][F("intro")][F("cycles")]      = Config_data.led.intro.cycles;
  doc[F("proximity")][F("rssi")][F("low")]    = Config_data.proximity.rssi.low;
  doc[F("proximity")][F("rssi")][F("high")]   = Config_data.proximity.rssi.high;
  doc[F("proximity")][F("timeout")]           = Config_data.proximity.timeout;

  File configFile = SPIFFS.open(CONFIG_FILE_NAME, "w");
  if (!configFile) {
    Serial.printf("Failed to open config file %s\n", CONFIG_FILE_NAME);
    return false;
  }

  serializeJsonPretty(doc, configFile);
  configFile.close();

  return true;
}


const struct ConfigData& Config() {
  return Config_data;
}
