#include <ArduinoOTA.h>

#include "mode-flash.h"
#include "wifi-scan.h"


static void Mode_Flash_ota_error(short id);


void Mode_Flash_Init() {
  Wifi_Switch(WifiStateSelf);

  ArduinoOTA.onError(Mode_Flash_ota_error);
  ArduinoOTA.begin();
  
  Serial.printf("Ready. To flash the badge, join WiFi network\n");

  LED_ChangePattern(LEDOff);

  LED_Flash(3);
}


void Mode_Flash_Loop() {
  ArduinoOTA.handle();
}


void Mode_Flash_Shutdown() {
  Wifi_Switch(WifiStateListen);

  LED_Flash(3);
}

void Mode_Flash_ota_error(short id) { 
  Serial.printf("ota error %d\n", id); 
}
