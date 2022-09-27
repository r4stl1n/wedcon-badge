#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <elapsedMillis.h>
#include <FastLED.h>
#include <SD.h>
#include "mode2.h"


static CRGB Mode_2_leds[MODE_2_NUM_LEDS];
static ESP8266WebServer Mode_2_webserver(80);
static struct ScriptLine Mode_2_Script[MODE_2_MAX_SCRIPT_SIZE];
static short Mode_2_scriptLineCount = 0;
static short Mode_2_scriptLineIndex = 0;
static short Mode_2_targetDuration = 0;
static elapsedMillis Mode_2_currentActionTimer = 0;
static bool Mode_2_updateEnabled = false;
static String Mode_2_SSID;

static void Mode_2_runScript();
static void Mode_2_updateLEDs(short scriptIndex);
static void Mode_2_parseScript();
static void Mode_2_handleOnIndex();
static void Mode_2_handleOnSave();
static void Mode_2_handleNotFound();
static bool Mode_2_loadFromSpiffs(String path);


void Mode_2_Init(String name) {
  Mode_2_SSID = name;
  Mode_2_updateEnabled = false;
  
  FastLED.addLeds<NEOPIXEL, MODE_2_DATA_PIN>(Mode_2_leds, MODE_2_NUM_LEDS);
  FastLED.showColor(CHSV(0, 0, 0));

  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  SD.begin(D8);

  Mode_2_parseScript();
}


void Mode_2_Shutdown() {
  Mode_2_webserver.close();

  SD.end();

  SPIFFS.end();

  WiFi.disconnect();

  FastLED.showColor(CHSV(0, 0, 0));
  FastLED.clear();
}


void Mode_2_Loop() {
  Mode_2_webserver.handleClient();
  Mode_2_runScript();
}


void Mode_2_EnableUpdate() {
  if(Mode_2_updateEnabled) {
    return;
  }
  
  Mode_2_updateEnabled = true;
  
  // Setup the wifi access point
  // Server ip should be 192.168.4.1
//  Serial.printf("Setting up wifi network: %s - %s\n", Mode_2_SSID.c_str(), WiFi.softAP(Mode_2_SSID) ? "Ready" : "Failed!");
  bool ok = WiFi.softAP(Mode_2_SSID);
  Serial.printf(">>>>>>>>>>> new code here \n");
  Serial.printf("Setting up wifi network: %s - %s\n", Mode_2_SSID.c_str(), ok ? "Ready" : "Failed!");
  Serial.printf("IP address: %d.%d.%d.%d\n", WiFi.softAPIP()[0], WiFi.softAPIP()[1], WiFi.softAPIP()[2], WiFi.softAPIP()[3]);
  
  Mode_2_webserver.begin();
  Mode_2_webserver.on("/", HTTP_GET, Mode_2_handleOnIndex);
  Mode_2_webserver.on("/save", HTTP_POST, Mode_2_handleOnSave);
  Mode_2_webserver.onNotFound(Mode_2_handleNotFound);
}


void Mode_2_runScript() {
  if (Mode_2_scriptLineCount <= 0) {
    delay(10);
    return;
  }

  if ((Mode_2_targetDuration > 0) && (Mode_2_currentActionTimer < Mode_2_targetDuration)) {
    return;
  }

  Serial.printf("showing line %d\n", Mode_2_scriptLineIndex + 1);
  
  Mode_2_currentActionTimer = 0;
  Mode_2_targetDuration = Mode_2_Script[Mode_2_scriptLineIndex].duration;
  
  Mode_2_updateLEDs(Mode_2_scriptLineIndex);

  Mode_2_scriptLineIndex++;
  Mode_2_scriptLineIndex %= Mode_2_scriptLineCount;
}


void Mode_2_updateLEDs(short scriptIndex) {
  Mode_2_leds[0] = CHSV(Mode_2_Script[scriptIndex].leds[0], Mode_2_Script[scriptIndex].leds[1], Mode_2_Script[scriptIndex].leds[2]);
  Mode_2_leds[1] = CHSV(Mode_2_Script[scriptIndex].leds[3], Mode_2_Script[scriptIndex].leds[4], Mode_2_Script[scriptIndex].leds[5]);
  Mode_2_leds[2] = CHSV(Mode_2_Script[scriptIndex].leds[6], Mode_2_Script[scriptIndex].leds[7], Mode_2_Script[scriptIndex].leds[8]);
  Mode_2_leds[3] = CHSV(Mode_2_Script[scriptIndex].leds[9], Mode_2_Script[scriptIndex].leds[10], Mode_2_Script[scriptIndex].leds[11]);
  Mode_2_leds[4] = CHSV(Mode_2_Script[scriptIndex].leds[12], Mode_2_Script[scriptIndex].leds[13], Mode_2_Script[scriptIndex].leds[14]);
  Mode_2_leds[5] = CHSV(Mode_2_Script[scriptIndex].leds[15], Mode_2_Script[scriptIndex].leds[16], Mode_2_Script[scriptIndex].leds[17]);

  FastLED.show();
  delay(1);
}


void Mode_2_parseScript() {
  FastLED.showColor(CHSV(0, 0, 0));
  
  Serial.println("Processing script file to disk");

  File scriptFile = SPIFFS.open(MODE_2_SCRIPT_FILE_NAME, "r");
  if (!scriptFile) {
    Serial.println("No script file currently exist");
    return;
  }

  Mode_2_scriptLineCount = 0;
  String data = "";
  while (scriptFile.available() && Mode_2_scriptLineCount < MODE_2_MAX_SCRIPT_SIZE) {
    char c = scriptFile.read();

    if (c != '\n') {
      data += c;
      continue;
    }

    Mode_2_parseLine(data);
    
    data = "";
  }

  Mode_2_parseLine(data);
  scriptFile.close();

  Serial.printf("Script contains %d lines\n", Mode_2_scriptLineCount);

  Mode_2_currentActionTimer = 0;
  Mode_2_scriptLineIndex = 0;
  Mode_2_targetDuration = 0;
}


void Mode_2_parseLine(String line) {
    Serial.printf("Processing line %d: %s\n", Mode_2_scriptLineCount + 1, line.c_str());

    ScriptLine scriptLine;
    int match = sscanf(line.c_str(), "%hd %hhd %hhd %hhd %hhd %hhd %hhd %hhd %hhd %hhd %hhd %hhd %hhd %hhd %hhd %hhd %hhd %hhd %hhd", 
                       &scriptLine.duration,
                       &scriptLine.leds[0], &scriptLine.leds[1], &scriptLine.leds[2], 
                       &scriptLine.leds[3], &scriptLine.leds[4], &scriptLine.leds[5], 
                       &scriptLine.leds[6], &scriptLine.leds[7], &scriptLine.leds[8], 
                       &scriptLine.leds[9], &scriptLine.leds[10], &scriptLine.leds[11], 
                       &scriptLine.leds[12], &scriptLine.leds[13], &scriptLine.leds[14], 
                       &scriptLine.leds[15], &scriptLine.leds[16], &scriptLine.leds[17]);

    if (match != 19) {
      return;
    }

    Mode_2_Script[Mode_2_scriptLineCount] = scriptLine;
    Mode_2_scriptLineCount++;
}


void Mode_2_handleOnIndex() {
  Serial.println("Processing index request");
  Mode_2_webserver.sendHeader("Location", "/mode2.html", true);  //Redirect to our html web page
  Mode_2_webserver.send(302, "text/plain", "");
}


void Mode_2_handleOnSave() {
  if ( ! Mode_2_webserver.hasArg("script")) {
    Mode_2_webserver.send(400, "text/plain", "400: Invalid Request");
    return;
  }

  File file = SPIFFS.open(MODE_2_SCRIPT_FILE_NAME, "w");
  if (!file) {
    Mode_2_webserver.send(500, "text/plain", "500: Internal Error");
    Serial.println("Error opening file for writing");
    return;
  }

  String script = Mode_2_webserver.arg("script");
  
  Serial.printf("Got script (%d bytes):\n%s\n", script.length(), script.c_str());

  int bytesWritten = file.print(script);
  file.close();

  if (bytesWritten < 0) {
    Serial.printf("Failed to write script to flash. Wrote %d of %d bytes\n", bytesWritten, script.length());
    return;
  }

  Serial.println("Script recorded to flash");

  Mode_2_parseScript();
}


void Mode_2_handleNotFound() {
  if (Mode_2_loadFromSpiffs(Mode_2_webserver.uri())) {
    return;
  }
  
  String message = "File Not Detected\n\n";
  message += "URI: " + Mode_2_webserver.uri() + "\n";
  message += "Method: " + String((Mode_2_webserver.method() == HTTP_GET) ? "GET" : "POST") + "\n";
  message += String(Mode_2_webserver.args()) + " arguments: \n";
  for (uint8_t i = 0; i < Mode_2_webserver.args(); i++) {
    message += "  " + Mode_2_webserver.argName(i) + "='" + Mode_2_webserver.arg(i) + "'\n";
  }
  
  Mode_2_webserver.send(404, "text/plain", message);

  Serial.printf("File not found: '%s'\n", Mode_2_webserver.uri().c_str());
}


bool Mode_2_loadFromSpiffs(String path) {
  if (path.endsWith("/")) {
    path += "index.htm";
  }

  String dataType = "text/plain";
  if (Mode_2_webserver.hasArg("download")) {
    dataType = "application/octet-stream";
  } else if (path.endsWith(".src")) {
    path = path.substring(0, path.lastIndexOf("."));
  } else if (path.endsWith(".html")) {
    dataType = "text/html";
  } else if (path.endsWith(".txt")) {
    dataType = "text/plain";
  } else if (path.endsWith(".htm")) {
    dataType = "text/html";
  } else if (path.endsWith(".css")) {
    dataType = "text/css";
  } else if (path.endsWith(".js")) {
    dataType = "application/javascript";
  } else if (path.endsWith(".png")) {
    dataType = "image/png";
  } else if (path.endsWith(".gif")) {
    dataType = "image/gif";
  } else if (path.endsWith(".jpg")) {
    dataType = "image/jpeg";
  } else if (path.endsWith(".ico")) {
    dataType = "image/x-icon";
  } else if (path.endsWith(".xml")) {
    dataType = "text/xml";
  } else if (path.endsWith(".pdf")) {
    dataType = "application/pdf";
  } else if (path.endsWith(".zip")) {
    dataType = "application/zip";
  }

  File dataFile = SPIFFS.open(path.c_str(), "r");
  unsigned long have = dataFile.size();
  unsigned long sent = Mode_2_webserver.streamFile(dataFile, dataType);
  dataFile.close();
  
  if (sent != have) {
    Serial.printf("Failed to stream %s. Sent %d of %d bytes\n", path.c_str(), sent, have);
    return false;
  }

  return true;
}
