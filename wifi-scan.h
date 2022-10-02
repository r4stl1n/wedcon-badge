#pragma once


// Wifi settings
#define WIFI_CHANNEL         11
#define WIFI_SSID_PREFIX     "m+r_wedding_11-11__"
#define WIFI_SSID_BRIDE      (MODE_1_WIFI_SSID_PREFIX "bride")
#define WIFI_SSID_GROOM      (MODE_1_WIFI_SSID_PREFIX "groom")
#define WIFI_NO_RSSI         -1000


enum {
	WifiBride = 0,
	WifiGroom,
	WifiToast,
	WifiParty,

  WifiMax = WifiParty
};

struct WifiInfo {
  int32_t rssi;
  char*   ssidHash;
};


void Wifi_Init();
void Wifi_Loop();
void Wifi_Shutdown();

int32_t Wifi_GetRSSI(int kind);
String Wifi_GetName();
String Wifi_GetHash(String text);
