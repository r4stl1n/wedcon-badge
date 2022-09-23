#include <ESP8266WiFi.h>
#include <FastLED.h>

#include "mode1.h"


static CRGB Mode_1_leds[MODE_1_NUM_LEDS];
static int32_t Mode_1_lastRssiBride = 0;
static int32_t Mode_1_lastRssiGroom = 0;
static bool Mode_1_scanningForBride = false;
static uint Mode_1_brightness = 0;
static uint Mode_1_hue = 0;

static void Mode_1_updateRSSI();
static void Mode_1_updateLEDs();
static void Mode_1_launchWifiScan();


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
  Mode_1_updateRSSI();

  if (Mode_1_lastRssiBride < 0) {
//    Serial.printf("bride: %ddBm   ", Mode_1_lastRssiBride);
  }

  if (Mode_1_lastRssiGroom < 0) {
//    Serial.printf("groom: %ddBm   ", Mode_1_lastRssiGroom);
  }

//  Serial.println("");

  Mode_1_updateLEDs();
}


void Mode_1_updateRSSI() {
  int32_t maxRSSI = 1;
  int scanResult = WiFi.scanComplete();
  switch (scanResult) {
    case 0:
//        Serial.printf("No networks found\n");
      break;

    case -1: // still scanning
      return;

    case -2:
      Serial.printf("WiFi scan error\n");   
      break;

    default:
      for (int n = 0; n < scanResult; n++)
      {      
        String ssid;
        uint8_t encryptionType;
        int32_t rssi;
        uint8_t* bssid;
        int32_t channel;
        bool hidden;

        WiFi.getNetworkInfo(0, ssid, encryptionType, rssi, bssid, channel, hidden);

        if (maxRSSI > 0 || rssi > maxRSSI) {
          maxRSSI = rssi;
        }
      }
      break;
  }

  //  Serial.printf("%d network(s) found:\n", scanResult);

  /*
  Serial.printf(" %s => [CH %02d] [%02X:%02X:%02X:%02X:%02X:%02X] %ddBm\n",
        ssid.c_str(),
        channel,
        bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5],
        maxRSSI);
  */

  if (maxRSSI < 0) {
    if (Mode_1_scanningForBride) {
      Mode_1_lastRssiBride = maxRSSI;
    } else {
      Mode_1_lastRssiGroom = maxRSSI;
    }
  }

  Mode_1_launchWifiScan();
}


void Mode_1_updateLEDs() {
  const uint range = MODE_1_BRIGHTNESS_HIGH - MODE_1_BRIGHTNESS_LOW;
  uint newBrightness = (Mode_1_brightness > 127 ? 255 - Mode_1_brightness : Mode_1_brightness) + MODE_1_BRIGHTNESS_LOW;
  Mode_1_brightness = (Mode_1_brightness + 1) % ((MODE_1_BRIGHTNESS_HIGH - MODE_1_BRIGHTNESS_LOW) * 2);

  if (Mode_1_brightness == 0) {
    Mode_1_hue++;
//    Mode_1_hue += 16;
//    Serial.printf("hue %d\n", Mode_1_hue);  
  }

/*
  if (Mode_1_lastRssiGroom >= MODE_1_WIFI_RSSI_LIMIT && Mode_1_lastRssiBride >= MODE_1_WIFI_RSSI_LIMIT) {
    FastLED.showColor(CHSV(MODE_1_HUE_BOTH, 255, newBrightness)); 
  } else if (Mode_1_lastRssiGroom < MODE_1_WIFI_RSSI_LIMIT && Mode_1_lastRssiBride >= MODE_1_WIFI_RSSI_LIMIT) {
    FastLED.showColor(CHSV(MODE_1_HUE_BRIDE, 255, newBrightness)); 
  } else if (Mode_1_lastRssiGroom >= MODE_1_WIFI_RSSI_LIMIT && Mode_1_lastRssiBride < MODE_1_WIFI_RSSI_LIMIT) {
    FastLED.showColor(CHSV(MODE_1_HUE_GROOM, 255, newBrightness)); 
  } else {
    FastLED.showColor(CHSV(Mode_1_hue++, 255, 50)); 
  }
  */
//    FastLED.showColor(CHSV(MODE_1_HUE_BOTH, 255, newBrightness)); 
//    FastLED.showColor(CHSV(MODE_1_HUE_BRIDE, 255, newBrightness)); // <= FIX ME: only led1 and only red
//    FastLED.showColor(CHSV(MODE_1_HUE_GROOM, 255, newBrightness));  // <= FIX ME: only led1 and only black
//    FastLED.showColor(CHSV(Mode_1_hue++, 255, 50)); 

  switch (Mode_1_hue % 4) {
  case 0:
    FastLED.showColor(CHSV(MODE_1_HUE_BOTH, 255, newBrightness)); 
    break;

  case 1:
    FastLED.showColor(CHSV(MODE_1_HUE_BRIDE, 255, newBrightness)); 
    break;
    
  case 2:
    FastLED.showColor(CHSV(MODE_1_HUE_GROOM, 255, newBrightness)); 
    break;
    
  case 3:
    FastLED.showColor(CHSV(0, 0, newBrightness)); 
    break;
  }

  delay(20);
}


void Mode_1_launchWifiScan() {
    Mode_1_scanningForBride = !Mode_1_scanningForBride;
    WiFi.scanNetworks(true, false, MODE_1_WIFI_CHANNEL, (uint8*)(Mode_1_scanningForBride ? MODE_1_WIFI_SSID_BRIDE : MODE_1_WIFI_SSID_GROOM));
}