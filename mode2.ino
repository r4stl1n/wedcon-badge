#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FastLED.h>
#include <SD.h>
#include "mode2.h"


static CRGB Mode_2_leds[MODE_2_NUM_LEDS];
static uint Mode_2_brightness = 0;
static uint Mode_2_hue = 0;
static ESP8266WebServer server(80);

String header;

struct ScriptLine script[MODE_2_MAX_SCRIPT_SIZE];

void Mode_2_Init() {

  // Setup the FastLEDs
  FastLED.addLeds<NEOPIXEL, MODE_1_DATA_PIN>(Mode_1_leds, MODE_1_NUM_LEDS);

  // Setup the wifi access point
  // Server ip is 192.168.4.1
  Serial.print("Setting up wifi network: ");
  Serial.println(MODE_2_WIFI_SSID);
  Serial.println(WiFi.softAP(MODE_2_WIFI_SSID, "", MODE_2_WIFI_CHANNEL, 10) ? "Ready" : "Failed!");

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
}

void Mode_2_readFileToScript() {
  File scriptFile = SPIFFS.open("/mode2-script.txt", "r");

  if (!scriptFile) {
    Serial.println("No script file currently exist");
    return;
  }

  String data = "";
  while (scriptFile.available()) {
    char letter = scriptFile.read();

    if (letter == "\n") {

      continue
    }

    data = data += letter;

  }

  Serial.println(data);

  scriptFile.close();
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

  File file = SPIFFS.open("/mode2-script.txt", "w");

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
}

bool Mode_2_loadFromSpiffs(String path) {
  String dataType = "text/plain";

  if (path.endsWith("/")) path += "index.htm";

  if (path.endsWith(".src")) path = path.substring(0, path.lastIndexOf("."));
  else if (path.endsWith(".html")) dataType = "text/html";
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
