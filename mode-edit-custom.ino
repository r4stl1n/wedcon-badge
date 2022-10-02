#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SD.h>

#include "mode-edit-custom.h"
#include "led.h"


static ESP8266WebServer Mode_Edit_Custom_webserver(80);

static void Mode_Edit_Custom_handleOnIndex();
static void Mode_Edit_Custom_handleOnSave();
static void Mode_Edit_Custom_handleNotFound();
static bool Mode_Edit_Custom_loadFromSpiffs(String path);


void Mode_Edit_Custom_Init() {
  String name = Wifi_GetName();
  WiFi.softAP(name);
  delay(100);

  Serial.printf("Ready. To upload your own script, join WiFi network %s\n", name.c_str());

  delay(100);

  Mode_Edit_Custom_webserver.begin();
  Mode_Edit_Custom_webserver.on("/", HTTP_GET, Mode_Edit_Custom_handleOnIndex);
  Mode_Edit_Custom_webserver.on("/save", HTTP_POST, Mode_Edit_Custom_handleOnSave);
  Mode_Edit_Custom_webserver.onNotFound(Mode_Edit_Custom_handleNotFound);

  LED_Flash(2);
}


void Mode_Edit_Custom_Loop() {
  Mode_Edit_Custom_webserver.handleClient();
}


void Mode_Edit_Custom_Shutdown() {
  Mode_Edit_Custom_webserver.close();

  LED_Flash(2);
}


void Mode_Edit_Custom_handleOnIndex() {
  Serial.println("Processing index request");
  Mode_Edit_Custom_webserver.sendHeader("Location", "/mode2.html", true);  //Redirect to our html web page
  Mode_Edit_Custom_webserver.send(302, "text/plain", "");
}


void Mode_Edit_Custom_handleOnSave() {
  if (!Mode_Edit_Custom_webserver.hasArg("script")) {
    Mode_Edit_Custom_webserver.send(400, "text/plain", "400: Invalid Request");
    return;
  }

  File file = SPIFFS.open(LED_CUSTOM_SCRIPT_FILE_NAME, "w");
  if (!file) {
    Mode_Edit_Custom_webserver.send(500, "text/plain", "500: Internal Error");
    Serial.println("Error opening file for writing");
    return;
  }

  String script = Mode_Edit_Custom_webserver.arg("script");
  
  Serial.printf("Got script (%d bytes):\n%s\n", script.length(), script.c_str());

  int bytesWritten = file.print(script);
  file.close();

  if (bytesWritten < 0) {
    Serial.printf("Failed to write script to flash. Wrote %d of %d bytes\n", bytesWritten, script.length());
    return;
  }

  Serial.println("Script recorded to flash");

  LED_ChangePattern(LEDScript);
}


void Mode_Edit_Custom_handleNotFound() {
  if (Mode_Edit_Custom_loadFromSpiffs(Mode_Edit_Custom_webserver.uri())) {
    return;
  }
  
  String message = "File Not Detected\n\n";
  message += "URI: " + Mode_Edit_Custom_webserver.uri() + "\n";
  message += "Method: " + String((Mode_Edit_Custom_webserver.method() == HTTP_GET) ? "GET" : "POST") + "\n";
  message += String(Mode_Edit_Custom_webserver.args()) + " arguments: \n";
  for (uint8_t i = 0; i < Mode_Edit_Custom_webserver.args(); i++) {
    message += "  " + Mode_Edit_Custom_webserver.argName(i) + "='" + Mode_Edit_Custom_webserver.arg(i) + "'\n";
  }
  
  Mode_Edit_Custom_webserver.send(404, "text/plain", message);

  Serial.printf("File not found: '%s'\n", Mode_Edit_Custom_webserver.uri().c_str());
}


bool Mode_Edit_Custom_loadFromSpiffs(String path) {
  if (path.endsWith("/")) {
    path += "index.htm";
  }

  String dataType = "text/plain";
  if (Mode_Edit_Custom_webserver.hasArg("download")) {
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
  unsigned long sent = Mode_Edit_Custom_webserver.streamFile(dataFile, dataType);
  dataFile.close();
  
  if (sent != have) {
    Serial.printf("Failed to stream %s. Sent %d of %d bytes\n", path.c_str(), sent, have);
    return false;
  }

  return true;
}
