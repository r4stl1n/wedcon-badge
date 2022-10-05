#pragma once


// Wifi settings
#define WIFI_CHANNEL         11
#define WIFI_SSID_PREFIX     "m+r_wedding_11-11__"
#define WIFI_NO_RSSI         -1000
#define WIFI_TOAST_DURATION  1000
#define WIFI_TOAST_PAUSE     60000


enum {
	WifiBride = 0,
	WifiGroom,
	WifiToast,
	WifiRaveOn,
	WifiRaveOff,

  WifiMax = WifiRaveOff
};

struct WifiInfo {
  int32_t rssi;
  char*   ssidHash;
};


void Wifi_Init();
void Wifi_Loop();
void Wifi_Shutdown();

bool Wifi_StartAP(String name);
bool Wifi_StartAPWithSecret(String name);
void Wifi_StopAP();
int32_t Wifi_GetRSSI(int kind);
String Wifi_GetName();
String Wifi_GetHash(String text);
