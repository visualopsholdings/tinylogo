/*
  A holder for a logo sketch.
  
  Author: Paul Hamilton
  Date: 14-Jun-2023
  
  This work is licensed under the Creative Commons Attribution 4.0 International License. 
  To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/ or 
  send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  https://github.com/visualopsholdings/tinylogo
*/

#include "logosketch.hpp"

// change this line to set the logo program you want to upload
// Other files are
// - ledflash1.lgo
// - btnrgbflash.lgo
// - btnledflash.lgo
// - rgb.lgo
// - trafficlights.lgo

//#LOGO FILE=../logo/btnledflash.lgo NAME=sketch
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
	"TESTSTATE\n"
	"TESTBTN\n"
// variables
// strings
	"btnledflash\n"
	"OLDVALUE\n"
	"VALUE\n"
};
static const short code_sketch[][INST_LENGTH] PROGMEM = {
	{ OPTYPE_BUILTIN, 2, 1 },		// 0
	{ OPTYPE_JUMP, 40, 0 },		// 1
	{ OPTYPE_HALT, 0, 0 },		// 2
	{ OPTYPE_BUILTIN, 18, 1 },		// 3
	{ OPTYPE_STRING, 10, 11 },		// 4
	{ OPTYPE_RETURN, 0, 0 },		// 5
	{ OPTYPE_INT, 13, 0 },		// 6
	{ OPTYPE_RETURN, 0, 0 },		// 7
	{ OPTYPE_INT, 7, 0 },		// 8
	{ OPTYPE_RETURN, 0, 0 },		// 9
	{ OPTYPE_BUILTIN, 23, 1 },		// 10
	{ OPTYPE_JUMP, 6, 0 },		// 11
	{ OPTYPE_BUILTIN, 22, 1 },		// 12
	{ OPTYPE_JUMP, 6, 0 },		// 13
	{ OPTYPE_BUILTIN, 24, 1 },		// 14
	{ OPTYPE_JUMP, 8, 0 },		// 15
	{ OPTYPE_RETURN, 0, 0 },		// 16
	{ OPTYPE_BUILTIN, 21, 1 },		// 17
	{ OPTYPE_JUMP, 6, 0 },		// 18
	{ OPTYPE_RETURN, 0, 0 },		// 19
	{ OPTYPE_BUILTIN, 22, 1 },		// 20
	{ OPTYPE_JUMP, 6, 0 },		// 21
	{ OPTYPE_RETURN, 0, 0 },		// 22
	{ OPTYPE_BUILTIN, 20, 1 },		// 23
	{ OPTYPE_JUMP, 8, 0 },		// 24
	{ OPTYPE_RETURN, 0, 0 },		// 25
	{ OPTYPE_JUMP, 17, 0 },		// 26
	{ OPTYPE_BUILTIN, 6, 1 },		// 27
	{ OPTYPE_INT, 1000, 0 },		// 28
	{ OPTYPE_JUMP, 20, 0 },		// 29
	{ OPTYPE_RETURN, 0, 0 },		// 30
	{ OPTYPE_BUILTIN, 1, 1 },		// 31
	{ OPTYPE_STRING, 11, 8 },		// 32
	{ OPTYPE_REF, 12, 5 },		// 33
	{ OPTYPE_BUILTIN, 5, 1 },		// 34
	{ OPTYPE_REF, 12, 5 },		// 35
	{ OPTYPE_BUILTIN, 7, 1 },		// 36
	{ OPTYPE_INT, 1, 0 },		// 37
	{ OPTYPE_JUMP, 26, 0 },		// 38
	{ OPTYPE_RETURN, 0, 0 },		// 39
	{ OPTYPE_BUILTIN, 1, 1 },		// 40
	{ OPTYPE_STRING, 12, 5 },		// 41
	{ OPTYPE_JUMP, 23, 0 },		// 42
	{ OPTYPE_BUILTIN, 5, 1 },		// 43
	{ OPTYPE_REF, 12, 5 },		// 44
	{ OPTYPE_BUILTIN, 8, 1 },		// 45
	{ OPTYPE_REF, 11, 8 },		// 46
	{ OPTYPE_JUMP, 31, 0 },		// 47
	{ OPTYPE_RETURN, 0, 0 },		// 48
	{ SCOPTYPE_WORD, 3, 0 }, 
	{ SCOPTYPE_WORD, 6, 0 }, 
	{ SCOPTYPE_WORD, 8, 0 }, 
	{ SCOPTYPE_WORD, 10, 0 }, 
	{ SCOPTYPE_WORD, 17, 0 }, 
	{ SCOPTYPE_WORD, 20, 0 }, 
	{ SCOPTYPE_WORD, 23, 0 }, 
	{ SCOPTYPE_WORD, 26, 0 }, 
	{ SCOPTYPE_WORD, 31, 0 }, 
	{ SCOPTYPE_WORD, 40, 0 }, 
	{ SCOPTYPE_END, 0, 0 } 
};
//#LOGO ENDFILE

LogoSketch sketch(strings_sketch, (const PROGMEM short *)code_sketch);

// At the start
void setup() {
  sketch.setup();
}

// Go around and around
void loop() {
  sketch.loop();
}

