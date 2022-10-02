#pragma once

// ESP-12E pin# of FLASH/PROGRAM/MODE button (left button)
#define BUTTON_PIN    0
#define BUTTON_HOLD   3000

enum Mode {
  ModeProximity = 0,
  ModeCustom,
  ModeOff,
  ModeShowID,
  ModeEditCustom,
  ModeFlash,

  ModeMax = ModeOff // cycle through to ModeOff (not ModeFlash) 
};


void Mode_Init();
void Mode_Loop();
void Mode_Shutdown();

short Mode_GetMode();
