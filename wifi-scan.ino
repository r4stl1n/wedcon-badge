#include <Hash.h>

#include "wifi-scan.h"


static struct WifiInfo Wifi_networks[] = {
  { // WifiBride
    WIFI_NO_RSSI, { "928e56d4dcb9171ffd38f3646439eb6fb5283e39" }
  },
  { // WifiGroom
    WIFI_NO_RSSI, { "abf77bfa1018789b96e58fc6b883024272a4129d" }
  },
  { // WifiToast
    WIFI_NO_RSSI, { "ead0d6ae21e8b5792397753fa9e7d4a2772e7ce2" }
  },
  { // WifiRave
    WIFI_NO_RSSI, { "e380dd5149661a80f83da0d3411a03600e47a27c" }
  },
  { // (unused)
    WIFI_NO_RSSI, { "7648dedebcd721835e2908afb104c7e11a559612" }
  }
};

static unsigned long Wifi_toastStartedAt = 0;
static bool Wifi_broadcasting = false;
static bool Wifi_scanning = false;

static void Wifi_launchScan();
static void Wifi_waitForScan();

/**
static void say(String name) {
  Serial.printf(" '%s' => '%s' \n", name.c_str(), Wifi_GetHash(name).c_str());
}
/**/


void Wifi_Init() {
  WiFi.mode(WIFI_AP_STA);
  WiFi.disconnect();

  Wifi_launchScan();
}


void Wifi_Loop() {
  if (!Wifi_scanning) {
    return;
  }

/*
  delay(1);
/**/

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
        String ssid = WiFi.SSID(n);

        for (int n = 0; n <= WifiMax; n++) {
          Wifi_networks[n].rssi = WIFI_NO_RSSI;
        }

        for (int n = 0; n <= WifiMax; n++) {
          if (Wifi_GetHash(ssid).equals(Wifi_networks[n].ssidHash)) {
            Wifi_networks[n].rssi = WiFi.RSSI(n);

            switch (n) {
              case WifiToast:
              case WifiRave:
                if (Wifi_toastStartedAt == 0 || ((millis() - Wifi_toastStartedAt) > Config().wifi.toast.pause)) {
                  Wifi_toastStartedAt = millis();

                  switch (Mode_GetMode()) {
                    case ModeProximity:
                    case ModeCustom:
                    case ModeOff:
                    case ModeShowID:
                      if (!Wifi_broadcasting) {
                        Wifi_broadcasting = true;
                        Wifi_StartAPWithSecret(ssid);  
                      }
                      break;
                  }

                  LED_ChangePattern(n == WifiToast ? LEDToast : LEDRave);
                }
                break; 
            }
          }
        }
      }
      WiFi.scanDelete();
      break;
  }

  if (Wifi_broadcasting && ((millis() - Wifi_toastStartedAt) > Config().wifi.toast.duration)) {
    Wifi_StopAP(); 
    Wifi_broadcasting = false; 
  }

/**
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
/**/

  Wifi_launchScan();
}


void Wifi_Shutdown(){
  WiFi.disconnect();
}


bool Wifi_StartAP(String name) {
  Wifi_waitForScan();

  WiFi.softAPdisconnect(true);
  WiFi.disconnect();

  if (!WiFi.softAP(name.c_str(), NULL, Config().wifi.channel)) {
    Serial.printf("Failed to start WiFi network %s\n", name.c_str());
    return false;
  };


  Serial.printf("Started WiFi network %s\n", name.c_str());

//  delay(100);
  return true;
}


bool Wifi_StartAPWithSecret(String name) {
  Wifi_waitForScan();

  WiFi.softAPdisconnect(true);
  WiFi.disconnect();
  
  char tmp[100];
  sprintf(tmp, "time=%04x, id=%06x", micros(), ESP.getChipId());

  if (!WiFi.softAP(name, Wifi_GetHash(tmp), Config().wifi.channel)) {
    Serial.printf("Failed to start WiFi network %s\n", name.c_str());
    return false;
  };

  Serial.printf("Started WiFi network %s\n", name.c_str());

//  delay(100);
  return true;
}


void Wifi_StopAP() {
  WiFi.softAPdisconnect(true);
//  delay(100);
  Wifi_launchScan();
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
  WiFi.scanNetworks(true, false, Config().wifi.channel);
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
