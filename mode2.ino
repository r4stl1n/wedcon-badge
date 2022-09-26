#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <elapsedMillis.h>
#include <FastLED.h>
#include <SD.h>
#include "mode2.h"


static CRGB Mode_2_leds[MODE_2_NUM_LEDS];
static uint Mode_2_brightness = 0;
static uint Mode_2_hue = 0;
static ESP8266WebServer server(80);

struct ScriptLine Mode_2_Script[MODE_2_MAX_SCRIPT_SIZE];

short Mode_2_ScriptLineCount = 0;


short Mode_2_RunningLineCount = 0;
short Mode_2_targetDuration = 0;
elapsedMillis Mode_2_currentActionTimer = 0;

void Mode_2_Init(String name) {

  // Setup the FastLEDs
  FastLED.addLeds<NEOPIXEL, MODE_2_DATA_PIN>(Mode_2_leds, MODE_2_NUM_LEDS);

  // Setup the wifi access point
  // Server ip is 192.168.4.1
  Serial.print("Setting up wifi network: ");
  Serial.println(MODE_2_WIFI_SSID);
  Serial.println(WiFi.softAP(MODE_2_WIFI_SSID) ? "Ready" : "Failed!");
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());
  
  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  SD.begin(D8);

  // Loading initial file state
  Serial.println("Loading initial file state");
  Mode_2_readFileToScript();

  // Start the esp8266 webserver
  server.begin();

  // Create the page handlers
  server.on("/", HTTP_GET, Mode_2_handleOnIndex);
  server.on("/save", HTTP_POST, Mode_2_handleOnSave);

  server.onNotFound(Mode_2_handleNotFound);

}


void Mode_2_Shutdown() {
  WiFi.scanDelete();
  WiFi.disconnect();

  // Shutdown the web server
  server.close();
  // Shutdown the file system
  SPIFFS.end();

  FastLED.showColor(CHSV(0, 0, 0));
  FastLED.clear();
}


void Mode_2_Loop() {

  server.handleClient();

  Mode_2_processScript();

}

void Mode_2_processScript() {

  // Check if there is any script lines to process
  // If there is not just return out
  if (Mode_2_ScriptLineCount <= 0) {
    return;
  }

  if ((Mode_2_currentActionTimer >= Mode_2_targetDuration) || Mode_2_targetDuration == 0) {

    Mode_2_updateLeds(Mode_2_RunningLineCount);

    Mode_2_targetDuration = Mode_2_Script[Mode_2_RunningLineCount].duration;
    // Reset the action timer
    Mode_2_currentActionTimer = 0;

    if (Mode_2_ScriptLineCount - 1 == Mode_2_RunningLineCount) {
      Mode_2_RunningLineCount = 0;
    } else {
      Mode_2_RunningLineCount = Mode_2_RunningLineCount + 1;
    }

//    Serial.println("LED command duration updated to: " + String(Mode_2_targetDuration));

  }

}

void Mode_2_resetLedLogic()  {
  // Reset the action timer
  Mode_2_currentActionTimer = 0;
  Mode_2_RunningLineCount = 0;
  Mode_2_targetDuration = 0;
}

void Mode_2_updateLeds(short scriptIndex) {

//  Serial.println("Updating leds for script index: " + String(scriptIndex));

  Mode_2_leds[0] = CHSV(Mode_2_Script[scriptIndex].leds[0], Mode_2_Script[scriptIndex].leds[1], Mode_2_Script[scriptIndex].leds[2]);
  Mode_2_leds[1] = CHSV(Mode_2_Script[scriptIndex].leds[3], Mode_2_Script[scriptIndex].leds[4], Mode_2_Script[scriptIndex].leds[5]);
  Mode_2_leds[2] = CHSV(Mode_2_Script[scriptIndex].leds[6], Mode_2_Script[scriptIndex].leds[7], Mode_2_Script[scriptIndex].leds[8]);
  Mode_2_leds[3] = CHSV(Mode_2_Script[scriptIndex].leds[9], Mode_2_Script[scriptIndex].leds[10], Mode_2_Script[scriptIndex].leds[11]);
  Mode_2_leds[4] = CHSV(Mode_2_Script[scriptIndex].leds[12], Mode_2_Script[scriptIndex].leds[13], Mode_2_Script[scriptIndex].leds[14]);
  Mode_2_leds[5] = CHSV(Mode_2_Script[scriptIndex].leds[15], Mode_2_Script[scriptIndex].leds[16], Mode_2_Script[scriptIndex].leds[17]);

  FastLED.show();
}

void Mode_2_readFileToScript() {

  Serial.println("Processing script file to disk");

  File scriptFile = SPIFFS.open(MODE_2_SCRIPT_FILE_NAME, "r");

  if (!scriptFile) {
    Serial.println("No script file currently exist");
    return;
  }

  String data = "";

  // Reset the script line count
  Mode_2_ScriptLineCount = 0;

  while (scriptFile.available()) {

    char letter = scriptFile.read();

    if (letter == '\n') {

      Serial.println("Processing line: " + data);

      // Create a temporary script structure
      ScriptLine scriptLine;

      // We found a new line so we need to process the data we already collected
      // first lets split the data
      sscanf(data.c_str(), "%hd %hhd %hhd %hhd %hhd %hhd %hhd %hhd %hhd %hhd %hhd %hhd %hhd %hhd %hhd %hhd %hhd %hhd %hhd", &scriptLine.duration,
             &scriptLine.leds[0], &scriptLine.leds[1], &scriptLine.leds[2], &scriptLine.leds[3],
             &scriptLine.leds[4], &scriptLine.leds[5], &scriptLine.leds[6], &scriptLine.leds[7],
             &scriptLine.leds[8], &scriptLine.leds[9], &scriptLine.leds[10], &scriptLine.leds[11],
             &scriptLine.leds[12], &scriptLine.leds[13], &scriptLine.leds[14], &scriptLine.leds[15],
             &scriptLine.leds[16], &scriptLine.leds[17]);

      data = "";

      Mode_2_Script[Mode_2_ScriptLineCount] = scriptLine;
      Mode_2_ScriptLineCount = Mode_2_ScriptLineCount + 1;

      continue;
    }

    data = data += letter;

  }

  scriptFile.close();

  Serial.println("Script contains " + String(Mode_2_ScriptLineCount) + " command/s.");

  Mode_2_resetLedLogic();
}

void Mode_2_handleOnIndex() {
  Serial.println("Processing index request");
  server.sendHeader("Location", "/mode2.html", true);  //Redirect to our html web page
  server.send(302, "text/plain", "");
}

void Mode_2_handleOnSave() {

  if ( ! server.hasArg("script")) {
    server.send(400, "text/plain", "400: Invalid Request");
    return;
  }

  File file = SPIFFS.open(MODE_2_SCRIPT_FILE_NAME, "w");

  if (!file) {
    Serial.println("Error opening file for writing");
    return;
  }

  Serial.println(server.arg("script"));

  int bytesWritten = file.print(server.arg("script"));

  Serial.println(bytesWritten);

  file.close();

  if (bytesWritten < 0) {
    Serial.println("Failed to write script to flash");
    return;
  }

  Serial.println("Script recorded to flash");

  Mode_2_readFileToScript();
}

bool Mode_2_loadFromSpiffs(String path) {
  String dataType = "text/plain";

  if (path.endsWith("/")) path += "index.htm";

  if (path.endsWith(".src")) path = path.substring(0, path.lastIndexOf("."));
  else if (path.endsWith(".html")) dataType = "text/html";
  else if (path.endsWith(".txt")) dataType = "text/plain";
  else if (path.endsWith(".htm")) dataType = "text/html";
  else if (path.endsWith(".css")) dataType = "text/css";
  else if (path.endsWith(".js")) dataType = "application/javascript";
  else if (path.endsWith(".png")) dataType = "image/png";
  else if (path.endsWith(".gif")) dataType = "image/gif";
  else if (path.endsWith(".jpg")) dataType = "image/jpeg";
  else if (path.endsWith(".ico")) dataType = "image/x-icon";
  else if (path.endsWith(".xml")) dataType = "text/xml";
  else if (path.endsWith(".pdf")) dataType = "application/pdf";
  else if (path.endsWith(".zip")) dataType = "application/zip";

  File dataFile = SPIFFS.open(path.c_str(), "r");

  if (server.hasArg("download")) dataType = "application/octet-stream";

  if (server.streamFile(dataFile, dataType) != dataFile.size()) {
    Serial.println(path + " - " + "stream != dataFile size");
    dataFile.close();
    return false;
  }

  dataFile.close();
  return true;
}

void Mode_2_handleNotFound() {
  if (Mode_2_loadFromSpiffs(server.uri())) return;
  String message = "File Not Detected\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " NAME:" + server.argName(i) + "\n VALUE:" + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  Serial.println(message);
}
