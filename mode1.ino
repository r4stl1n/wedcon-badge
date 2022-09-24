#include <ESP8266WiFi.h>
#include <FastLED.h>

#include "mode1.h"


static CRGB Mode_1_leds[MODE_1_NUM_LEDS];
static uint Mode_1_brightness = 0;
static uint Mode_1_hue = 0;

static unsigned long Mode_1_lastSeenBride = 0;
static unsigned long Mode_1_lastSeenGroom = 0;
static bool Mode_1_scanningForBride = false;

static void Mode_1_updateRSSI();
static void Mode_1_updateLEDs();
static uint Mode_1_getNewBrightness();
static void Mode_1_launchWifiScan();
static void Mode_1_evalRSSI();
static unsigned long Mode_1_evalLastSeen(unsigned long lastSeen, int32_t maxRSSI);


void Mode_1_Init() {
  FastLED.addLeds<NEOPIXEL, MODE_1_DATA_PIN>(Mode_1_leds, MODE_1_NUM_LEDS);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Mode_1_launchWifiScan();
}


void Mode_1_Shutdown() {
  WiFi.scanDelete();
  WiFi.disconnect();

  FastLED.showColor(CHSV(0, 0, 0)); 
  FastLED.clear();
}


void Mode_1_Loop() {
  Mode_1_scanForCouple();
  Mode_1_updateLEDs();
}


void Mode_1_scanForCouple() {
  int32_t maxRSSI = -1000;
  int scanResult = WiFi.scanComplete();
  switch (scanResult) {
    case 0:
      break;

    case -1: // still scanning
      return;

    case -2:
      Serial.printf("WiFi scan error\n");   
      break;

    default:
      maxRSSI = Mode_1_evalRSSI(scanResult);
      break;
  }

  if (Mode_1_scanningForBride) {
    Mode_1_lastSeenBride = Mode_1_evalLastSeen(Mode_1_lastSeenBride, maxRSSI);
  } else {
    Mode_1_lastSeenGroom = Mode_1_evalLastSeen(Mode_1_lastSeenGroom, maxRSSI);
  }

  Mode_1_launchWifiScan();
}


void Mode_1_updateLEDs() {
  if (Mode_1_lastSeenBride > 0 && Mode_1_lastSeenGroom > 0) {
    FastLED.showColor(CHSV(MODE_1_HUE_BOTH, MODE_1_SAT_BOTH, Mode_1_getNewBrightness()));  
  } else if (Mode_1_lastSeenBride > 0 && Mode_1_lastSeenGroom == 0) {
    FastLED.showColor(CHSV(MODE_1_HUE_BRIDE, MODE_1_SAT_BRIDE, Mode_1_getNewBrightness())); 
  } else if (Mode_1_lastSeenBride == 0 && Mode_1_lastSeenGroom > 0) {
    FastLED.showColor(CHSV(MODE_1_HUE_GROOM, MODE_1_SAT_GROOM, Mode_1_getNewBrightness())); 
  } else {
    FastLED.showColor(CHSV(Mode_1_hue++, MODE_1_SAT_NONE, MODE_1_BRIGHTNESS_NONE)); 
  }
      
  delay(20);
}


uint Mode_1_getNewBrightness() {
  const uint range = MODE_1_BRIGHTNESS_HIGH - MODE_1_BRIGHTNESS_LOW;
  Mode_1_brightness = (Mode_1_brightness + 1) % (range * 2);
  return (Mode_1_brightness > range ? 2 * range - Mode_1_brightness : Mode_1_brightness) + MODE_1_BRIGHTNESS_LOW;
}


void Mode_1_launchWifiScan() {
  Mode_1_scanningForBride = !Mode_1_scanningForBride;
  WiFi.scanNetworks(true, false, MODE_1_WIFI_CHANNEL, (uint8*)(Mode_1_scanningForBride ? MODE_1_WIFI_SSID_BRIDE : MODE_1_WIFI_SSID_GROOM));
}


int32_t Mode_1_evalRSSI(int scanResult) {
  int32_t maxRSSI = -1000;
  for (int n = 0; n < scanResult; n++)
  {      
    String ssid;
    uint8_t encryptionType;
    int32_t rssi;
    uint8_t* bssid;
    int32_t channel;
    bool hidden;

    WiFi.getNetworkInfo(n, ssid, encryptionType, rssi, bssid, channel, hidden);

    if (rssi > maxRSSI) {
      maxRSSI = rssi;
    }
  }

  static unsigned long lastPrintedBride = 0;
  static unsigned long lastPrintedGroom = 0;
  if (Mode_1_scanningForBride) {
    if (millis() - lastPrintedBride > 1000) {
      Serial.printf("bride: %ddBm\n", maxRSSI);
      lastPrintedBride = millis();
    }
  } else {
    if (millis() - lastPrintedGroom > 1000) {
      Serial.printf("groom: %ddBm\n", maxRSSI);
      lastPrintedGroom = millis();
    }
  }

  return maxRSSI;
}


unsigned long Mode_1_evalLastSeen(unsigned long lastSeen, int32_t maxRSSI) {
  if (maxRSSI >= MODE_1_WIFI_RSSI_HIGH) {
    Serial.println("on");
    return millis();
  }

  if (lastSeen == 0) {
    Serial.println("never");
    return 0;    
  }
  
  if ((millis() - lastSeen) > MODE_1_PROXIMITY_TIMEOUT) {
    Serial.println("timeout");
    return 0;    
  }
  
  if (maxRSSI >= MODE_1_WIFI_RSSI_LOW) {
    Serial.println("near");
    return millis();
  }

  Serial.printf(" delta: %dms\n", millis() - lastSeen);

  Serial.println(".");
  return lastSeen;
}
