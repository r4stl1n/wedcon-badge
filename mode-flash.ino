#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <FastLED.h>

#define NO_GLOBAL_ARDUINOOTA
#include <ArduinoOTA.h>

#include "mode-flash.h"
#include "wifi-scan.h"


static ArduinoOTAClass* Mode_Flash_arduinoOTA = NULL;

static void ota_error(short id) { Serial.printf("ota error %d\n", id); }


void Mode_Flash_Init() {
  LED_ChangePattern(LEDOff);

  String name = Wifi_GetName();
  WiFi.softAP(name);
  delay(100);

  Mode_Flash_arduinoOTA = new(ArduinoOTAClass);
  Mode_Flash_arduinoOTA->onError(ota_error);
  Mode_Flash_arduinoOTA->begin();
  
  Serial.printf("Ready. To flash the badge, join WiFi network %s\n", name.c_str());

  delay(100);

  LED_Flash(3);
}


void Mode_Flash_Loop() {
  Mode_Flash_arduinoOTA->handle();
}


void Mode_Flash_Shutdown() {
  delete Mode_Flash_arduinoOTA;
  Mode_Flash_arduinoOTA = NULL;

  WiFi.softAPdisconnect(true);

  LED_Flash(3);
}
