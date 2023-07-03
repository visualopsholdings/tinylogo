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
//    for a normal AVR RED=3 GREEN=4 BLUE=2
//    for an ESP32 RED=26 GREEN=25 BLUE=27
// - trafficlights.lgo
// - sos.lgo
// - tiny.lgo
// - wifiscan.lgo
// - wifisend.lgo

//#LOGO FILE=../logo/rgb.lgo NAME=sketch INLINE=true RED=26 GREEN=25 BLUE=27
static const char program_sketch[] PROGMEM = 
"#   rgb.lgo\n"
"#\n"
"#   Author: Paul Hamilton (paul@visualops.com)\n"
"#   Date: 2-Jun-2023\n"
"#\n"
"#   Allow colors to be set on an RGB LED\n"
"#\n"
"#   This work is licensed under the Creative Commons Attribution 4.0 International License. \n"
"#   To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/ or \n"
"#   send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.\n"
"#\n"
"#   https://github.com/visualopsholdings/tinylogo  \n"
"#\n"
"\n"
"to ID\n"
"  print \"rgb\n"
"end\n"
"\n"
"# change your pins here\n"
"to BPIN\n"
"  27\n"
"end\n"
"\n"
"to RPIN\n"
"  26\n"
"end\n"
"\n"
"to GPIN\n"
"  25\n"
"end\n"
"\n"
"# called right at the start\n"
"to SETUP\n"
"  pinrgb RPIN 1\n"
"  REDR SCLR 0\n"
"  pinrgb GPIN 2\n"
"  GREENR SCLR 0\n"
"  pinrgb BPIN 3\n"
"  BLUER SCLR 0\n"
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
"  (:C / 100) * 255\n"
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
"# SETUP\n"
"AMBER\n"
;
//#LOGO ENDFILE

LogoInlineSketch sketch((char *)program_sketch);

// for HTTPS, uncomment all this and then run flashcode and it will pull that file
// in as a variable.
// const char *host_cert =
// //#LOGO FILE=/Users/paul/Downloads/ISRG%20Root%20X1.cer INLINE=true
// //#LOGO ENDFILE
// ;

// At the start
void setup() {
  sketch.setup(115200);
  // for HTTPS, uncomment this and set the hostname you will be calling.
//  sketch.sslsetup("hostname", host_cert);
}

// Go around and around
void loop() {
  sketch.loop();
}

