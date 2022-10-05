#include <ESP8266WiFi.h>
#include <FastLED.h>

#include "mode-proximity.h"
#include "config.h"
#include "wifi-scan.h"

static unsigned long Mode_Proximity_lastSeen[WifiMax + 1];
static short Mode_lastPattern = LEDOff;

static unsigned long Mode_Proximity_evalLastSeen(unsigned long lastSeen, int32_t rssi);


void Mode_Proximity_Init() {
  for (int n = 0; n <= WifiMax; n++) {
    Mode_Proximity_lastSeen[n] = 0;
  }

  LED_ChangePattern(Mode_lastPattern);
}


void Mode_Proximity_Loop() {
  for (int n = 0; n <= WifiMax; n++) {
    Mode_Proximity_lastSeen[n] = Mode_Proximity_evalLastSeen(Mode_Proximity_lastSeen[n], Wifi_GetRSSI(n));
  }

  short lastPattern = Mode_lastPattern;
  
  if ((Mode_Proximity_lastSeen[WifiBride] > 0) && (Mode_Proximity_lastSeen[WifiGroom] > 0)) {
    if (lastPattern != LEDBoth) {
      Serial.printf("both: bride=%d groom=%d\n", Mode_Proximity_lastSeen[WifiBride], Mode_Proximity_lastSeen[WifiGroom]);
    }
    
    Mode_lastPattern = LEDBoth;
  } else if (Mode_Proximity_lastSeen[WifiBride] > 0) {
    if (lastPattern != LEDBride) {
      Serial.printf("bride: bride=%d groom=%d\n", Mode_Proximity_lastSeen[WifiBride], Mode_Proximity_lastSeen[WifiGroom]);
    }
    
    Mode_lastPattern = LEDBride;
  } else if (Mode_Proximity_lastSeen[WifiGroom] > 0) {
    if (lastPattern != LEDGroom) {
      Serial.printf("groom: bride=%d groom=%d\n", Mode_Proximity_lastSeen[WifiBride], Mode_Proximity_lastSeen[WifiGroom]);
    }
    
    Mode_lastPattern = LEDGroom;
  } else {
    if (lastPattern != LEDRainbow) {
      Serial.printf("none: bride=%d groom=%d\n", Mode_Proximity_lastSeen[WifiBride], Mode_Proximity_lastSeen[WifiGroom]);
    }
    
    Mode_lastPattern = LEDRainbow;
  }

  if (lastPattern != Mode_lastPattern) {
    LED_ChangePattern(Mode_lastPattern);
  }
}


void Mode_Proximity_Restore() {
  LED_ChangePattern(Mode_lastPattern);
}


void Mode_Proximity_Shutdown() {
}


unsigned long Mode_Proximity_evalLastSeen(unsigned long lastSeen, int32_t rssi) {
  if (rssi < 0 && rssi >= Config().proximity.rssi.high) {
    return millis();
  }

  if (lastSeen == 0) {
    return 0;
  }

  if ((millis() - lastSeen) > Config().proximity.timeout) {
    return 0;
  }

  if (rssi < 0 && rssi >= Config().proximity.rssi.low) {
    return millis();
  }

  return lastSeen;
}
