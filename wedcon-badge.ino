#include <Bounce2.h>
#include <FS.h>
#include "mode1.h"


#define BUTTON_PIN 	0

enum Modes {
  ModeProximity = 0,
  ModeOff,
  ModeCustom,
  ModeExpert,

  ModeMax = ModeExpert
};

static Bounce button = Bounce();
static uint mode = 0;


void setup() {
  Serial.begin(115200);
  Serial.println("");
  Serial.printf("mode: %d\n", mode + 1);

  //Initialize File System
  SPIFFS.begin();
  Serial.println("File System Initialized");

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

	button.attach(BUTTON_PIN, INPUT_PULLUP);
	button.interval(25);

  Mode_1_Init();
}

void blink(uint num) {
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
  digitalWrite(LED_BUILTIN, HIGH);

  for (uint n = 0; n < num; n++) {
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
  }
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
    }
  }

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
  }
}
