#include "mode-show-id.h"


const struct ScriptLine LED_digitScript[] = {
  { 500, { { 0, 0, 120 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } } },
  { 500, { { 0, 0, 0 }, { 0, 0, 120 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } } },
  { 500, { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 120 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } } },
  { 500, { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 120 }, { 0, 0, 0 }, { 0, 0, 0 } } },
  { 500, { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 120 }, { 0, 0, 0 } } },
  { 500, { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 120 } } }
};

static struct ScriptLine LED_idScript[] = {
  { 1000, { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } } },
  {  500, { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } } },
  { 1000, { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } } },
  {  500, { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } } },
  { 1000, { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } } },
  {  500, { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } } },
  { 1000, { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } } },
  {  500, { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } } },
  { 1000, { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } } },
  {  500, { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } } },
  { 1000, { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } } },
  {  500, { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } } },
  { 1000, { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } } },
  {  500, { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } } },
  { 1000, { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } } },
  {  500, { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } } },
  { 1000, { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } } },
  {  500, { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } } },
  { 1000, { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } } },
  {  500, { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } } },
  { 3000, { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } } }
};

static void Mode_ShowID_setID(int id);

static struct ScriptLine* Mode_ShowID_previousScript = NULL;
static unsigned long Mode_ShowID_previousScriptLineCount = 0;


void Mode_ShowID_Init() {
  Mode_ShowID_previousScript = LED_GetScript(Mode_ShowID_previousScriptLineCount);
  LED_ChangePattern(LEDOff);

  // TODO
  Mode_ShowID_setID(0, 3);
  Mode_ShowID_setID(1, 1);
  Mode_ShowID_setID(2, 1);
  Mode_ShowID_setID(3, 0);
  Mode_ShowID_setID(4, 5);
  Mode_ShowID_setID(5, 4);
  Mode_ShowID_setID(6, 4);
  Mode_ShowID_setID(7, 4);
  Mode_ShowID_setID(8, 5);
  Mode_ShowID_setID(9, 0);
  // TODO

  LED_Flash(1);

  LED_SetScript(sizeof(LED_idScript)/sizeof(struct ScriptLine), LED_idScript);
}


void Mode_ShowID_Loop() {
}


void Mode_ShowID_Restore() {
  LED_SetScript(sizeof(LED_idScript)/sizeof(struct ScriptLine), LED_idScript);
}


void Mode_ShowID_Shutdown() {
  LED_SetScript(Mode_ShowID_previousScriptLineCount, Mode_ShowID_previousScript);
  LED_Flash(1);
}


void Mode_ShowID_setID(int id, int digit) {
  if (id < 0 || id > (sizeof(LED_idScript)/sizeof(struct ScriptLine)/2)) {
    return;
  }

  if (digit < 0 || digit > (sizeof(LED_digitScript)/sizeof(struct ScriptLine))) {
    return;
  }

  LED_idScript[id * 2 + 1] = LED_digitScript[digit];
}
