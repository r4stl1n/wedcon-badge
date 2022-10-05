#include <Bounce2.h>

#include "mode.h"


static Bounce Mode_button = Bounce();
static short Mode_mode = ModeProximity;
static bool Mode_skipRise = false;

static void Mode_blink(uint num);


void Mode_Init() {
  Mode_button.attach(BUTTON_PIN, INPUT_PULLUP);
  Mode_button.interval(25);
  
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
}


void Mode_Loop() {
  Mode_button.update();
  
  if (!Mode_button.read() && Mode_button.currentDuration() >= Config().hold.timeout) {
    if (!Mode_skipRise) {
      switch (Mode_mode) {
        case ModeProximity:
          Mode_mode = ModeShowID;
          break;
          
        case ModeCustom:
          Mode_mode = ModeEditCustom;
          break;
          
        case ModeOff:
          Mode_mode = ModeFlash;
          break;

        case ModeShowID:
          Mode_mode = ModeProximity;
          break;
          
        case ModeEditCustom:
          Mode_mode = ModeCustom;
          break;
          
        case ModeFlash:
          Mode_mode = ModeOff;
          break;
      }

      Mode_skipRise = true;
    }
  } else if (Mode_button.rose()) {
    if (!Mode_skipRise) {
      Mode_mode = (Mode_mode + 1) % (ModeMax + 1);

      Mode_blink(Mode_mode);
    }

    Mode_skipRise = false;
  }
}


void Mode_Shutdown() {
}


short Mode_GetMode() {
  return Mode_mode;
}


void Mode_blink(uint num) {
  digitalWrite(LED_BUILTIN, LOW);
  delay(5);
  digitalWrite(LED_BUILTIN, HIGH);

  for (uint n = 0; n < num; n++) {
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
    delay(5);
    digitalWrite(LED_BUILTIN, HIGH);
  }
}
