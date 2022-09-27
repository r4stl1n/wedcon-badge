# wedcon-badge

## Modes

This badge features three main modes:

1. Proximity Mode

In this mode, the badge cycles through the colors of the rainbow while idle. When it detects that the wedding couple is near, it alerts by first flashing and then 'breathing' a fixed color. The colors are as follows

* White: both the bride and the groom are closeby
* Pink: only the bride is nearby
* Blue: only the groom is close


2. Custom Mode

In 'custom' mode, the badge displays a sequence of colors. This sequence can be changed by the owner. 

In order to do so, the badge must be put in edit mode first. This can be done by pressing and holding the flash button (left button) for 5 seconds. 

Then, the user can connect to the badge's wifi network and point a web browser at 
	http://192.168.4.1

When the user clicks on 'Finish', the new color sequence will be saved and displayed whenever the badge is in this mode.


3. Off Mode

In 'off' mode, the badge turns off its lights. 

This mode can also be used to reprogram the badge. Press and hold the flash button (left button) for 5 seconds. The badge will flash 3 times to indicate it is in flash mode. 

Use your laptop to connect to the badge's wifi network, run the Arduino IDE, configure it to connect to the badge's wifi port, and flash your own sketch!

To exit flash mode without reflashing your badge, just hit the reset button on the badge.


## Reflashing the Badge

You can create your own sketch or modify the firmware that came with your badge. To do that, you need to install the following prerequisites.

Install Arduino 1.8 (version 2.0 can't upload the data folder!!!)
* https://www.arduino.cc/en/software

Install Arduino file system uploader plugin:
* Install https://github.com/esp8266/arduino-esp8266fs-plugin

Add board URL in preferences:
* http://arduino.esp8266.com/stable/package_esp8266com_index.json

Add board:
* ESP8266 (which includes ESP-12E)

To modify the firmware that came with your badge, clone the following repository using 'git'
* https://github.com/r4stl1n/wedcon-badge

You need to add these libraries needed to build this firmware:
* ArduinoOTA
* Bounce2
* elapsedMillis
* ESP8266WebServer
* ESP8266WiFi
* FastLED
* SD

If you modify the files in the 'data' folder, don't forget to push them to the badge by using the 'ESP8266 Sketch Data Upload' item in the tools menu.

Have fun!
