#pragma once


// Wifi settings
#define WIFI_CHANNEL         11
#define WIFI_SSID_PREFIX     "madroy_11-11_"
#define WIFI_NO_RSSI         -1000
#define WIFI_TOAST_DURATION  1000
#define WIFI_TOAST_PAUSE     60000


enum {
	WifiBride = 0,
	WifiGroom,
	WifiToast,
	WifiRave,

  WifiMax = WifiRave
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
bool Wifi_GetID(byte* id, int len); // use: byte id[Mode_ShowID_digitNum];
String Wifi_GetName();
String Wifi_GetHash(String text);
