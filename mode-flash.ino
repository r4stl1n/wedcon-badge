#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <FastLED.h>
#include <ArduinoOTA.h>

#include "mode-flash.h"
#include "wifi-scan.h"


static void ota_error(short id) { Serial.printf("ota error %d\n", id); }


void Mode_Flash_Init() {
  LED_ChangePattern(LEDOff);

  if (!Wifi_StartAP(Wifi_GetName())) {
    return;
  }

  ArduinoOTA.onError(ota_error);
  ArduinoOTA.begin();
  
  Serial.printf("Ready. To flash the badge, join WiFi network\n");

  LED_Flash(3);
}


void Mode_Flash_Loop() {
  ArduinoOTA.handle();
}


void Mode_Flash_Shutdown() {
  Wifi_StopAP();

  LED_Flash(3);
}
