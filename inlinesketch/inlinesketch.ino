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

//#LOGO FILE=../../../../esp32/vopsnotify.lgo NAME=sketch INLINE=true
static const char program_sketch[] PROGMEM = 
"#   vopsnotify.lgo\n"
"#\n"
"#   Author: Paul Hamilton (paul@visualops.com)\n"
"#   Date: 2-Jul-2023\n"
"#\n"
"#   Notify that a new message came in on a Vops stream.\n"
"#\n"
"#         make \"SSID \"My Access Point\"\n"
"#         make \"PWD \"Access Point Password\"\n"
"#         make \"HOST \"au.visualops.com\"\n"
"#         make \"PORT 443\n"
"#         make \"DOCID \"Stream ID\"\n"
"#         make \"VID \"VID\"\n"
"#         LOGIN\n"
"#\n"
"#   This work is licensed under the Creative Commons Attribution 4.0 International License. \n"
"#   To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/ or \n"
"#   send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.\n"
"#\n"
"#   https://github.com/visualopsholdings/tinylogo  \n"
"#\n"
"\n"
"to ID\n"
"  print \"vopsnotify\n"
"end\n"
"\n"
"# called right at the start\n"
"to SETUP\n"
"  #setup the pins and channels\n"
"  pinrgb 26 1\n"
"  rgbout 1 256\n"
"  pinrgb 25 2\n"
"  rgbout 2 256\n"
"  pinrgb 27 3\n"
"  rgbout 3 256\n"
"  btstart \"vopsnotify\" \"1030945b-2c2d-4627-84a2-d2383259be94\"\n"
"  # set the LED to blue\n"
"  rgbout 3 0\n"
"end\n"
"\n"
"to REDR :V\n"
"  rgbout 1 :V\n"
"end\n"
"\n"
"to GREENR :V\n"
"  rgbout 2 :V\n"
"end\n"
"\n"
"to BLUER :V\n"
"  rgbout 3 :V\n"
"end\n"
"\n"
"# scale a color to be between 1 and 100\n"
"to SCLR :C\n"
"  256 - ((:C / 100) * 256)\n"
"end\n"
"\n"
"# arguments are reversed!\n"
"to SET :B :G :R\n"
"  REDR SCLR :R\n"
"  GREENR SCLR :G\n"
"  BLUER SCLR :B\n"
"end\n"
"\n"
"to AMBER\n"
" SET 100 75 0\n"
"end\n"
"\n"
"to RED\n"
"  SET 100 0 0\n"
"end\n"
"\n"
"to GREEN\n"
"  SET 0 100 0\n"
"end\n"
"\n"
"to BLUE\n"
"  SET 0 0 100\n"
"end\n"
"\n"
"to OFF\n"
"  SET 0 0 0\n"
"end\n"
"\n"
"to FLASH\n"
"  AMBER wait 500 OFF wait 1000\n"
"end\n"
"\n"
"to SHOWERR\n"
"  print &\n"
"  RED\n"
"end\n"
"\n"
"to USERID\n"
"  wifiget :HOST :PORT \"/rest/1.0/users/me\" \"_id\"\n"
"end\n"
"\n"
"to MSG\n"
"  vopsopenmsg USERID :DOCID\n"
"end\n"
"\n"
"to STARTWIFI\n"
"  AMBER\n"
"  wifistation\n"
"  print wificonnect :SSID :PWD\n"
"end\n"
"\n"
"to LOGIN\n"
"  { \n"
"    STARTWIFI\n"
"    print wifilogin :HOST :PORT \"\" :VID\n"
"    wifisockets :HOST :PORT MSG\n"
"    OFF\n"
"  } SHOWERR\n"
"end\n"
"\n"
"to LOGINTEST\n"
"  { \n"
"    STARTWIFI\n"
"    print wifilogintest :HOST :PORT \"tracy\"\n"
"    wifisockets :HOST :PORT MSG\n"
"    OFF\n"
"  } SHOWERR\n"
"end\n"
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

