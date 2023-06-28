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
// - rgb.lgo
// - trafficlights.lgo
// - sos.lgo
// - tiny.lgo

//#LOGO FILE=../logo/btnrgbflash.lgo NAME=sketch
static const char strings_sketch[] PROGMEM = {
// words
	"ID\n"
	"LEDPIN\n"
	"BTNPIN\n"
	"SETUP\n"
	"ON\n"
	"OFF\n"
	"BUTTON\n"
	"FLASH\n"
	"TESTPRESS\n"
	"TESTSTATE\n"
	"TESTBTN\n"
	"RUN\n"
// variables
// strings
	"btnrgbflash\n"
	"ON\n"
	"OFF\n"
	"PRESS\n"
	"OLDVALUE\n"
	"VALUE\n"
};
static const short code_sketch[][INST_LENGTH] PROGMEM = {
	{ OPTYPE_JUMP, 65, 0 },		// 0
	{ OPTYPE_HALT, 0, 0 },		// 1
	{ OPTYPE_BUILTIN, 18, 0 },		// 2
	{ OPTYPE_STRING, 12, 11 },		// 3
	{ OPTYPE_RETURN, 0, 0 },		// 4
	{ OPTYPE_INT, 2, 0 },		// 5
	{ OPTYPE_RETURN, 0, 0 },		// 6
	{ OPTYPE_INT, 7, 0 },		// 7
	{ OPTYPE_RETURN, 0, 0 },		// 8
	{ OPTYPE_BUILTIN, 23, 0 },		// 9
	{ OPTYPE_JUMP, 5, 0 },		// 10
	{ OPTYPE_BUILTIN, 25, 0 },		// 11
	{ OPTYPE_JUMP, 5, 0 },		// 12
	{ OPTYPE_INT, 255, 0 },		// 13
	{ OPTYPE_BUILTIN, 24, 0 },		// 14
	{ OPTYPE_JUMP, 7, 0 },		// 15
	{ OPTYPE_RETURN, 0, 0 },		// 16
	{ OPTYPE_BUILTIN, 18, 0 },		// 17
	{ OPTYPE_STRING, 4, 2 },		// 18
	{ OPTYPE_BUILTIN, 25, 0 },		// 19
	{ OPTYPE_JUMP, 5, 0 },		// 20
	{ OPTYPE_INT, 0, 0 },		// 21
	{ OPTYPE_RETURN, 0, 0 },		// 22
	{ OPTYPE_BUILTIN, 18, 0 },		// 23
	{ OPTYPE_STRING, 5, 3 },		// 24
	{ OPTYPE_BUILTIN, 25, 0 },		// 25
	{ OPTYPE_JUMP, 5, 0 },		// 26
	{ OPTYPE_INT, 255, 0 },		// 27
	{ OPTYPE_RETURN, 0, 0 },		// 28
	{ OPTYPE_BUILTIN, 20, 0 },		// 29
	{ OPTYPE_JUMP, 7, 0 },		// 30
	{ OPTYPE_RETURN, 0, 0 },		// 31
	{ OPTYPE_JUMP, 17, 0 },		// 32
	{ OPTYPE_BUILTIN, 6, 0 },		// 33
	{ OPTYPE_INT, 1000, 0 },		// 34
	{ OPTYPE_JUMP, 23, 0 },		// 35
	{ OPTYPE_RETURN, 0, 0 },		// 36
	{ OPTYPE_BUILTIN, 5, 0 },		// 37
	{ OPTYPE_REF, 15, 5 },		// 38
	{ OPTYPE_BUILTIN, 7, 0 },		// 39
	{ OPTYPE_INT, 1, 0 },		// 40
	{ OPTYPE_JUMP, 32, 0 },		// 41
	{ OPTYPE_BUILTIN, 1, 0 },		// 42
	{ OPTYPE_STRING, 15, 5 },		// 43
	{ OPTYPE_INT, 0, 0 },		// 44
	{ OPTYPE_RETURN, 0, 0 },		// 45
	{ OPTYPE_BUILTIN, 1, 0 },		// 46
	{ OPTYPE_STRING, 16, 8 },		// 47
	{ OPTYPE_REF, 17, 5 },		// 48
	{ OPTYPE_BUILTIN, 5, 0 },		// 49
	{ OPTYPE_REF, 17, 5 },		// 50
	{ OPTYPE_BUILTIN, 7, 0 },		// 51
	{ OPTYPE_INT, 1, 0 },		// 52
	{ OPTYPE_JUMP, 32, 0 },		// 53
	{ OPTYPE_RETURN, 0, 0 },		// 54
	{ OPTYPE_BUILTIN, 1, 0 },		// 55
	{ OPTYPE_STRING, 17, 5 },		// 56
	{ OPTYPE_JUMP, 29, 0 },		// 57
	{ OPTYPE_BUILTIN, 5, 0 },		// 58
	{ OPTYPE_REF, 17, 5 },		// 59
	{ OPTYPE_BUILTIN, 8, 0 },		// 60
	{ OPTYPE_REF, 16, 8 },		// 61
	{ OPTYPE_JUMP, 46, 0 },		// 62
	{ OPTYPE_JUMP, 37, 0 },		// 63
	{ OPTYPE_RETURN, 0, 0 },		// 64
	{ OPTYPE_BUILTIN, 2, 0 },		// 65
	{ OPTYPE_JUMP, 55, 0 },		// 66
	{ OPTYPE_RETURN, 0, 0 },		// 67
	{ SCOPTYPE_WORD, 2, 0 }, 
	{ SCOPTYPE_WORD, 5, 0 }, 
	{ SCOPTYPE_WORD, 7, 0 }, 
	{ SCOPTYPE_WORD, 9, 0 }, 
	{ SCOPTYPE_WORD, 17, 0 }, 
	{ SCOPTYPE_WORD, 23, 0 }, 
	{ SCOPTYPE_WORD, 29, 0 }, 
	{ SCOPTYPE_WORD, 32, 0 }, 
	{ SCOPTYPE_WORD, 37, 0 }, 
	{ SCOPTYPE_WORD, 46, 0 }, 
	{ SCOPTYPE_WORD, 55, 0 }, 
	{ SCOPTYPE_WORD, 65, 0 }, 
	{ SCOPTYPE_END, 0, 0 } 
};
//#LOGO ENDFILE

//LogoSketch sketch(strings_sketch, (const PROGMEM short *)code_sketch);
LogoSketch sketch((char *)strings_sketch, (const PROGMEM short *)code_sketch);

// At the start
void setup() {
  sketch.setup();
}

// Go around and around
void loop() {
  sketch.loop();
}

