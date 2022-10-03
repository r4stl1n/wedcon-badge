#include <Hash.h>

#include "wifi-scan.h"


static struct WifiInfo Wifi_networks[] = {
  {
    WIFI_NO_RSSI, { "928e56d4dcb9171ffd38f3646439eb6fb5283e39" }
  },
  {
    WIFI_NO_RSSI, { "abf77bfa1018789b96e58fc6b883024272a4129d" }
  },
  {
    WIFI_NO_RSSI, { "ead0d6ae21e8b5792397753fa9e7d4a2772e7ce2" }
  },
  {
    WIFI_NO_RSSI, { "e380dd5149661a80f83da0d3411a03600e47a27c" }
  },
  { // wn01233
    WIFI_NO_RSSI, { "dbb202cdd2005f05250bd2d696b90e69d675b7c7" }
  },
  { // wn00000
    WIFI_NO_RSSI, { "c3f7f235400b3ad8c8a3e26da653440ed9745829" }
  },
  { // m+r_wedding_11-11__2ef970
    WIFI_NO_RSSI, { "e20bf269e86d51ef022db38723b2d7a90131d640" }
  }
};

static void Wifi_launchScan();

/*
static void say(String name) {
  Serial.printf(" '%s' => '%s' \n", name.c_str(), Wifi_GetHash(name).c_str());
}
*/


void Wifi_Init() {
  WiFi.mode(WIFI_AP_STA);
  WiFi.disconnect();

  Wifi_launchScan();
}


void Wifi_Loop() {
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
      for (int n = 0; n < scanResult; n++) {  
        String ssid;
        uint8_t encryptionType;
        int32_t rssi = WIFI_NO_RSSI;
        uint8_t* bssid;
        int32_t channel;
        bool hidden;

        WiFi.getNetworkInfo(n, ssid, encryptionType, rssi, bssid, channel, hidden);

        for (int n = 0; n <= WifiMax; n++) {
          Wifi_networks[n].rssi = WIFI_NO_RSSI;
        }

        for (int n = 0; n <= WifiMax; n++) {
          if (Wifi_GetHash(ssid).equals(Wifi_networks[n].ssidHash)) {
            Wifi_networks[n].rssi = rssi;
          }
        }
      }
      break;
  }

  static unsigned long skipped = 0;
  static unsigned long printed = 0;
  if ((millis() - printed) > 1000) {
//    Serial.printf("\n %d scans/sec = %f ms/scan\n", skipped + 1, 1000.0/float(skipped + 1));

    for (int n = 0; n <= WifiMax; n++) {
      if (Wifi_networks[n].rssi > WIFI_NO_RSSI) {
        Serial.printf("   %d: %6ddBm - %s\n", n + 1, Wifi_networks[n].rssi, Wifi_networks[n].ssidHash);
      }
    }
    
    printed = millis();
    skipped = 0;
  } else {
    skipped++;
  }

  Wifi_launchScan();
}


void Wifi_Shutdown(){
  WiFi.scanDelete();
  WiFi.disconnect();
}


int32_t Wifi_GetRSSI(int kind) {
  if (kind < 0 || kind > WifiMax) {
    return WIFI_NO_RSSI;
  }

  return Wifi_networks[kind].rssi;
}


String Wifi_GetName() {
    char tmp[40];
    sprintf(tmp, WIFI_SSID_PREFIX "%06x", ESP.getChipId());
    return String(tmp);
}


String Wifi_GetHash(String text) {
    return sha1(text);
}


void Wifi_launchScan() {
  WiFi.scanNetworks(true, false, Config().wifi.channel);
}
