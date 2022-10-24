#include "mode-show-id.h"
#include "led.h"


// 2^32 - 1 = 4,294,967,295 base 10 = 1,550,104,015,503 base 6 (13 digits)
#define Mode_ShowID_digitNum      13
#define Mode_ShowID_scriptLength  (Mode_ShowID_digitNum * 2 + 1)
#define Mode_ShowID_on            {0, 0, 120}
#define Mode_ShowID_off           {0, 0, 0}
#define Mode_ShowID_onDuration    500
#define Mode_ShowID_offDuration   1000
#define Mode_ShowID_pause         3000


static struct ScriptLine Mode_ShowID_script[Mode_ShowID_scriptLength];
static struct ScriptLine* Mode_ShowID_previousScript = NULL;
static unsigned long Mode_ShowID_previousScriptLineCount = 0;


void Mode_ShowID_Init() {
  Mode_ShowID_previousScript = LED_GetScript(Mode_ShowID_previousScriptLineCount);
  
  LED_ChangePattern(LEDOff);

  // initiualize script
  for (int n = 0; n < Mode_ShowID_scriptLength; n++) {
    Mode_ShowID_script[n] = { Mode_ShowID_offDuration, { Mode_ShowID_off, Mode_ShowID_off, Mode_ShowID_off, Mode_ShowID_off, Mode_ShowID_off, Mode_ShowID_off } };
  }

  for (int n = 0; n < Mode_ShowID_digitNum; n++) {
    Mode_ShowID_script[2 * n + 1].duration = Mode_ShowID_onDuration;
  }

  Mode_ShowID_script[Mode_ShowID_scriptLength - 1].duration = Mode_ShowID_pause;

  // convert ID to script
  byte id[Mode_ShowID_digitNum];
  Wifi_GetID(id, Mode_ShowID_digitNum);
  
  for (int n = 0; n < Mode_ShowID_digitNum; n++) {
    Mode_ShowID_script[2 * n + 1].hsl[id[n]] = Mode_ShowID_on;
  }

  LED_Flash(1);

  LED_SetScript(Mode_ShowID_scriptLength, Mode_ShowID_script);
}


void Mode_ShowID_Loop() {
}


void Mode_ShowID_Restore() {
  LED_SetScript(Mode_ShowID_scriptLength, Mode_ShowID_script);
}


void Mode_ShowID_Shutdown() {
  LED_SetScript(Mode_ShowID_previousScriptLineCount, Mode_ShowID_previousScript);
  LED_Flash(1);
}
