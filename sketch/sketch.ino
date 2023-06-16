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
// - btnrgbflash.lgo
// - btnledflash.lgo
// - rgb.lgo
// - trafficlights.lgo

//#LOGO FILE=../logo/btnledflash.lgo NAME=sketch
static const char strings_sketch[] PROGMEM = {
// words
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
	"OLDVALUE\n"
	"VALUE\n"
};
static const short code_sketch[][INST_LENGTH] PROGMEM = {
	{ OPTYPE_BUILTIN, 2, 1 },		// 0
	{ OPTYPE_JUMP, 37, 0 },		// 1
	{ OPTYPE_HALT, 0, 0 },		// 2
	{ OPTYPE_INT, 13, 0 },		// 3
	{ OPTYPE_RETURN, 0, 0 },		// 4
	{ OPTYPE_INT, 7, 0 },		// 5
	{ OPTYPE_RETURN, 0, 0 },		// 6
	{ OPTYPE_BUILTIN, 23, 1 },		// 7
	{ OPTYPE_JUMP, 3, 0 },		// 8
	{ OPTYPE_BUILTIN, 22, 1 },		// 9
	{ OPTYPE_JUMP, 3, 0 },		// 10
	{ OPTYPE_BUILTIN, 24, 1 },		// 11
	{ OPTYPE_JUMP, 5, 0 },		// 12
	{ OPTYPE_RETURN, 0, 0 },		// 13
	{ OPTYPE_BUILTIN, 21, 1 },		// 14
	{ OPTYPE_JUMP, 3, 0 },		// 15
	{ OPTYPE_RETURN, 0, 0 },		// 16
	{ OPTYPE_BUILTIN, 22, 1 },		// 17
	{ OPTYPE_JUMP, 3, 0 },		// 18
	{ OPTYPE_RETURN, 0, 0 },		// 19
	{ OPTYPE_BUILTIN, 20, 1 },		// 20
	{ OPTYPE_JUMP, 5, 0 },		// 21
	{ OPTYPE_RETURN, 0, 0 },		// 22
	{ OPTYPE_JUMP, 14, 0 },		// 23
	{ OPTYPE_BUILTIN, 6, 1 },		// 24
	{ OPTYPE_INT, 1000, 0 },		// 25
	{ OPTYPE_JUMP, 17, 0 },		// 26
	{ OPTYPE_RETURN, 0, 0 },		// 27
	{ OPTYPE_BUILTIN, 1, 1 },		// 28
	{ OPTYPE_STRING, 9, 8 },		// 29
	{ OPTYPE_REF, 10, 5 },		// 30
	{ OPTYPE_BUILTIN, 5, 1 },		// 31
	{ OPTYPE_REF, 10, 5 },		// 32
	{ OPTYPE_BUILTIN, 7, 1 },		// 33
	{ OPTYPE_INT, 1, 0 },		// 34
	{ OPTYPE_JUMP, 23, 0 },		// 35
	{ OPTYPE_RETURN, 0, 0 },		// 36
	{ OPTYPE_BUILTIN, 1, 1 },		// 37
	{ OPTYPE_STRING, 10, 5 },		// 38
	{ OPTYPE_JUMP, 20, 0 },		// 39
	{ OPTYPE_BUILTIN, 5, 1 },		// 40
	{ OPTYPE_REF, 10, 5 },		// 41
	{ OPTYPE_BUILTIN, 8, 1 },		// 42
	{ OPTYPE_REF, 9, 8 },		// 43
	{ OPTYPE_JUMP, 28, 0 },		// 44
	{ OPTYPE_RETURN, 0, 0 },		// 45
	{ SCOPTYPE_WORD, 3, 0 }, 
	{ SCOPTYPE_WORD, 5, 0 }, 
	{ SCOPTYPE_WORD, 7, 0 }, 
	{ SCOPTYPE_WORD, 14, 0 }, 
	{ SCOPTYPE_WORD, 17, 0 }, 
	{ SCOPTYPE_WORD, 20, 0 }, 
	{ SCOPTYPE_WORD, 23, 0 }, 
	{ SCOPTYPE_WORD, 28, 0 }, 
	{ SCOPTYPE_WORD, 37, 0 }, 
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

