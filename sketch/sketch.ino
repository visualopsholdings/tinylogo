/*
  A holder for a logo sketch.
  
  Author: Paul Hamilton
  Date: 14-Jun-2023
  
  This work is licensed under the Creative Commons Attribution 4.0 International License. 
  To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/ or 
  send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  https://github.com/visualopsholdings/tinylogo
*/

// minimum on a Leonardo is
// Sketch uses 3462 bytes (12%) of program storage space. Maximum is 28672 bytes.
// Global variables use 149 bytes (5%) of dynamic memory, leaving 2411 bytes for local variables. Maximum is 2560 bytes.

// just including some static code and strings might give
// Sketch uses 17540 bytes (61%) of program storage space. Maximum is 28672 bytes.
// Global variables use 1386 bytes (54%) of dynamic memory, leaving 1174 bytes for local variables. Maximum is 2560 bytes.

// including "Serial.begin()" adds 12 bytes to program storage, no dynamic memory

// including our sketch code
// Sketch uses 21910 bytes (76%) of program storage space. Maximum is 28672 bytes.
// Global variables use 1428 bytes (55%) of dynamic memory, leaving 1132 bytes for local variables. Maximum is 2560 bytes.

#include "logosketch.hpp"

// this sketch relies on the USE_FLASH_CODE being uncommented in logo.hpp

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

//#LOGO FILE=../logo/btnledflash.lgo NAME=sketch BTNPIN=9
static const char strings_sketch[] PROGMEM = {
// words
	"ID\n"
	"LEDPIN\n"
	"BTNPIN\n"
	"SETUP\n"
	"ON\n"
	"OFF\n"
	"FLASH\n"
	"BUTTON\n"
	"TESTSTATE\n"
	"TESTBTN\n"
	"RUN\n"
// variables
// strings
	"btnledflash\n"
	"ON\n"
	"OFF\n"
	"OLDVALUE\n"
	"VALUE\n"
};
static const short code_sketch[][INST_LENGTH] PROGMEM = {
	{ OPTYPE_JUMP, 52, 0 },		// 0
	{ OPTYPE_HALT, 0, 0 },		// 1
	{ OPTYPE_BUILTIN, 18, 0 },		// 2
	{ OPTYPE_STRING, 11, 11 },		// 3
	{ OPTYPE_RETURN, 0, 0 },		// 4
	{ OPTYPE_INT, 13, 0 },		// 5
	{ OPTYPE_RETURN, 0, 0 },		// 6
	{ OPTYPE_INT, 9, 0 },		// 7
	{ OPTYPE_RETURN, 0, 0 },		// 8
	{ OPTYPE_BUILTIN, 23, 0 },		// 9
	{ OPTYPE_JUMP, 5, 0 },		// 10
	{ OPTYPE_BUILTIN, 22, 0 },		// 11
	{ OPTYPE_JUMP, 5, 0 },		// 12
	{ OPTYPE_BUILTIN, 30, 0 },		// 13
	{ OPTYPE_JUMP, 7, 0 },		// 14
	{ OPTYPE_RETURN, 0, 0 },		// 15
	{ OPTYPE_BUILTIN, 18, 0 },		// 16
	{ OPTYPE_STRING, 4, 2 },		// 17
	{ OPTYPE_BUILTIN, 21, 0 },		// 18
	{ OPTYPE_JUMP, 5, 0 },		// 19
	{ OPTYPE_RETURN, 0, 0 },		// 20
	{ OPTYPE_BUILTIN, 18, 0 },		// 21
	{ OPTYPE_STRING, 5, 3 },		// 22
	{ OPTYPE_BUILTIN, 22, 0 },		// 23
	{ OPTYPE_JUMP, 5, 0 },		// 24
	{ OPTYPE_RETURN, 0, 0 },		// 25
	{ OPTYPE_JUMP, 16, 0 },		// 26
	{ OPTYPE_BUILTIN, 6, 0 },		// 27
	{ OPTYPE_INT, 1000, 0 },		// 28
	{ OPTYPE_JUMP, 21, 0 },		// 29
	{ OPTYPE_RETURN, 0, 0 },		// 30
	{ OPTYPE_BUILTIN, 20, 0 },		// 31
	{ OPTYPE_JUMP, 7, 0 },		// 32
	{ OPTYPE_RETURN, 0, 0 },		// 33
	{ OPTYPE_BUILTIN, 1, 0 },		// 34
	{ OPTYPE_STRING, 14, 8 },		// 35
	{ OPTYPE_REF, 15, 5 },		// 36
	{ OPTYPE_BUILTIN, 5, 0 },		// 37
	{ OPTYPE_REF, 15, 5 },		// 38
	{ OPTYPE_BUILTIN, 7, 0 },		// 39
	{ OPTYPE_INT, 1, 0 },		// 40
	{ OPTYPE_JUMP, 26, 0 },		// 41
	{ OPTYPE_RETURN, 0, 0 },		// 42
	{ OPTYPE_BUILTIN, 1, 0 },		// 43
	{ OPTYPE_STRING, 15, 5 },		// 44
	{ OPTYPE_JUMP, 31, 0 },		// 45
	{ OPTYPE_BUILTIN, 5, 0 },		// 46
	{ OPTYPE_REF, 15, 5 },		// 47
	{ OPTYPE_BUILTIN, 8, 0 },		// 48
	{ OPTYPE_REF, 14, 8 },		// 49
	{ OPTYPE_JUMP, 34, 0 },		// 50
	{ OPTYPE_RETURN, 0, 0 },		// 51
	{ OPTYPE_BUILTIN, 2, 0 },		// 52
	{ OPTYPE_JUMP, 43, 0 },		// 53
	{ OPTYPE_RETURN, 0, 0 },		// 54
	{ SCOPTYPE_WORD, 2, 0 }, 
	{ SCOPTYPE_WORD, 5, 0 }, 
	{ SCOPTYPE_WORD, 7, 0 }, 
	{ SCOPTYPE_WORD, 9, 0 }, 
	{ SCOPTYPE_WORD, 16, 0 }, 
	{ SCOPTYPE_WORD, 21, 0 }, 
	{ SCOPTYPE_WORD, 26, 0 }, 
	{ SCOPTYPE_WORD, 31, 0 }, 
	{ SCOPTYPE_WORD, 34, 0 }, 
	{ SCOPTYPE_WORD, 43, 0 }, 
	{ SCOPTYPE_WORD, 52, 0 }, 
	{ SCOPTYPE_END, 0, 0 } 
};
//#LOGO ENDFILE

LogoSketch sketch((char *)strings_sketch, (const PROGMEM short *)code_sketch);

// At the start
void setup() {
  sketch.setup();
}

// Go around and around
void loop() {
  sketch.loop();
}

