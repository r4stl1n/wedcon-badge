#include "mode-custom.h"
#include "led.h"


/*
static struct ScriptLine LED_testScript[] = {
  { 300, { { 0, 0, 255 }, { 0, 0, 255 }, { 0, 0, 255 }, { 0, 0, 255 }, { 0, 0, 255 }, { 0, 0, 255 } } },
  { 300, { { 0, 255, 255 }, { 0, 255, 255 }, { 0, 255, 255 }, { 0, 255, 255 }, { 0, 255, 255 }, { 0, 255, 255 } } }
};
*/


void Mode_Custom_Init() {
  LED_Off();
  LED_ChangePattern(LEDScript);
//  LED_SetScript(sizeof(LED_testScript)/sizeof(struct ScriptLine), LED_testScript);
}


void Mode_Custom_Loop() {
}


void Mode_Custom_Shutdown() {
}


/*
static CRGB ModeCustom_leds[MODE_CUSTOM_NUM_LEDS];
static ESP8266WebServer ModeCustom_webserver(80);
static struct ScriptLine ModeCustom_Script[MODE_CUSTOM_MAX_SCRIPT_SIZE];
static short ModeCustom_scriptLineCount = 0;
static short ModeCustom_scriptLineIndex = 0;
static short ModeCustom_targetDuration = 0;
static elapsedMillis ModeCustom_currentActionTimer = 0;
static bool ModeCustom_updateEnabled = false;
static String ModeCustom_SSID;

static void ModeCustom_runScript();
static void ModeCustom_updateLEDs(short scriptIndex);
static void ModeCustom_parseScript();
static void ModeCustom_handleOnIndex();
static void ModeCustom_handleOnSave();
static void ModeCustom_handleNotFound();
static bool ModeCustom_loadFromSpiffs(String path);


void ModeCustom_Init() {
  ModeCustom_SSID = name;
  ModeCustom_updateEnabled = false;
  
  FastLED.addLeds<NEOPIXEL, MODECustom_DATA_PIN>(ModeCustom_leds, MODE_CUSTOM_NUM_LEDS);
  FastLED.showColor(CHSV(0, 0, 0));

  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  SD.begin(D8);

  ModeCustom_parseScript();
}


void ModeCustom_Shutdown() {
  ModeCustom_webserver.close();

  SD.end();

  SPIFFS.end();

  WiFi.disconnect();

  FastLED.showColor(CHSV(0, 0, 0));
  FastLED.clear();
}


void ModeCustom_Loop() {
  ModeCustom_webserver.handleClient();
  ModeCustom_runScript();
}


void ModeCustom_EnableUpdate() {
  if(ModeCustom_updateEnabled) {
    return;
  }
  
  ModeCustom_updateEnabled = true;
  
  // Setup the wifi access point
  // Server ip should be 192.168.4.1
//  Serial.printf("Setting up wifi network: %s - %s\n", ModeCustom_SSID.c_str(), WiFi.softAP(ModeCustom_SSID) ? "Ready" : "Failed!");
  bool ok = WiFi.softAP(ModeCustom_SSID);
  Serial.printf(">>>>>>>>>>> new code here \n");
  Serial.printf("Setting up wifi network: %s - %s\n", ModeCustom_SSID.c_str(), ok ? "Ready" : "Failed!");
  Serial.printf("IP address: %d.%d.%d.%d\n", WiFi.softAPIP()[0], WiFi.softAPIP()[1], WiFi.softAPIP()[2], WiFi.softAPIP()[3]);
  
  ModeCustom_webserver.begin();
  ModeCustom_webserver.on("/", HTTP_GET, ModeCustom_handleOnIndex);
  ModeCustom_webserver.on("/save", HTTP_POST, ModeCustom_handleOnSave);
  ModeCustom_webserver.onNotFound(ModeCustom_handleNotFound);
}


void ModeCustom_runScript() {
  if (ModeCustom_scriptLineCount <= 0) {
    delay(10);
    return;
  }

  if ((ModeCustom_targetDuration > 0) && (ModeCustom_currentActionTimer < ModeCustom_targetDuration)) {
    return;
  }

  Serial.printf("showing line %d\n", ModeCustom_scriptLineIndex + 1);
  
  ModeCustom_currentActionTimer = 0;
  ModeCustom_targetDuration = ModeCustom_Script[ModeCustom_scriptLineIndex].duration;
  
  ModeCustom_updateLEDs(ModeCustom_scriptLineIndex);

  ModeCustom_scriptLineIndex++;
  ModeCustom_scriptLineIndex %= ModeCustom_scriptLineCount;
}


void ModeCustom_updateLEDs(short scriptIndex) {
  ModeCustom_leds[0] = CHSV(ModeCustom_Script[scriptIndex].leds[0], ModeCustom_Script[scriptIndex].leds[1], ModeCustom_Script[scriptIndex].leds[2]);
  ModeCustom_leds[1] = CHSV(ModeCustom_Script[scriptIndex].leds[3], ModeCustom_Script[scriptIndex].leds[4], ModeCustom_Script[scriptIndex].leds[5]);
  ModeCustom_leds[2] = CHSV(ModeCustom_Script[scriptIndex].leds[6], ModeCustom_Script[scriptIndex].leds[7], ModeCustom_Script[scriptIndex].leds[8]);
  ModeCustom_leds[3] = CHSV(ModeCustom_Script[scriptIndex].leds[9], ModeCustom_Script[scriptIndex].leds[10], ModeCustom_Script[scriptIndex].leds[11]);
  ModeCustom_leds[4] = CHSV(ModeCustom_Script[scriptIndex].leds[12], ModeCustom_Script[scriptIndex].leds[13], ModeCustom_Script[scriptIndex].leds[14]);
  ModeCustom_leds[5] = CHSV(ModeCustom_Script[scriptIndex].leds[15], ModeCustom_Script[scriptIndex].leds[16], ModeCustom_Script[scriptIndex].leds[17]);

  FastLED.show();
  delay(1);
}


void ModeCustom_parseScript() {
  FastLED.showColor(CHSV(0, 0, 0));
  
  Serial.println("Processing script file to disk");

  File scriptFile = SPIFFS.open(MODECustom_SCRIPT_FILE_NAME, "r");
  if (!scriptFile) {
    Serial.println("No script file currently exist");
    return;
  }

  ModeCustom_scriptLineCount = 0;
  String data = "";
  while (scriptFile.available() && ModeCustom_scriptLineCount < MODECustom_MAX_SCRIPT_SIZE) {
    char c = scriptFile.read();

    if (c != '\n') {
      data += c;
      continue;
    }

    ModeCustom_parseLine(data);
    
    data = "";
  }

  ModeCustom_parseLine(data);
  scriptFile.close();

  Serial.printf("Script contains %d lines\n", ModeCustom_scriptLineCount);

  ModeCustom_currentActionTimer = 0;
  ModeCustom_scriptLineIndex = 0;
  ModeCustom_targetDuration = 0;
}


void ModeCustom_parseLine(String line) {
    Serial.printf("Processing line %d: %s\n", ModeCustom_scriptLineCount + 1, line.c_str());

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

    ModeCustom_Script[ModeCustom_scriptLineCount] = scriptLine;
    ModeCustom_scriptLineCount++;
}


void ModeCustom_handleOnIndex() {
  Serial.println("Processing index request");
  ModeCustom_webserver.sendHeader("Location", "/mode2.html", true);  //Redirect to our html web page
  ModeCustom_webserver.send(302, "text/plain", "");
}


void ModeCustom_handleOnSave() {
  if ( ! ModeCustom_webserver.hasArg("script")) {
    ModeCustom_webserver.send(400, "text/plain", "400: Invalid Request");
    return;
  }

  File file = SPIFFS.open(MODECustom_SCRIPT_FILE_NAME, "w");
  if (!file) {
    ModeCustom_webserver.send(500, "text/plain", "500: Internal Error");
    Serial.println("Error opening file for writing");
    return;
  }

  String script = ModeCustom_webserver.arg("script");
  
  Serial.printf("Got script (%d bytes):\n%s\n", script.length(), script.c_str());

  int bytesWritten = file.print(script);
  file.close();

  if (bytesWritten < 0) {
    Serial.printf("Failed to write script to flash. Wrote %d of %d bytes\n", bytesWritten, script.length());
    return;
  }

  Serial.println("Script recorded to flash");

  ModeCustom_parseScript();
}


void ModeCustom_handleNotFound() {
  if (ModeCustom_loadFromSpiffs(ModeCustom_webserver.uri())) {
    return;
  }
  
  String message = "File Not Detected\n\n";
  message += "URI: " + ModeCustom_webserver.uri() + "\n";
  message += "Method: " + String((ModeCustom_webserver.method() == HTTP_GET) ? "GET" : "POST") + "\n";
  message += String(ModeCustom_webserver.args()) + " arguments: \n";
  for (uint8_t i = 0; i < ModeCustom_webserver.args(); i++) {
    message += "  " + ModeCustom_webserver.argName(i) + "='" + ModeCustom_webserver.arg(i) + "'\n";
  }
  
  ModeCustom_webserver.send(404, "text/plain", message);

  Serial.printf("File not found: '%s'\n", ModeCustom_webserver.uri().c_str());
}


bool ModeCustom_loadFromSpiffs(String path) {
  if (path.endsWith("/")) {
    path += "index.htm";
  }

  String dataType = "text/plain";
  if (ModeCustom_webserver.hasArg("download")) {
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
  unsigned long sent = ModeCustom_webserver.streamFile(dataFile, dataType);
  dataFile.close();
  
  if (sent != have) {
    Serial.printf("Failed to stream %s. Sent %d of %d bytes\n", path.c_str(), sent, have);
    return false;
  }

  return true;
}
*/
