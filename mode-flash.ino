#include <ESP8266WiFi.h>
#include <FastLED.h>
#include <ArduinoOTA.h>
#include "mode1.h"
#include "mode-flash.h"


static void Mode_Flash_alert();


void Mode_Flash_Init(String name) {
  Mode_Flash_alert();

  WiFi.mode(WIFI_AP);
  WiFi.softAP(name);

  ArduinoOTA.begin();
  
  Serial.printf("Ready. To flash the badge, join WiFi network %s\n", name.c_str());

  delay(100);
}


void Mode_Flash_Shutdown() {
  WiFi.disconnect();
}


void Mode_Flash_Loop() {
  ArduinoOTA.handle();
}


void Mode_Flash_alert() {
  static CRGB leds[MODE_1_NUM_LEDS];

  FastLED.addLeds<NEOPIXEL, MODE_1_DATA_PIN>(leds, MODE_1_NUM_LEDS);

  FastLED.showColor(CHSV(0, 0, 0));
  delay(300);
  FastLED.showColor(CHSV(0, 0, 255));
  delay(10);
  FastLED.showColor(CHSV(0, 0, 0));
  delay(300);
  FastLED.showColor(CHSV(0, 0, 255));
  delay(10);
  FastLED.showColor(CHSV(0, 0, 0));
  delay(300);
  FastLED.showColor(CHSV(0, 0, 255));
  delay(10);
  FastLED.showColor(CHSV(0, 0, 0));

  FastLED.showColor(CHSV(0, 0, 0));
  FastLED.clear();
}
