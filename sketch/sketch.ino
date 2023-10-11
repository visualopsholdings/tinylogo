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
	"DOFADEOUT\n"
	"INSTEP\n"
	"DOFADEIN\n"
	"LONGFADEIN\n"
	"LONGFADEOUT\n"
	"FADEIN\n"
	"FADEOUT\n"
// variables
	"TIME\n"
// strings
	"ledbot\n"
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
	{ OPTYPE_STRING, 13, 6 },		// 3
	{ OPTYPE_RETURN, 0, 0 },		// 4
	{ OPTYPE_BUILTIN, 33, 0 },		// 5
	{ OPTYPE_INT, 10, 0 },		// 6
	{ OPTYPE_INT, 1, 0 },		// 7
	{ OPTYPE_BUILTIN, 34, 0 },		// 8
	{ OPTYPE_INT, 1, 0 },		// 9
	{ OPTYPE_INT, 255, 0 },		// 10
	{ OPTYPE_RETURN, 0, 0 },		// 11
	{ OPTYPE_BUILTIN, 34, 0 },		// 12
	{ OPTYPE_INT, 1, 0 },		// 13
	{ OPTYPE_INT, 0, 0 },		// 14
	{ OPTYPE_RETURN, 0, 0 },		// 15
	{ OPTYPE_BUILTIN, 34, 0 },		// 16
	{ OPTYPE_INT, 1, 0 },		// 17
	{ OPTYPE_INT, 255, 0 },		// 18
	{ OPTYPE_RETURN, 0, 0 },		// 19
	{ OPTYPE_BUILTIN, 6, 0 },		// 20
	{ OPTYPE_GSTART, 0, 0 },		// 21
	{ OPTYPE_REF, 18, 5 },		// 22
	{ OPTYPE_BUILTIN, 9, 0 },		// 23
	{ OPTYPE_INT, 5, 0 },		// 24
	{ OPTYPE_GEND, 0, 0 },		// 25
	{ OPTYPE_BUILTIN, 1, 0 },		// 26
	{ OPTYPE_STRING, 14, 3 },		// 27
	{ OPTYPE_GSTART, 0, 0 },		// 28
	{ OPTYPE_REF, 14, 3 },		// 29
	{ OPTYPE_BUILTIN, 10, 0 },		// 30
	{ OPTYPE_REF, 16, 4 },		// 31
	{ OPTYPE_GEND, 0, 0 },		// 32
	{ OPTYPE_BUILTIN, 34, 0 },		// 33
	{ OPTYPE_INT, 1, 0 },		// 34
	{ OPTYPE_REF, 14, 3 },		// 35
	{ OPTYPE_RETURN, 0, 0 },		// 36
	{ OPTYPE_POPREF, 0, 0 },		// 37
	{ OPTYPE_BUILTIN, 1, 0 },		// 38
	{ OPTYPE_STRING, 14, 3 },		// 39
	{ OPTYPE_INT, 0, 0 },		// 40
	{ OPTYPE_BUILTIN, 1, 0 },		// 41
	{ OPTYPE_STRING, 16, 4 },		// 42
	{ OPTYPE_INT, 5, 0 },		// 43
	{ OPTYPE_BUILTIN, 1, 0 },		// 44
	{ OPTYPE_STRING, 17, 5 },		// 45
	{ OPTYPE_BUILTIN, 45, 0 },		// 46
	{ OPTYPE_GSTART, 0, 0 },		// 47
	{ OPTYPE_INT, 255, 0 },		// 48
	{ OPTYPE_BUILTIN, 11, 0 },		// 49
	{ OPTYPE_REF, 16, 4 },		// 50
	{ OPTYPE_GEND, 0, 0 },		// 51
	{ OPTYPE_BUILTIN, 1, 0 },		// 52
	{ OPTYPE_STRING, 18, 5 },		// 53
	{ OPTYPE_BUILTIN, 45, 0 },		// 54
	{ OPTYPE_GSTART, 0, 0 },		// 55
	{ OPTYPE_REF, 12, 4 },		// 56
	{ OPTYPE_BUILTIN, 11, 0 },		// 57
	{ OPTYPE_REF, 17, 5 },		// 58
	{ OPTYPE_GEND, 0, 0 },		// 59
	{ OPTYPE_BUILTIN, 34, 0 },		// 60
	{ OPTYPE_INT, 1, 0 },		// 61
	{ OPTYPE_INT, 0, 0 },		// 62
	{ OPTYPE_BUILTIN, 3, 0 },		// 63
	{ OPTYPE_REF, 17, 5 },		// 64
	{ OPTYPE_JUMP, 20, 0 },		// 65
	{ OPTYPE_BUILTIN, 34, 0 },		// 66
	{ OPTYPE_INT, 1, 0 },		// 67
	{ OPTYPE_INT, 255, 0 },		// 68
	{ OPTYPE_RETURN, 0, 0 },		// 69
	{ OPTYPE_BUILTIN, 6, 0 },		// 70
	{ OPTYPE_GSTART, 0, 0 },		// 71
	{ OPTYPE_REF, 18, 5 },		// 72
	{ OPTYPE_BUILTIN, 9, 0 },		// 73
	{ OPTYPE_INT, 5, 0 },		// 74
	{ OPTYPE_GEND, 0, 0 },		// 75
	{ OPTYPE_BUILTIN, 1, 0 },		// 76
	{ OPTYPE_STRING, 14, 3 },		// 77
	{ OPTYPE_GSTART, 0, 0 },		// 78
	{ OPTYPE_REF, 14, 3 },		// 79
	{ OPTYPE_BUILTIN, 9, 0 },		// 80
	{ OPTYPE_REF, 16, 4 },		// 81
	{ OPTYPE_GEND, 0, 0 },		// 82
	{ OPTYPE_BUILTIN, 34, 0 },		// 83
	{ OPTYPE_INT, 1, 0 },		// 84
	{ OPTYPE_REF, 14, 3 },		// 85
	{ OPTYPE_RETURN, 0, 0 },		// 86
	{ OPTYPE_POPREF, 0, 0 },		// 87
	{ OPTYPE_BUILTIN, 1, 0 },		// 88
	{ OPTYPE_STRING, 14, 3 },		// 89
	{ OPTYPE_INT, 255, 0 },		// 90
	{ OPTYPE_BUILTIN, 1, 0 },		// 91
	{ OPTYPE_STRING, 16, 4 },		// 92
	{ OPTYPE_INT, 5, 0 },		// 93
	{ OPTYPE_BUILTIN, 1, 0 },		// 94
	{ OPTYPE_STRING, 17, 5 },		// 95
	{ OPTYPE_BUILTIN, 45, 0 },		// 96
	{ OPTYPE_GSTART, 0, 0 },		// 97
	{ OPTYPE_INT, 255, 0 },		// 98
	{ OPTYPE_BUILTIN, 11, 0 },		// 99
	{ OPTYPE_REF, 16, 4 },		// 100
	{ OPTYPE_GEND, 0, 0 },		// 101
	{ OPTYPE_BUILTIN, 1, 0 },		// 102
	{ OPTYPE_STRING, 18, 5 },		// 103
	{ OPTYPE_BUILTIN, 45, 0 },		// 104
	{ OPTYPE_GSTART, 0, 0 },		// 105
	{ OPTYPE_REF, 12, 4 },		// 106
	{ OPTYPE_BUILTIN, 11, 0 },		// 107
	{ OPTYPE_REF, 17, 5 },		// 108
	{ OPTYPE_GEND, 0, 0 },		// 109
	{ OPTYPE_BUILTIN, 34, 0 },		// 110
	{ OPTYPE_INT, 1, 0 },		// 111
	{ OPTYPE_INT, 255, 0 },		// 112
	{ OPTYPE_BUILTIN, 3, 0 },		// 113
	{ OPTYPE_REF, 17, 5 },		// 114
	{ OPTYPE_JUMP, 70, 0 },		// 115
	{ OPTYPE_BUILTIN, 34, 0 },		// 116
	{ OPTYPE_INT, 1, 0 },		// 117
	{ OPTYPE_INT, 0, 0 },		// 118
	{ OPTYPE_RETURN, 0, 0 },		// 119
	{ OPTYPE_JUMP, 87, 1 },		// 120
	{ OPTYPE_INT, 3000, 0 },		// 121
	{ OPTYPE_RETURN, 0, 0 },		// 122
	{ OPTYPE_JUMP, 37, 1 },		// 123
	{ OPTYPE_INT, 3000, 0 },		// 124
	{ OPTYPE_RETURN, 0, 0 },		// 125
	{ OPTYPE_JUMP, 87, 1 },		// 126
	{ OPTYPE_INT, 500, 0 },		// 127
	{ OPTYPE_RETURN, 0, 0 },		// 128
	{ OPTYPE_JUMP, 37, 1 },		// 129
	{ OPTYPE_INT, 500, 0 },		// 130
	{ OPTYPE_RETURN, 0, 0 },		// 131
	{ SCOPTYPE_WORD, 2, 0 }, 
	{ SCOPTYPE_WORD, 5, 0 }, 
	{ SCOPTYPE_WORD, 12, 0 }, 
	{ SCOPTYPE_WORD, 16, 0 }, 
	{ SCOPTYPE_WORD, 20, 0 }, 
	{ SCOPTYPE_WORD, 37, 1 }, 
	{ SCOPTYPE_WORD, 70, 0 }, 
	{ SCOPTYPE_WORD, 87, 1 }, 
	{ SCOPTYPE_WORD, 120, 0 }, 
	{ SCOPTYPE_WORD, 123, 0 }, 
	{ SCOPTYPE_WORD, 126, 0 }, 
	{ SCOPTYPE_WORD, 129, 0 }, 
	{ SCOPTYPE_VAR, 0, 0 }, 
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

