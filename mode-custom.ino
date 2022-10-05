#include "mode-custom.h"
#include "led.h"


void Mode_Custom_Init() {
  LED_Off();
  LED_ChangePattern(LEDScript);
}


void Mode_Custom_Loop() {
}


void Mode_Custom_Restore() {
  LED_Off();
  LED_ChangePattern(LEDScript);
}


void Mode_Custom_Shutdown() {
}
