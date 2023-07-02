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
// - ledflash.lgo
// - btnrgbflash.lgo
// - btnledflash.lgo
//    for an ESP32, set BTNPIN to 27, otherwise you can use 9 for the Leonardo
// - rgb.lgo
// - trafficlights.lgo
// - sos.lgo
// - tiny.lgo

//#LOGO FILE=../logo/ledflash.lgo NAME=sketch BTNPIN=9 INLINE=true
static const char program_sketch[] PROGMEM = 
"#   ledflash.lgo\n"
"#\n"
"#   Author: Paul Hamilton (paul@visualops.com)\n"
"#   Date: 2-Jun-2023\n"
"#\n"
"#   Flash an LED, this is all internal, no external words needed.\n"
"#\n"
"#   This work is licensed under the Creative Commons Attribution 4.0 International License. \n"
"#   To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/ or \n"
"#   send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.\n"
"#\n"
"#   https://github.com/visualopsholdings/tinylogo  \n"
"#\n"
"\n"
"to ID\n"
"  print \"ledflash\n"
"end\n"
"\n"
"# change your pins here\n"
"to LEDPIN\n"
"  13\n"
"end\n"
"\n"
"to SETUP\n"
"  pinout LEDPIN\n"
"end\n"
"\n"
"to ON\n"
"  dhigh LEDPIN\n"
"end\n"
"\n"
"to OFF\n"
"  dlow LEDPIN\n"
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
//"GO\n"
;
//#LOGO ENDFILE

LogoInlineSketch sketch((char *)program_sketch);

// At the start
void setup() {
  sketch.setup(115200);
}

// Go around and around
void loop() {
  sketch.loop();
}

