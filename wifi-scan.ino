#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <Hash.h>

#include "wifi-scan.h"


static struct WifiInfo Wifi_networks[] = {
  { // WifiBride
    WIFI_NO_RSSI, { WIFI_BRIDE_HASH }
  },
  { // WifiGroom
    WIFI_NO_RSSI, { WIFI_GROOM_HASH }
  },
  { // WifiToast
    WIFI_NO_RSSI, { WIFI_TOAST_HASH }
  },
  { // WifiRave
    WIFI_NO_RSSI, { WIFI_RAVE_HASH }
  }
};

static unsigned long Wifi_beaconStartedAt = 0;
static bool Wifi_beaconing = false;
static bool Wifi_scanning = false;
static int Wifi_state = WifiStateNone;

static bool Wifi_beaconStart(String ssid);
static void Wifi_beaconLoop();
static int Wifi_getBeaconInfo(String& beaconSSID);
static void Wifi_scanLoop();
static bool Wifi_startAP(String name, bool hasSecret);
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
  Wifi_beaconLoop();
  Wifi_scanLoop();
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
    case WifiStateListen:
      WiFi.softAPdisconnect(true);
      Wifi_launchScan();
      
      Serial.printf("  listening\n");
      break;
      
    case WifiStateSelf:
      Wifi_waitForScan();
      WiFi.disconnect();
      Wifi_startAP(Wifi_GetName(), false);
      
      Serial.printf("  started ssid %s\n", Wifi_GetName().c_str());
      break;
  }
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


bool Wifi_beaconStart(String ssid) {
  if (Wifi_beaconing || Wifi_beaconStartedAt > 0) {
    return false;
  }
  
  Wifi_beaconStartedAt = millis();
  Wifi_beaconing = true;
  Wifi_waitForScan();
  WiFi.disconnect();
  
  Wifi_startAP(ssid, true);
  
  Serial.printf("  beacon %s launched\n", ssid.c_str());

  return true;
}


void Wifi_beaconLoop() {
  if (!Wifi_beaconing && Wifi_beaconStartedAt == 0) {
    return;
  }
  
  if (Wifi_beaconing && (millis() - Wifi_beaconStartedAt) > Config().wifi.beacon.duration) {
    Wifi_beaconing = false;
    
    WiFi.softAPdisconnect(true);
    Wifi_launchScan();
    
    Serial.printf("  beacon stopped\n");
  }
  
  if (Wifi_beaconStartedAt > 0 && (millis() - Wifi_beaconStartedAt) > Config().wifi.beacon.pause) {
    Wifi_beaconStartedAt = 0;
    
    Serial.printf("  beacon timed out\n");
  }
}


int Wifi_getBeaconInfo(int scanResult, String& beaconSSID) {
  for (int n = 0; n <= WifiMax; n++) {
    Wifi_networks[n].rssi = WIFI_NO_RSSI;
  }

  static bool isBridePrev = false;
  static bool isGroomPrev = false;
  static bool isToastPrev = false;
  static bool isRavePrev = false;
  
  String toastSSID;
  String raveSSID;
  bool isBride = false;
  bool isGroom = false;
  bool isToast = false;
  bool isRave = false;
  
  for (int n = 0; n < scanResult; n++) {          
    String ssid;
    int32_t rssi;
    uint8_t encryptionType;
    uint8_t *bssid;
    int32_t channel;
    bool hidden;
    WiFi.getNetworkInfo(n, ssid, encryptionType, rssi, bssid, channel, hidden);
    
    for (int n = 0; n <= WifiMax; n++) {
      if (Wifi_GetHash(ssid).equals(Wifi_networks[n].ssidHash)) {
        Wifi_networks[n].rssi = rssi;

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

  if (isBride != isBridePrev || isGroom != isGroomPrev || isToast != isToastPrev || isRave != isRavePrev) {
    Serial.printf("  wifi: b=%d g=%d t=%d r=%d\n", isBride, isGroom, isToast, isRave);
    
    if (isToast != isToastPrev) {
      Serial.printf("  got toast\n");
    }
    
    if (isRave != isRavePrev) {
      Serial.printf("  got rave\n");
    }

    isBridePrev = isBride;
    isGroomPrev = isGroom;
    isToastPrev = isToast;
    isRavePrev = isRave;
  }
  
  if (isToast) {
    beaconSSID = toastSSID;
    return LEDToast;
  }
  
  if (isRave) {
    beaconSSID = raveSSID;
    return LEDRave;
  }

  return LEDOff;
}


void Wifi_scanLoop() {
  if (!Wifi_scanning) {
    return;
  }

  String beaconSSID;
  int beaconType = LEDOff;
  int scanResult = WiFi.scanComplete();
  switch (scanResult) {
    case 0:
      break;

    case -1: // still scanning
      return;

    case -2:
      break;

    default:
      beaconType = Wifi_getBeaconInfo(scanResult, beaconSSID);
      WiFi.scanDelete();  
      Wifi_scanning = false;
      break;
  }

  switch (beaconType) {
    case LEDToast:
    case LEDRave:
      if (Wifi_beaconStart(beaconSSID)) {
        LED_ChangePattern(beaconType);
      } else {
        Wifi_launchScan();
      }
      break;

    default:
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

  return true;
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
    delay(10);
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
