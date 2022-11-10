#pragma once


#define WIFI_BRIDE_HASH "928e56d4dcb9171ffd38f3646439eb6fb5283e39"
#define WIFI_GROOM_HASH "abf77bfa1018789b96e58fc6b883024272a4129d"
#define WIFI_TOAST_HASH "ead0d6ae21e8b5792397753fa9e7d4a2772e7ce2"
#define WIFI_RAVE_HASH  "e380dd5149661a80f83da0d3411a03600e47a27c"


// Wifi settings
#define WIFI_CHANNEL         11
#define WIFI_SSID_PREFIX     "madroy_11-11_"
#define WIFI_BEACON_DURATION 1000
#define WIFI_BEACON_PAUSE    15000
#define WIFI_NO_RSSI         -1000

enum {
  WifiStateNone = 0,
  WifiStateListen,
  WifiStateSelf
};

enum {
	WifiBride = 0,
	WifiGroom,
	WifiToast,
	WifiRave,

  WifiMax = WifiRave
};

struct WifiInfo {
  int32_t         rssi;
  char*           ssidHash;
};


void Wifi_Init();
void Wifi_Loop();
void Wifi_Shutdown();

void Wifi_Switch(int state); 
bool Wifi_GetID(byte* id, int len); // use: byte id[Mode_ShowID_digitNum];
String Wifi_GetName();
String Wifi_GetHash(String text);

/*
bool Wifi_StartAP(String name);
bool Wifi_StartAPWithSecret(String name);
void Wifi_StopAP();
int32_t Wifi_GetRSSI(int kind);
*/
