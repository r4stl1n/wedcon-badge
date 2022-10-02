#pragma once

#include <FastLED.h>


// "breathing" brightness for whenever any of the wedding party is detected
#define LED_BRIGHTNESS_LOW            100
#define LED_BRIGHTNESS_HIGH           (LED_BRIGHTNESS_LOW + 100)

// bride AND groom detected
#define LED_HUE_BOTH                  250           // red
#define LED_SATURATION_BOTH           240           

// bride detected
#define LED_HUE_BRIDE                 150           // blue
#define LED_SATURATION_BRIDE          200           

// groom detected
#define LED_HUE_GROOM                 210           // purple
#define LED_SATURATION_GROOM          255           

// none = no wedding party detected
#define LED_BRIGHTNESS_NONE           50            // cycling rainbow
#define LED_SATURATION_NONE           150           

// toast
#define LED_HUE_TOAST                 28           // yellow
#define LED_SATURATION_TOAST          255            

// intro settings
#define LED_INTRO_CYCLES              5
#define LED_INTRO_COUNT               (LED_INTRO_CYCLES * (LED_BRIGHTNESS_HIGH - LED_BRIGHTNESS_LOW) * 2)
#define LED_DELAY                     20

// LED config
#define LED_NUM                       6
#define LED_DATA_PIN                  4

#define LED_MAX_SCRIPT_SIZE           1000

#define LED_TOAST_SCRIPT_FILE_NAME    "/toast-script.txt"
#define LED_RAVE_SCRIPT_FILE_NAME     "/rave-script.txt"
#define LED_CUSTOM_SCRIPT_FILE_NAME   "/custom-script.txt"
#define LED_DEFAULT_SCRIPT_FILE_NAME  "/default-script.txt"

#define LED_TOAST_TIMEOUT             30000


struct ScriptLine {
  short duration;
  struct HSL {
    byte hue;
    byte saturation;
    byte luminence;
  } hsl[LED_NUM];
};


enum LEDPattern {
  LEDOff = 0,
  LEDRainbow,
  LEDBride,
  LEDGroom,
  LEDBoth,
  LEDToast,
  LEDRave,
  LEDScript,

  LEDMax = LEDScript
};


void LED_Init();
void LED_Loop();
void LED_Shutdown();

void LED_Flash(int count, const CHSV color = CHSV(0, 0, 150));
void LED_Off();
void LED_ChangePattern(short pattern);
void LED_SetScript(int lines, struct ScriptLine* script);
bool LED_LoadScript(String name);
