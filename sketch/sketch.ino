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
//    for a normal AVR RED=3 GREEN=4 BLUE=2
//    for an ESP32 RED=26 GREEN=25 BLUE=27
// - trafficlights.lgo
// - sos.lgo
// - tiny.lgo
// - ledbot.lgo

//#LOGO FILE=../logo/ledbot.lgo NAME=sketch
static const char strings_sketch[] PROGMEM = {
// words
	"ID\n"
	"SETUP\n"
	"ON\n"
	"OFF\n"
	"OUTSTEP\n"
	"FADEOUT\n"
	"INSTEP\n"
	"FADEIN\n"
// variables
// strings
	"ledbot\n"
	"TIME\n"
	"VAL\n"
	"VAL\n"
	"STEP\n"
	"COUNT\n"
	"PAUSE\n"
	"VAL\n"
	"VAL\n"
	"STEP\n"
	"COUNT\n"
	"PAUSE\n"
};
static const short code_sketch[][INST_LENGTH] PROGMEM = {
	{ OPTYPE_NOOP, 0, 0 },		// 0
	{ OPTYPE_HALT, 0, 0 },		// 1
	{ OPTYPE_BUILTIN, 18, 0 },		// 2
	{ OPTYPE_STRING, 8, 6 },		// 3
	{ OPTYPE_RETURN, 0, 0 },		// 4
	{ OPTYPE_BUILTIN, 1, 0 },		// 5
	{ OPTYPE_STRING, 9, 4 },		// 6
	{ OPTYPE_INT, 500, 0 },		// 7
	{ OPTYPE_BUILTIN, 33, 0 },		// 8
	{ OPTYPE_INT, 10, 0 },		// 9
	{ OPTYPE_INT, 1, 0 },		// 10
	{ OPTYPE_BUILTIN, 34, 0 },		// 11
	{ OPTYPE_INT, 1, 0 },		// 12
	{ OPTYPE_INT, 255, 0 },		// 13
	{ OPTYPE_RETURN, 0, 0 },		// 14
	{ OPTYPE_BUILTIN, 34, 0 },		// 15
	{ OPTYPE_INT, 1, 0 },		// 16
	{ OPTYPE_INT, 0, 0 },		// 17
	{ OPTYPE_RETURN, 0, 0 },		// 18
	{ OPTYPE_BUILTIN, 34, 0 },		// 19
	{ OPTYPE_INT, 1, 0 },		// 20
	{ OPTYPE_INT, 255, 0 },		// 21
	{ OPTYPE_RETURN, 0, 0 },		// 22
	{ OPTYPE_BUILTIN, 6, 0 },		// 23
	{ OPTYPE_GSTART, 0, 0 },		// 24
	{ OPTYPE_REF, 14, 5 },		// 25
	{ OPTYPE_BUILTIN, 9, 0 },		// 26
	{ OPTYPE_INT, 5, 0 },		// 27
	{ OPTYPE_GEND, 0, 0 },		// 28
	{ OPTYPE_BUILTIN, 1, 0 },		// 29
	{ OPTYPE_STRING, 10, 3 },		// 30
	{ OPTYPE_GSTART, 0, 0 },		// 31
	{ OPTYPE_REF, 10, 3 },		// 32
	{ OPTYPE_BUILTIN, 10, 0 },		// 33
	{ OPTYPE_REF, 12, 4 },		// 34
	{ OPTYPE_GEND, 0, 0 },		// 35
	{ OPTYPE_BUILTIN, 34, 0 },		// 36
	{ OPTYPE_INT, 1, 0 },		// 37
	{ OPTYPE_REF, 10, 3 },		// 38
	{ OPTYPE_RETURN, 0, 0 },		// 39
	{ OPTYPE_BUILTIN, 1, 0 },		// 40
	{ OPTYPE_STRING, 10, 3 },		// 41
	{ OPTYPE_INT, 0, 0 },		// 42
	{ OPTYPE_BUILTIN, 1, 0 },		// 43
	{ OPTYPE_STRING, 12, 4 },		// 44
	{ OPTYPE_INT, 5, 0 },		// 45
	{ OPTYPE_BUILTIN, 1, 0 },		// 46
	{ OPTYPE_STRING, 13, 5 },		// 47
	{ OPTYPE_BUILTIN, 45, 0 },		// 48
	{ OPTYPE_GSTART, 0, 0 },		// 49
	{ OPTYPE_INT, 255, 0 },		// 50
	{ OPTYPE_BUILTIN, 11, 0 },		// 51
	{ OPTYPE_REF, 12, 4 },		// 52
	{ OPTYPE_GEND, 0, 0 },		// 53
	{ OPTYPE_BUILTIN, 1, 0 },		// 54
	{ OPTYPE_STRING, 14, 5 },		// 55
	{ OPTYPE_BUILTIN, 45, 0 },		// 56
	{ OPTYPE_GSTART, 0, 0 },		// 57
	{ OPTYPE_REF, 9, 4 },		// 58
	{ OPTYPE_BUILTIN, 11, 0 },		// 59
	{ OPTYPE_REF, 13, 5 },		// 60
	{ OPTYPE_GEND, 0, 0 },		// 61
	{ OPTYPE_BUILTIN, 34, 0 },		// 62
	{ OPTYPE_INT, 1, 0 },		// 63
	{ OPTYPE_INT, 0, 0 },		// 64
	{ OPTYPE_BUILTIN, 3, 0 },		// 65
	{ OPTYPE_REF, 13, 5 },		// 66
	{ OPTYPE_JUMP, 23, 0 },		// 67
	{ OPTYPE_BUILTIN, 34, 0 },		// 68
	{ OPTYPE_INT, 1, 0 },		// 69
	{ OPTYPE_INT, 255, 0 },		// 70
	{ OPTYPE_RETURN, 0, 0 },		// 71
	{ OPTYPE_BUILTIN, 6, 0 },		// 72
	{ OPTYPE_GSTART, 0, 0 },		// 73
	{ OPTYPE_REF, 14, 5 },		// 74
	{ OPTYPE_BUILTIN, 9, 0 },		// 75
	{ OPTYPE_INT, 5, 0 },		// 76
	{ OPTYPE_GEND, 0, 0 },		// 77
	{ OPTYPE_BUILTIN, 1, 0 },		// 78
	{ OPTYPE_STRING, 10, 3 },		// 79
	{ OPTYPE_GSTART, 0, 0 },		// 80
	{ OPTYPE_REF, 10, 3 },		// 81
	{ OPTYPE_BUILTIN, 9, 0 },		// 82
	{ OPTYPE_REF, 12, 4 },		// 83
	{ OPTYPE_GEND, 0, 0 },		// 84
	{ OPTYPE_BUILTIN, 34, 0 },		// 85
	{ OPTYPE_INT, 1, 0 },		// 86
	{ OPTYPE_REF, 10, 3 },		// 87
	{ OPTYPE_RETURN, 0, 0 },		// 88
	{ OPTYPE_BUILTIN, 1, 0 },		// 89
	{ OPTYPE_STRING, 10, 3 },		// 90
	{ OPTYPE_INT, 255, 0 },		// 91
	{ OPTYPE_BUILTIN, 1, 0 },		// 92
	{ OPTYPE_STRING, 12, 4 },		// 93
	{ OPTYPE_INT, 5, 0 },		// 94
	{ OPTYPE_BUILTIN, 1, 0 },		// 95
	{ OPTYPE_STRING, 13, 5 },		// 96
	{ OPTYPE_BUILTIN, 45, 0 },		// 97
	{ OPTYPE_GSTART, 0, 0 },		// 98
	{ OPTYPE_INT, 255, 0 },		// 99
	{ OPTYPE_BUILTIN, 11, 0 },		// 100
	{ OPTYPE_REF, 12, 4 },		// 101
	{ OPTYPE_GEND, 0, 0 },		// 102
	{ OPTYPE_BUILTIN, 1, 0 },		// 103
	{ OPTYPE_STRING, 14, 5 },		// 104
	{ OPTYPE_BUILTIN, 45, 0 },		// 105
	{ OPTYPE_GSTART, 0, 0 },		// 106
	{ OPTYPE_REF, 9, 4 },		// 107
	{ OPTYPE_BUILTIN, 11, 0 },		// 108
	{ OPTYPE_REF, 13, 5 },		// 109
	{ OPTYPE_GEND, 0, 0 },		// 110
	{ OPTYPE_BUILTIN, 34, 0 },		// 111
	{ OPTYPE_INT, 1, 0 },		// 112
	{ OPTYPE_INT, 255, 0 },		// 113
	{ OPTYPE_BUILTIN, 3, 0 },		// 114
	{ OPTYPE_REF, 13, 5 },		// 115
	{ OPTYPE_JUMP, 72, 0 },		// 116
	{ OPTYPE_BUILTIN, 34, 0 },		// 117
	{ OPTYPE_INT, 1, 0 },		// 118
	{ OPTYPE_INT, 0, 0 },		// 119
	{ OPTYPE_RETURN, 0, 0 },		// 120
	{ SCOPTYPE_WORD, 2, 0 }, 
	{ SCOPTYPE_WORD, 5, 0 }, 
	{ SCOPTYPE_WORD, 15, 0 }, 
	{ SCOPTYPE_WORD, 19, 0 }, 
	{ SCOPTYPE_WORD, 23, 0 }, 
	{ SCOPTYPE_WORD, 40, 0 }, 
	{ SCOPTYPE_WORD, 72, 0 }, 
	{ SCOPTYPE_WORD, 89, 0 }, 
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

