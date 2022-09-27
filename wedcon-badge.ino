#include <Bounce2.h>
#include <FastLED.h>
#include "mode1.h"
#include "mode2.h"
#include "mode-flash.h"


// ESP-12E pin# of FLASH/PROGRAM/MODE button (left button)
#define BUTTON_PIN 	0

enum Modes {
  ModeProximity = 0,
  ModeOff,
  ModeCustom,
  ModeExpert,
  ModeFlash,

  ModeMax = ModeExpert
};

static Bounce button = Bounce();
static uint mode = 0;

static void blink(uint num);
static String getName();


void setup() {
  Serial.begin(115200);
  Serial.printf("\nMy name is %s\n", (const char*)getName().c_str());

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  button.attach(BUTTON_PIN, INPUT_PULLUP);
  button.interval(25);

  Serial.printf("Now in mode: %d\n", mode + 1);
  Mode_1_Init(getName());
}


void loop() {
  button.update();

  uint oldMode = mode;
	if (mode != ModeFlash && button.fell()) {
    mode = (mode + 1) % (ModeMax + 1);
    blink(mode);

    Serial.printf("Now in mode: %d\n", mode + 1);
  }

  if (mode != ModeFlash && !button.read() && button.currentDuration() >= 5000) {
    mode = ModeFlash;

    Serial.printf("*** entering flash mode ***\n");
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

      case ModeFlash:
        Mode_Flash_Shutdown();
        break;
    }

    switch (mode) {
      case ModeProximity:
        Mode_1_Init(getName());
        break;

      case ModeOff:
        // nothing to do here
        break;

      case ModeCustom:
        Mode_2_Init(getName());
        break;

      case ModeExpert:
        // todo: call your code here
        break;

      case ModeFlash:
        Mode_Flash_Init(getName());
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

    case ModeFlash:
      Mode_Flash_Loop();
      break;
  }
}


void blink(uint num) {
  digitalWrite(LED_BUILTIN, LOW);
  delay(5);
  digitalWrite(LED_BUILTIN, HIGH);

  for (uint n = 0; n < num; n++) {
    delay(150);
    digitalWrite(LED_BUILTIN, LOW);
    delay(5);
    digitalWrite(LED_BUILTIN, HIGH);
  }
}


String getName() {
    char tmp[20];
    sprintf(tmp, MODE_1_WIFI_SSID_PREFIX "%06x", ESP.getChipId());
    return String(tmp);
}
