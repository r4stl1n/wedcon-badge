#pragma once


// Wifi settings
#define MODE_1_WIFI_CHANNEL         11
#define MODE_1_WIFI_SSID_BRIDE      "20221111_madisonroyal__bride"
#define MODE_1_WIFI_SSID_GROOM      "20221111_madisonroyal__groom"
#define MODE_1_WIFI_RSSI_HIGH       -50
#define MODE_1_WIFI_RSSI_LOW        -60
#define MODE_1_PROXIMITY_TIMEOUT    1000


// count and ESP pin# for LEDs
#define MODE_1_NUM_LEDS         6
#define MODE_1_DATA_PIN         4

// "breathing" brightness for whenever any of the wedding party is detected
#define MODE_1_BRIGHTNESS_LOW   100
#define MODE_1_BRIGHTNESS_HIGH  (MODE_1_BRIGHTNESS_LOW + 100)

// bride AND groom detected
#define MODE_1_HUE_BOTH         0             // white (= red without saturation)
#define MODE_1_SAT_BOTH         0             // no saturation

// bride detected
#define MODE_1_HUE_BRIDE        230           // pink
#define MODE_1_SAT_BRIDE        150           // medium saturation

// groom detected
#define MODE_1_HUE_GROOM        144           // blue
#define MODE_1_SAT_GROOM        150           // medium saturation

// none = no wedding party detected
#define MODE_1_BRIGHTNESS_NONE  50            
#define MODE_1_SAT_NONE         150           // medium saturation (color=cycling rainbow)

#define MODE_1_ALERT_CYCLES     5
#define MODE_1_ALERT_COUNT      (MODE_1_ALERT_CYCLES * (MODE_1_BRIGHTNESS_HIGH - MODE_1_BRIGHTNESS_LOW) * 2)


void Mode_1_Init();
void Mode_1_Loop();
void Mode_1_Shutdown();