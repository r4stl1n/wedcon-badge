#include <SD.h>

#include "led.h"
#include "config.h"


static CRGB LED_leds[LED_NUM];

static struct ScriptLine* LED_Script;
static struct ScriptLine LED_ScriptBuffer[LED_MAX_SCRIPT_SIZE];
static unsigned long LED_scriptLineCount = 0;

static short LED_pattern = LEDOff;
static byte LED_hue = 0;
static uint LED_brightness = 0;

static unsigned long LED_introIndex = 0;
static unsigned long LED_lastStepAt = 0;
static unsigned long LED_targetDuration = 0;
static unsigned long LED_scriptLineIndex = 0;
static unsigned long LED_lastTickAt = 0;
static unsigned long LED_toastStartedAt = 0;
static short LED_previousPattern = 0;

static struct ScriptLine LED_toastScript[] = {
  {   20, { { 0, 0, 255 }, { 0, 0, 255 }, { 0, 0, 255 }, { 0, 0, 255 }, { 0, 0, 255 }, { 0, 0, 255 } } },
  { 1000, { { 0, 0, 255 }, { 0, 0, 255 }, { 0, 0, 255 }, { 0, 0, 255 }, { 0, 0, 255 }, { 0, 0, 255 } } }
};

static void LED_parseLine(String line);
static bool LED_nextStep();
static uint LED_getNewBrightness();
static void LED_runScript();
static void LED_updateLEDs(short scriptIndex);
static String LED_patternName(short pattern);


void LED_Init() {
  FastLED.addLeds<NEOPIXEL, LED_DATA_PIN>(LED_leds, LED_NUM);
  LED_Off();

  for (int n = 0; n < 6; n++ ) {
    LED_toastScript[1].hsl[n].hue = Config().led.toast.hue;
    LED_toastScript[1].hsl[n].saturation = Config().led.toast.saturation;
    LED_toastScript[1].hsl[n].luminence = Config().led.toast.luminance;
  }

  if (!SPIFFS.begin()) {
    Serial.println("An error has occurred while mounting SPIFFS");
    return;
  }

  SD.begin(D8);
}


void LED_Loop() {
  switch(LED_pattern) {
  case LEDOff:
    LED_Off();
    break;
    
  case LEDRainbow:
    FastLED.showColor(CHSV(LED_hue++, Config().led.none.saturation, Config().led.none.luminance));
    delay(20);
    break;
    
  case LEDBride:
    if (LED_nextStep()) {
      FastLED.showColor(CHSV(Config().led.bride.hue, Config().led.bride.saturation, LED_getNewBrightness()));
    }
    break;
    
  case LEDGroom:
    if (LED_nextStep()) {
      FastLED.showColor(CHSV(Config().led.groom.hue, Config().led.groom.saturation, LED_getNewBrightness()));
    }
    break;
    
  case LEDBoth:
    if (LED_nextStep()) {
      FastLED.showColor(CHSV(Config().led.both.hue, Config().led.both.saturation, LED_getNewBrightness()));
    }
    break;
    
  case LEDToast:
    if ((millis() - LED_toastStartedAt) > Config().led.toast.timeout) {
      LED_ChangePattern(LED_previousPattern);
    } else {
      LED_runScript();
    }
    break;

  case LEDRave:    
  case LEDScript:
    LED_runScript();
    break;
  };
}


void LED_Shutdown() {
  SD.end();

  SPIFFS.end();

  LED_ChangePattern(LEDOff);
}


struct ScriptLine* LED_GetScript(unsigned long& scriptLineCount) {
  scriptLineCount = LED_scriptLineCount;
  return LED_Script;
}


void LED_Off() {
  FastLED.showColor(CHSV(0, 0, 0));
}


void LED_Flash(int count, const CHSV color) {
  FastLED.showColor(CHSV(0, 0, 0));
  delay(300);

  for (int n = 0; n < count; n++) {  
    FastLED.showColor(color);
    delay(10);
    FastLED.showColor(CHSV(0, 0, 0));
    delay(300);
  }
}


void LED_ChangePattern(short pattern) {
  Serial.printf("new LED pattern: %d = %s\n", pattern, LED_patternName(pattern).c_str());
  
  FastLED.showColor(CHSV(0, 0, 0));
  
  switch (pattern) {
  case LEDToast:
    LED_toastStartedAt = millis();    
    LED_previousPattern = LED_pattern;
    if (!LED_LoadScript(LED_TOAST_SCRIPT_FILE_NAME)) {
      LED_SetScript(sizeof(LED_toastScript)/sizeof(struct ScriptLine), LED_toastScript);
    }
    break;
    
  case LEDRave:
    LED_LoadScript(LED_RAVE_SCRIPT_FILE_NAME);
    break;

  case LEDScript:
    if (!LED_LoadScript(LED_CUSTOM_SCRIPT_FILE_NAME)) {
      LED_LoadScript(LED_DEFAULT_SCRIPT_FILE_NAME);
    }
    break;
  }
  
  LED_pattern = pattern;

  LED_introIndex = 0;
  LED_lastStepAt = millis();
  LED_targetDuration = 0;
  LED_scriptLineIndex = 0;
}


void LED_SetScript(int lines, struct ScriptLine* script) {
  LED_scriptLineCount = lines;
  LED_Script = script;

  LED_pattern = LEDScript;

  LED_introIndex = 0;
  LED_lastStepAt = millis();
  LED_targetDuration = 0;
  LED_scriptLineIndex = 0;
}


bool LED_LoadScript(String name) {
  File scriptFile = SPIFFS.open(name, "r");
  if (!scriptFile) {
    Serial.printf("Script '%s' does not exist\n", name.c_str());
    return false;
  }

  ScriptLine scriptLine;
  int scriptLineCount = 0;
  String data = "";
  while (scriptFile.available() && scriptLineCount < LED_MAX_SCRIPT_SIZE) {
    char c = scriptFile.read();
    if (c != '\n') {
      data += c;
      continue;
    }

    if (LED_parseLine(data, scriptLine)) {
      Serial.printf("Processing line %d: %s\n", scriptLineCount + 1, data.c_str());

      LED_ScriptBuffer[scriptLineCount++] = scriptLine;
    }
    
    data = "";
  }

  if (LED_parseLine(data, scriptLine)) {    
    Serial.printf("Processing line %d: %s\n", scriptLineCount + 1, data.c_str());

    LED_ScriptBuffer[scriptLineCount++] = scriptLine;
  }
  
  scriptFile.close();

  Serial.printf("Script contains %d lines\n", scriptLineCount);

  LED_SetScript(scriptLineCount, LED_ScriptBuffer);

  return true;
}


bool LED_parseLine(String line, ScriptLine& scriptLine) {
    int match = sscanf(line.c_str(), "%hd %hhd %hhd %hhd %hhd %hhd %hhd %hhd %hhd %hhd %hhd %hhd %hhd %hhd %hhd %hhd %hhd %hhd %hhd", 
                       &scriptLine.duration,
                       &scriptLine.hsl[0].hue, &scriptLine.hsl[0].saturation, &scriptLine.hsl[0].luminence, 
                       &scriptLine.hsl[1].hue, &scriptLine.hsl[1].saturation, &scriptLine.hsl[1].luminence, 
                       &scriptLine.hsl[2].hue, &scriptLine.hsl[2].saturation, &scriptLine.hsl[2].luminence, 
                       &scriptLine.hsl[3].hue, &scriptLine.hsl[3].saturation, &scriptLine.hsl[3].luminence, 
                       &scriptLine.hsl[4].hue, &scriptLine.hsl[4].saturation, &scriptLine.hsl[4].luminence, 
                       &scriptLine.hsl[5].hue, &scriptLine.hsl[5].saturation, &scriptLine.hsl[5].luminence);

    return match == 19;
}


bool LED_nextStep() {
  if (LED_introIndex < LED_INTRO_COUNT) {
    LED_introIndex++;
    return true;
  }

  bool go = (millis() - LED_lastTickAt) >= LED_DELAY;
  if (go) {
    LED_lastTickAt = millis();
  }
  
  return go;
}


uint LED_getNewBrightness() {
  const uint range = Config().led.luminance.high - Config().led.luminance.low;
  LED_brightness = (LED_brightness + 1) % (range * 2);
  return (LED_brightness > range ? 2 * range - LED_brightness : LED_brightness) + Config().led.luminance.low;
}


void LED_runScript() {
  if ((LED_scriptLineCount <= 0) || (LED_Script == NULL)) {
    delay(10);
    return;
  }

  if ((LED_targetDuration > 0) && ((millis() - LED_lastStepAt) < LED_targetDuration)) {
    return;
  }

//  Serial.printf("showing line %d for %dms\n", LED_scriptLineIndex + 1, LED_Script[LED_scriptLineIndex].duration);
/*
  Serial.printf(" %3d: %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n", 
                LED_scriptLineIndex + 1, LED_Script[LED_scriptLineIndex].duration,
                LED_Script[LED_scriptLineIndex].hsl[0].hue, LED_Script[LED_scriptLineIndex].hsl[0].saturation, LED_Script[LED_scriptLineIndex].hsl[0].luminence,
                LED_Script[LED_scriptLineIndex].hsl[1].hue, LED_Script[LED_scriptLineIndex].hsl[1].saturation, LED_Script[LED_scriptLineIndex].hsl[1].luminence,
                LED_Script[LED_scriptLineIndex].hsl[2].hue, LED_Script[LED_scriptLineIndex].hsl[2].saturation, LED_Script[LED_scriptLineIndex].hsl[2].luminence,
                LED_Script[LED_scriptLineIndex].hsl[3].hue, LED_Script[LED_scriptLineIndex].hsl[3].saturation, LED_Script[LED_scriptLineIndex].hsl[3].luminence,
                LED_Script[LED_scriptLineIndex].hsl[4].hue, LED_Script[LED_scriptLineIndex].hsl[4].saturation, LED_Script[LED_scriptLineIndex].hsl[4].luminence,
                LED_Script[LED_scriptLineIndex].hsl[5].hue, LED_Script[LED_scriptLineIndex].hsl[5].saturation, LED_Script[LED_scriptLineIndex].hsl[5].luminence
              );
*/

  LED_lastStepAt = millis();
  LED_targetDuration = LED_Script[LED_scriptLineIndex].duration;
  
  LED_updateLEDs(LED_scriptLineIndex);
  
  LED_scriptLineIndex = (LED_scriptLineIndex + 1) % LED_scriptLineCount;
}


void LED_updateLEDs(short scriptIndex) {
  LED_leds[0] = CHSV(LED_Script[scriptIndex].hsl[0].hue, LED_Script[scriptIndex].hsl[0].saturation, LED_Script[scriptIndex].hsl[0].luminence);
  LED_leds[1] = CHSV(LED_Script[scriptIndex].hsl[1].hue, LED_Script[scriptIndex].hsl[1].saturation, LED_Script[scriptIndex].hsl[1].luminence);
  LED_leds[2] = CHSV(LED_Script[scriptIndex].hsl[2].hue, LED_Script[scriptIndex].hsl[2].saturation, LED_Script[scriptIndex].hsl[2].luminence);
  LED_leds[3] = CHSV(LED_Script[scriptIndex].hsl[3].hue, LED_Script[scriptIndex].hsl[3].saturation, LED_Script[scriptIndex].hsl[3].luminence);
  LED_leds[4] = CHSV(LED_Script[scriptIndex].hsl[4].hue, LED_Script[scriptIndex].hsl[4].saturation, LED_Script[scriptIndex].hsl[4].luminence);
  LED_leds[5] = CHSV(LED_Script[scriptIndex].hsl[5].hue, LED_Script[scriptIndex].hsl[5].saturation, LED_Script[scriptIndex].hsl[5].luminence);
  
  FastLED.show();
}


String LED_patternName(short pattern) {
  switch (pattern) {
    case LEDOff:
      return "off";
        
    case LEDRainbow:
      return "rainbow";
        
    case LEDBride:
      return "bride";
        
    case LEDGroom:
      return "groom";
        
    case LEDBoth:
      return "both";
        
    case LEDToast:
      return "toast";
        
    case LEDRave:
      return "rave";
        
    case LEDScript:
      return "script";
        
    default:
      return String(pattern);
  }
}
