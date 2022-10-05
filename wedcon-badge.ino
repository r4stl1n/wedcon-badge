#include "led.h"
#include "config.h"
#include "mode.h"
#include "wifi-scan.h"

#include "mode-proximity.h"
#include "mode-custom.h"
#include "mode-off.h"
#include "mode-show-id.h"
#include "mode-edit-custom.h"
#include "mode-flash.h"


void setup() {
  Serial.begin(115200);
  Serial.printf("\nSSID: %s\n", Wifi_GetName().c_str());

  LED_Init();
  Mode_Init();
  Wifi_Init();

  Config_Load();

  Mode_Proximity_Init();
}


void loop() {
  LED_Loop();
  Wifi_Loop();

  short oldMode = Mode_GetMode();
  Mode_Loop();
  short newMode = Mode_GetMode();

  if (oldMode != newMode) {
    Serial.printf("mode %d => %d\n", oldMode, newMode);
    
    switch (oldMode) {
      case ModeProximity:
        if (newMode != ModeShowID) {
          Serial.printf("shutting down proximity mode\n");
          Mode_Proximity_Shutdown();
        }
        break;

      case ModeCustom:
        if (newMode != ModeEditCustom) {
          Serial.printf("shutting down custom mode\n");
          Mode_Custom_Shutdown();
        }
        break;
        
      case ModeOff:
        if (newMode != ModeFlash) {
          Serial.printf("shutting down off mode\n");
          Mode_Off_Shutdown();
        }
        break;
        
      case ModeShowID:
        Serial.printf("shutting down show id mode\n");
        Mode_ShowID_Shutdown();
        break;
        
      case ModeEditCustom:
        Serial.printf("shutting down edit custom mode\n");
        Mode_Edit_Custom_Shutdown();
        break;
        
      case ModeFlash:
        Serial.printf("shutting down flash mode\n");
        Mode_Flash_Shutdown();
        break;

      default:
        Serial.printf("shutting down unknown mode %d\n", newMode);
        Mode_Flash_Init();
        break;
    }

    switch (newMode) {
      case ModeProximity:
        Serial.printf("launching proximity mode\n");
        Mode_Proximity_Init();
        break;

      case ModeCustom:
        Serial.printf("launching custom mode\n");
        Mode_Custom_Init();
        break;

      case ModeOff:
        Serial.printf("launching off mode\n");
        Mode_Off_Init();
        break;

      case ModeShowID:
        Serial.printf("launching show id mode\n");
        Mode_ShowID_Init();
        break;
        
      case ModeEditCustom:
        Serial.printf("launching edit custom mode\n");
        Mode_Edit_Custom_Init();
        break;
        
      case ModeFlash:
        Serial.printf("launching flash mode\n");
        Mode_Flash_Init();
        break;

      default:
        Serial.printf("launching unknown mode %d\n", newMode);
        Mode_Flash_Init();
        break;
    }
  }

  switch (newMode) {
    case ModeProximity:
      Mode_Proximity_Loop();
      break;

    case ModeCustom:
      Mode_Custom_Loop();
      break;

    case ModeOff:
      Mode_Off_Loop();
      break;

    case ModeShowID:
      Mode_ShowID_Loop();
      break;
      
    case ModeEditCustom:
      Mode_Edit_Custom_Loop();
      break;
      
    case ModeFlash:
      Mode_Flash_Loop();
      break;

    default:
      Serial.printf("running unknown mode %d\n", newMode);
      break;
  }
}
