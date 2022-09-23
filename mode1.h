#pragma once


#define MODE_1_WIFI_CHANNEL     11
#define MODE_1_WIFI_SSID_BRIDE  "wn01233"
//#define MODE_1_WIFI_SSID_BRIDE  "20221111_madisonroyal__bride"
#define MODE_1_WIFI_SSID_GROOM  "20221111_madisonroyal__groom"
#define MODE_1_WIFI_RSSI_LIMIT  -70

#define MODE_1_NUM_LEDS         6
#define MODE_1_DATA_PIN         4
#define MODE_1_HUE_BOTH         4             // red
#define MODE_1_HUE_BRIDE        230           // pink
#define MODE_1_HUE_GROOM        144           // blue
#define MODE_1_BRIGHTNESS_LOW   20
#define MODE_1_BRIGHTNESS_HIGH  (MODE_1_BRIGHTNESS_LOW + 64)


void Mode_1_Init();
void Mode_1_Loop();
void Mode_1_Shutdown();