/*
  A holder for a logo sketch with actual inline code compiled on the device.
  
  Author: Paul Hamilton
  Date: 30-Jun-2023
  
  This work is licensed under the Creative Commons Attribution 4.0 International License. 
  To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/ or 
  send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  https://github.com/visualopsholdings/tinylogo
*/

#include "logosketch.hpp"

// change this line to set the logo program you want to upload
// Other files are
// in ../logo/
// - ledflash.lgo
// - btnrgbflash.lgo
// - btnledflash.lgo
//    for an ESP32, set BTNPIN to 27, otherwise you can use 9 for the Leonardo
// - rgb.lgo 
//    for a normal AVR RED=3 GREEN=4 BLUE=2
//    for an ESP32 RED=26 GREEN=25 BLUE=27
// - trafficlights.lgo
// - sos.lgo
// - tiny.lgo
// - wifiscan.lgo
// - exceptions.lgo
// - ../../../../esp32/wifitest.lgo
// - ../../../../esp32/vopsnotify.lgo
// - ledbot.lgo

//#LOGO FILE=../logo/ledbot.lgo NAME=sketch INLINE=true
static const char program_sketch[] PROGMEM = 
"#   ledbot.lgo\n"
"#\n"
"#   Author: Paul Hamilton (paul@visualops.com)\n"
"#   Date: 11-Oct-2023\n"
"#\n"
"#   Control the LEDBOT hardware interface\n"
"#\n"
"#   This work is licensed under the Creative Commons Attribution 4.0 International License. \n"
"#   To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/ or \n"
"#   send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.\n"
"#\n"
"#   https://github.com/visualopsholdings/tinylogo  \n"
"#\n"
"\n"
"to ID\n"
"  print \"ledbot\n"
"end\n"
"\n"
"to SETUP\n"
"  pinrgb 10 1\n"
"  rgbout 1 255\n"
"end\n"
"\n"
"to ON\n"
"  rgbout 1 0\n"
"end\n"
"\n"
"to OFF\n"
"  rgbout 1 255\n"
"end\n"
"\n"
"to FLASH\n"
"  ON wait 500 OFF wait 1000\n"
"end\n"
"\n"
"to GO\n"
"  forever FLASH\n"
"end\n"
"\n"
"to STOP\n"
"  OFF\n"
"end\n"
"\n"
"to FADEUP\n"
"  rgbout 1 255\n"
"  wait 500\n"
"  rgbout 1 200\n"
"  wait 500\n"
"  rgbout 1 150\n"
"  wait 500\n"
"  rgbout 1 100\n"
"  wait 500\n"
"  rgbout 1 50\n"
"  wait 500\n"
"  rgbout 1 0\n"
"  wait 500\n"
"end\n"
"\n"
"\n"
"FADEUP\n"
"\n"
;
//#LOGO ENDFILE

LogoInlineSketch sketch((char *)program_sketch);

// At the start
void setup() {
//  sketch.setup(115200);
  sketch.setup(9600);
}

// Go around and around
void loop() {
  sketch.loop();
}

