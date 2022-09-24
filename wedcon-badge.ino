#include <Bounce2.h>
#include <FastLED.h>
#include "mode1.h"


// ESP-12E pin# of FLASH/PROGRAM/MODE button (left button)
#define BUTTON_PIN 	0

enum Modes {
  ModeProximity = 0,
  ModeOff,
  ModeCustom,
  ModeExpert,
  ModeTest1,
  ModeTest2,
  ModeTest3,
  ModeTest4,

  ModeMax = ModeExpert // ModeTest4
};

static Bounce button = Bounce();
static uint mode = 0;

static CRGB Mode_Test_leds[MODE_1_NUM_LEDS];
static uint Mode_Test_brightness = 0;
static uint Mode_Test_hue = 0;

static void blink(uint num);
static uint Mode_Test_getNewBrightness();


void setup() {
  Serial.begin(115200);
  Serial.println("");
  Serial.printf("mode: %d\n", mode + 1);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

	button.attach(BUTTON_PIN, INPUT_PULLUP);
	button.interval(25);

  Mode_1_Init();
}


void loop() {
	button.update();

  uint oldMode = mode;
	if (button.fell()) {
    mode = (mode + 1) % (ModeMax + 1);
    blink(mode);

    Serial.printf("mode: %d\n", mode + 1);
  }

  if (oldMode != mode) {
    switch (oldMode) {
    case ModeProximity:
      Mode_1_Shutdown();
      break;

    case ModeOff:
      // nothing to do here
      break;
      
    case ModeCustom:
      Mode_2_Shutdown();
      break;
      
    case ModeExpert:
      // todo: call your code here
      break;

    case ModeTest1:
      FastLED.showColor(CHSV(0, 0, 0)); 
      FastLED.clear();
      break;

    case ModeTest2:
      FastLED.showColor(CHSV(0, 0, 0)); 
      FastLED.clear();
      break;

    case ModeTest3:
      FastLED.showColor(CHSV(0, 0, 0)); 
      FastLED.clear();
      break;

    case ModeTest4:
      FastLED.showColor(CHSV(0, 0, 0)); 
      FastLED.clear();
      break;
    }

    switch (mode) {
    case ModeProximity:
      Mode_1_Init();
      break;

    case ModeOff:
      // nothing to do here
      break;
      
    case ModeCustom:
      Mode_2_Init();
      break;
      
    case ModeExpert:
      // todo: call your code here
      break;

    case ModeTest1:
      FastLED.addLeds<NEOPIXEL, MODE_1_DATA_PIN>(Mode_Test_leds, MODE_1_NUM_LEDS);
      break;

    case ModeTest2:
      FastLED.addLeds<NEOPIXEL, MODE_1_DATA_PIN>(Mode_Test_leds, MODE_1_NUM_LEDS); 
      break;

    case ModeTest3:
      FastLED.addLeds<NEOPIXEL, MODE_1_DATA_PIN>(Mode_Test_leds, MODE_1_NUM_LEDS);
      break;

    case ModeTest4:
      FastLED.addLeds<NEOPIXEL, MODE_1_DATA_PIN>(Mode_Test_leds, MODE_1_NUM_LEDS);
      break;
    }
  }

  delay(1);
  switch (mode) {
    case ModeProximity:
      Mode_1_Loop();
      break;

    case ModeOff:
      // nothing to do here
      break;
      
    case ModeCustom:
      Mode_2_Loop();
      break;
      
    case ModeExpert:
      // todo: call your code here
      break;

    case ModeTest1:
      FastLED.showColor(CHSV(MODE_1_HUE_BOTH, MODE_1_SAT_BOTH, Mode_Test_getNewBrightness()));  
      break;

    case ModeTest2:
      FastLED.showColor(CHSV(MODE_1_HUE_BRIDE, MODE_1_SAT_BRIDE, Mode_Test_getNewBrightness())); 
      break;

    case ModeTest3:
      FastLED.showColor(CHSV(MODE_1_HUE_GROOM, MODE_1_SAT_GROOM, Mode_Test_getNewBrightness())); 
      break;

    case ModeTest4:
      FastLED.showColor(CHSV(Mode_Test_hue++, MODE_1_SAT_NONE, MODE_1_BRIGHTNESS_NONE)); 
      delay(20);
      break;
  }
}


void blink(uint num) {
  digitalWrite(LED_BUILTIN, LOW);
  delay(50);
  digitalWrite(LED_BUILTIN, HIGH);

  for (uint n = 0; n < num; n++) {
    delay(50);
    digitalWrite(LED_BUILTIN, LOW);
    delay(50);
    digitalWrite(LED_BUILTIN, HIGH);
  }
}


uint Mode_Test_getNewBrightness() {
  const uint range = MODE_1_BRIGHTNESS_HIGH - MODE_1_BRIGHTNESS_LOW;
  Mode_Test_brightness = (Mode_Test_brightness + 1) % (range * 2);
  return (Mode_Test_brightness > range ? 2 * range - Mode_Test_brightness : Mode_Test_brightness) + MODE_1_BRIGHTNESS_LOW;
}
