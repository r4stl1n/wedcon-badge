#pragma once


#define MODE_2_NUM_LEDS         6
#define MODE_2_DATA_PIN         4
#define MODE_2_BRIGHTNESS_LOW   20
#define MODE_2_BRIGHTNESS_HIGH  (MODE_1_BRIGHTNESS_LOW + 64)
#define MODE_2_MAX_SCRIPT_SIZE  1000
#define MODE_2_SCRIPT_FILE_NAME "/mode2-script.txt"


struct ScriptLine {
  short duration;
  byte leds[MODE_2_NUM_LEDS * 3];
};


void Mode_2_Init(String name, bool safe = false);
void Mode_2_Loop();
void Mode_2_Shutdown();
