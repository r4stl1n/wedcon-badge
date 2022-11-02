#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <Hash.h>

#include "wifi-scan.h"


static struct WifiInfo Wifi_networks[] = {
  { // WifiBride
    WIFI_NO_RSSI, { WIFI_BRIDE }, 0
  },
  { // WifiGroom
    WIFI_NO_RSSI, { WIFI_GROOM }, 0
  },
  { // WifiToast
    WIFI_NO_RSSI, { WIFI_TOAST }, 0
  },
  { // WifiRave
    WIFI_NO_RSSI, { WIFI_RAVE }, 0
  }
};

static unsigned long Wifi_toastStartedAt = 0;
static bool Wifi_broadcasting = false;
static bool Wifi_scanning = false;
static bool Wifi_special = false;
static int Wifi_state = WifiStateNone;

static void Wifi_launchScan();
static void Wifi_waitForScan();
static String Wifi_typeName(int n);

/**
static void say(String name) {
  Serial.printf(" '%s' => '%s' \n", name.c_str(), Wifi_GetHash(name).c_str());
}
/**/


void Wifi_Init() {
  WiFi.mode(WIFI_AP_STA);
  Wifi_Switch(WifiStateListen);
}


void Wifi_Loop() {
  if (Wifi_broadcasting) {
    if ((millis() - Wifi_toastStartedAt) > Config().wifi.toast.duration) {
      WiFi.softAPdisconnect(true);
      Wifi_broadcasting = false; 
      Wifi_launchScan();
      Serial.printf("  stopped broadcasting\n");
    }

    return;
  }
  
  if (!Wifi_scanning) {
    return;
  }

  int scanResult = WiFi.scanComplete();
  bool isBride = false;
  bool isGroom = false;
  bool isToast = false;
  bool isRave = false;
  String toastSSID;
  String raveSSID;
  switch (scanResult) {
    case 0:
      break;

    case -1: // still scanning
      return;

    case -2:
      Serial.printf("WiFi scan error\n");
      break;

    default:
      for (int n = 0; n <= WifiMax; n++) {
        Wifi_networks[n].rssi = WIFI_NO_RSSI;
      }

      for (int n = 0; n < scanResult; n++) {          
        String ssid;
        int32_t rssi;
        uint8_t encryptionType;
        uint8_t *bssid;
        int32_t channel;
        bool hidden;
        WiFi.getNetworkInfo(n, ssid, encryptionType, rssi, bssid, channel, hidden);
//        Serial.printf("  %3d: %s = %s [rssi %d] (ch %d)\n", n + 1, ssid.c_str(), Wifi_GetHash(ssid).c_str(), rssi, channel);

        for (int n = 0; n <= WifiMax; n++) {
          if (Wifi_GetHash(ssid).equals(Wifi_networks[n].ssidHash)) {
            Wifi_networks[n].rssi = rssi;
            Wifi_networks[n].lastSeenAt = millis();

            switch (n) {
              case WifiBride:
                isBride = true;
                break;
                
              case WifiGroom:
                isGroom = true;
                break;
                
              case WifiToast:
                isToast = true;
                toastSSID = ssid;
                break;
                
              case WifiRave:
                isRave = true;
                raveSSID = ssid;
                break;
            }
          }
        }
      }
      WiFi.scanDelete();
      break;
  }

//  Serial.printf("  wifi: b=%d g=%d t=%d r=%d\n", isBride, isGroom, isToast, isRave);

  if (isToast) {
    if ((millis() - Wifi_toastStartedAt) > Config().wifi.toast.pause) {
      Wifi_toastStartedAt = millis();
      Wifi_special = true;
      Wifi_startAP(toastSSID, true);  
      LED_ChangePattern(LEDToast);
      Serial.printf("  broadcasting toast\n");

      return;
    }
  } else if (isRave) {
    if ((millis() - Wifi_toastStartedAt) > Config().wifi.toast.pause) {
      Wifi_toastStartedAt = millis();
      Wifi_special = true;
      Wifi_startAP(raveSSID, true);  
      LED_ChangePattern(LEDRave);
      Serial.printf("  broadcasting rave\n");

      return;
    }
  } else {
    if (Wifi_special && (millis() - Wifi_toastStartedAt) > Config().led.toast.timeout) {
      LED_ChangePattern(LEDPrevious);
      Wifi_special = false;
    }
  }
 
  Wifi_launchScan();
}


void Wifi_Shutdown() {
  Wifi_waitForScan();
  WiFi.disconnect();
  WiFi.softAPdisconnect(true);
}


void Wifi_Switch(int state) {
  if (state == Wifi_state) {
    return;
  }

  Wifi_state = state;
  switch (Wifi_state) {
    case WifiStateSelf:
      Wifi_startAP(Wifi_GetName(), false);
      break;

    case WifiStateListen:
      Wifi_Shutdown();
      Wifi_launchScan();
      break;
  }
}


bool Wifi_startAP(String name, bool hasSecret) {
  Wifi_Shutdown();

  if (hasSecret) {  
    char tmp[100];
    sprintf(tmp, "time=%04x, id=%06x", micros(), ESP.getChipId());

    if (!WiFi.softAP(name, Wifi_GetHash(tmp))) {
      Serial.printf("Failed to start WiFi network %s\n", name.c_str());
      return false;
    }
  } else {
    if (!WiFi.softAP(name)) {
      Serial.printf("Failed to start WiFi network %s\n", name.c_str());
      return false;
    }
  }

  Serial.printf("Started WiFi network %s\n", name.c_str());
  Wifi_broadcasting = true;

  return true;
}


int32_t Wifi_GetRSSI(int kind) {
  if (kind < 0 || kind > WifiMax) {
    return WIFI_NO_RSSI;
  }

  return Wifi_networks[kind].rssi;
}


bool Wifi_GetID(byte* id, int len) {
  if (len != Mode_ShowID_digitNum) {
    return false;
  }
  
  // convert ID to int array
  unsigned long chipID = ESP.getChipId();
  for (int n = Mode_ShowID_digitNum - 1; n >= 0; n--) {
    char d = chipID % 6;
    chipID /= 6;

    id[n] = d;
  }
  
  return true;
}


String Wifi_GetName() {
  const char digits[] = { '1', '2', '3', '4', '5', '6' };
  byte id[Mode_ShowID_digitNum];
  Wifi_GetID(id, Mode_ShowID_digitNum);

  char tmp[Mode_ShowID_digitNum + 1];
  tmp[Mode_ShowID_digitNum] = 0;

  // convert ID to string
  for (int n = 0; n < Mode_ShowID_digitNum; n++) {
    tmp[n] = digits[id[n]];
  }
  
  String name = WIFI_SSID_PREFIX;
  return name + tmp;
}


String Wifi_GetHash(String text) {
  return sha1(text);
}


void Wifi_launchScan() {
  Wifi_scanning = true;
  WiFi.scanNetworks(true, false, 1);
}


void Wifi_waitForScan() {
  if (!Wifi_scanning) {
    return;
  }

  while (WiFi.scanComplete() == -1) {
    delay(100);
  }  

  WiFi.scanDelete();
  Wifi_scanning = false;
}


String Wifi_typeName(int n) {
  switch (n) {
    case WifiBride:
      return "bride";
    
    case WifiGroom:
      return "groom";
    
    case WifiToast:
      return "toast";
    
    case WifiRave:
      return "rave";

    default:
      return String(n);
  }
}
