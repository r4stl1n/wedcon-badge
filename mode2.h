#pragma once


#define MODE_2_WIFI_CHANNEL     11
#define MODE_2_WIFI_SSID        "wn01233-bc"

#define MODE_2_NUM_LEDS         6
#define MODE_2_DATA_PIN         4
#define MODE_2_BRIGHTNESS_LOW   20
#define MODE_2_BRIGHTNESS_HIGH  (MODE_1_BRIGHTNESS_LOW + 64)
#define MODE_2_MAX_SCRIPT_SIZE  128
#define MODE_2_SCRIPT_FILE_NAME "/mode2-script.txt"


struct ScriptLine {
  short duration;
  byte leds[17];
};


void Mode_2_Init(String name);
void Mode_2_Loop();
void Mode_2_Shutdown();
