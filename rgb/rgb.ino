/*
  Control 2 RGB LEDs in LOGO over serial or i2c
  
  Author: Paul Hamilton
  Date: 22-May-2023
  
  This work is licensed under the Creative Commons Attribution 4.0 International License. 
  To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/ or 
  send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  https://github.com/visualopsholdings/tinylogo
*/

#include "ringbuffer.hpp"
#include "cmd.hpp"
#include "logo.hpp"
#include "arduinotimeprovider.hpp"
#include "arduinoflashstring.hpp"
#include "arduinoflashcode.hpp"

#define LED_PIN     13
#define BLUE_PIN    2
#define RED_PIN     3
#define GREEN_PIN   4
#define BUTTON1_PIN 7
#define BUTTON2_PIN 6

void ledOn(Logo &logo) {
  digitalWrite(LED_PIN, HIGH);
}

void ledOff(Logo &logo) {
  digitalWrite(LED_PIN, LOW);
}

void redRaw(Logo &logo) {
  analogWrite(RED_PIN, logo.popint());
}

void greenRaw(Logo &logo) {
  analogWrite(GREEN_PIN, logo.popint());
}

void blueRaw(Logo &logo) {
  analogWrite(BLUE_PIN, logo.popint());
}

void button1(Logo &logo) {
  logo.pushint(digitalRead(BUTTON1_PIN));
}

void button2(Logo &logo) {
  logo.pushint(digitalRead(BUTTON2_PIN));
}

//#LOGO FILE=../logo/trafficlights.lgo NAME=rgb
static const char strings_rgb[] PROGMEM = {
	"SUBC\n"
	"DIVC\n"
	"SCLR\n"
	"REDC\n"
	"GREENC\n"
	"BLUEC\n"
	"SETALL\n"
	"AMBER\n"
	"RED\n"
	"GREEN\n"
	"BLUE\n"
	"OFF\n"
	"NEXTSTATE1\n"
	"FIRSTSTATE1\n"
	"NOTRED\n"
	"RESET\n"
	"PRESS1\n"
	"TESTST1\n"
	"TESTB1\n"
	"PRESS2\n"
	"TESTST2\n"
	"TESTB2\n"
	"TESTB\n"
	"C\n"
	"N\n"
	"B\n"
	"G\n"
	"R\n"
	"STATE1\n"
	"STATE1\n"
	"STATE1\n"
	"OLDVALUE1\n"
	"VALUE1\n"
	"OLDVALUE2\n"
	"VALUE2\n"
};
static const short code_rgb[][INST_LENGTH] PROGMEM = {
	{ OPTYPE_JUMP, 51, 0 },		// 0
	{ OPTYPE_BUILTIN, 2, 1 },		// 1
	{ OPTYPE_JUMP, 144, 0 },		// 2
	{ OPTYPE_HALT, 0, 0 },		// 3
	{ OPTYPE_POPREF, 0, 0 },		// 4
	{ OPTYPE_INT, 100, 0 },		// 5
	{ OPTYPE_BUILTIN, 9, 1 },		// 6
	{ OPTYPE_REF, 23, 1 },		// 7
	{ OPTYPE_RETURN, 0, 0 },		// 8
	{ OPTYPE_POPREF, 0, 0 },		// 9
	{ OPTYPE_JUMP, 4, 1 },		// 10
	{ OPTYPE_REF, 23, 1 },		// 11
	{ OPTYPE_BUILTIN, 11, 1 },		// 12
	{ OPTYPE_INT, 100, 0 },		// 13
	{ OPTYPE_RETURN, 0, 0 },		// 14
	{ OPTYPE_POPREF, 0, 0 },		// 15
	{ OPTYPE_JUMP, 9, 1 },		// 16
	{ OPTYPE_REF, 23, 1 },		// 17
	{ OPTYPE_BUILTIN, 12, 1 },		// 18
	{ OPTYPE_INT, 255, 0 },		// 19
	{ OPTYPE_RETURN, 0, 0 },		// 20
	{ OPTYPE_POPREF, 1, 0 },		// 21
	{ OPTYPE_BUILTIN, 0, 0 },		// 22
	{ OPTYPE_JUMP, 15, 1 },		// 23
	{ OPTYPE_REF, 24, 1 },		// 24
	{ OPTYPE_RETURN, 0, 0 },		// 25
	{ OPTYPE_POPREF, 1, 0 },		// 26
	{ OPTYPE_BUILTIN, 1, 0 },		// 27
	{ OPTYPE_JUMP, 15, 1 },		// 28
	{ OPTYPE_REF, 24, 1 },		// 29
	{ OPTYPE_RETURN, 0, 0 },		// 30
	{ OPTYPE_POPREF, 1, 0 },		// 31
	{ OPTYPE_BUILTIN, 2, 0 },		// 32
	{ OPTYPE_JUMP, 15, 1 },		// 33
	{ OPTYPE_REF, 24, 1 },		// 34
	{ OPTYPE_RETURN, 0, 0 },		// 35
	{ OPTYPE_POPREF, 2, 0 },		// 36
	{ OPTYPE_POPREF, 3, 0 },		// 37
	{ OPTYPE_POPREF, 4, 0 },		// 38
	{ OPTYPE_JUMP, 21, 1 },		// 39
	{ OPTYPE_REF, 27, 1 },		// 40
	{ OPTYPE_JUMP, 26, 1 },		// 41
	{ OPTYPE_REF, 26, 1 },		// 42
	{ OPTYPE_JUMP, 31, 1 },		// 43
	{ OPTYPE_REF, 25, 1 },		// 44
	{ OPTYPE_RETURN, 0, 0 },		// 45
	{ OPTYPE_JUMP, 36, 3 },		// 46
	{ OPTYPE_INT, 100, 0 },		// 47
	{ OPTYPE_INT, 75, 0 },		// 48
	{ OPTYPE_INT, 0, 0 },		// 49
	{ OPTYPE_RETURN, 0, 0 },		// 50
	{ OPTYPE_JUMP, 36, 3 },		// 51
	{ OPTYPE_INT, 100, 0 },		// 52
	{ OPTYPE_INT, 0, 0 },		// 53
	{ OPTYPE_INT, 0, 0 },		// 54
	{ OPTYPE_RETURN, 0, 0 },		// 55
	{ OPTYPE_JUMP, 36, 3 },		// 56
	{ OPTYPE_INT, 0, 0 },		// 57
	{ OPTYPE_INT, 100, 0 },		// 58
	{ OPTYPE_INT, 0, 0 },		// 59
	{ OPTYPE_RETURN, 0, 0 },		// 60
	{ OPTYPE_JUMP, 36, 3 },		// 61
	{ OPTYPE_INT, 0, 0 },		// 62
	{ OPTYPE_INT, 0, 0 },		// 63
	{ OPTYPE_INT, 100, 0 },		// 64
	{ OPTYPE_RETURN, 0, 0 },		// 65
	{ OPTYPE_JUMP, 36, 3 },		// 66
	{ OPTYPE_INT, 0, 0 },		// 67
	{ OPTYPE_INT, 0, 0 },		// 68
	{ OPTYPE_INT, 0, 0 },		// 69
	{ OPTYPE_RETURN, 0, 0 },		// 70
	{ OPTYPE_REF, 28, 6 },		// 71
	{ OPTYPE_BUILTIN, 10, 1 },		// 72
	{ OPTYPE_INT, 1, 0 },		// 73
	{ OPTYPE_RETURN, 0, 0 },		// 74
	{ OPTYPE_BUILTIN, 1, 1 },		// 75
	{ OPTYPE_STRING, 28, 6 },		// 76
	{ OPTYPE_INT, 0, 0 },		// 77
	{ OPTYPE_RETURN, 0, 0 },		// 78
	{ OPTYPE_BUILTIN, 4, 1 },		// 79
	{ OPTYPE_REF, 28, 6 },		// 80
	{ OPTYPE_BUILTIN, 7, 1 },		// 81
	{ OPTYPE_INT, 1, 0 },		// 82
	{ OPTYPE_JUMP, 56, 0 },		// 83
	{ OPTYPE_JUMP, 46, 0 },		// 84
	{ OPTYPE_RETURN, 0, 0 },		// 85
	{ OPTYPE_BUILTIN, 1, 1 },		// 86
	{ OPTYPE_STRING, 28, 6 },		// 87
	{ OPTYPE_JUMP, 75, 0 },		// 88
	{ OPTYPE_JUMP, 51, 0 },		// 89
	{ OPTYPE_RETURN, 0, 0 },		// 90
	{ OPTYPE_BUILTIN, 1, 1 },		// 91
	{ OPTYPE_STRING, 28, 6 },		// 92
	{ OPTYPE_JUMP, 71, 0 },		// 93
	{ OPTYPE_BUILTIN, 5, 1 },		// 94
	{ OPTYPE_REF, 28, 6 },		// 95
	{ OPTYPE_BUILTIN, 13, 1 },		// 96
	{ OPTYPE_INT, 2, 0 },		// 97
	{ OPTYPE_JUMP, 75, 0 },		// 98
	{ OPTYPE_BUILTIN, 4, 1 },		// 99
	{ OPTYPE_REF, 28, 6 },		// 100
	{ OPTYPE_BUILTIN, 7, 1 },		// 101
	{ OPTYPE_INT, 0, 0 },		// 102
	{ OPTYPE_JUMP, 51, 0 },		// 103
	{ OPTYPE_JUMP, 79, 0 },		// 104
	{ OPTYPE_RETURN, 0, 0 },		// 105
	{ OPTYPE_BUILTIN, 1, 1 },		// 106
	{ OPTYPE_STRING, 31, 9 },		// 107
	{ OPTYPE_REF, 32, 6 },		// 108
	{ OPTYPE_BUILTIN, 5, 1 },		// 109
	{ OPTYPE_REF, 32, 6 },		// 110
	{ OPTYPE_BUILTIN, 7, 1 },		// 111
	{ OPTYPE_INT, 1, 0 },		// 112
	{ OPTYPE_JUMP, 91, 0 },		// 113
	{ OPTYPE_RETURN, 0, 0 },		// 114
	{ OPTYPE_BUILTIN, 1, 1 },		// 115
	{ OPTYPE_STRING, 32, 6 },		// 116
	{ OPTYPE_BUILTIN, 3, 0 },		// 117
	{ OPTYPE_BUILTIN, 5, 1 },		// 118
	{ OPTYPE_REF, 32, 6 },		// 119
	{ OPTYPE_BUILTIN, 8, 1 },		// 120
	{ OPTYPE_REF, 31, 9 },		// 121
	{ OPTYPE_JUMP, 106, 0 },		// 122
	{ OPTYPE_RETURN, 0, 0 },		// 123
	{ OPTYPE_JUMP, 86, 0 },		// 124
	{ OPTYPE_RETURN, 0, 0 },		// 125
	{ OPTYPE_BUILTIN, 1, 1 },		// 126
	{ OPTYPE_STRING, 33, 9 },		// 127
	{ OPTYPE_REF, 34, 6 },		// 128
	{ OPTYPE_BUILTIN, 5, 1 },		// 129
	{ OPTYPE_REF, 34, 6 },		// 130
	{ OPTYPE_BUILTIN, 7, 1 },		// 131
	{ OPTYPE_INT, 1, 0 },		// 132
	{ OPTYPE_JUMP, 124, 0 },		// 133
	{ OPTYPE_RETURN, 0, 0 },		// 134
	{ OPTYPE_BUILTIN, 1, 1 },		// 135
	{ OPTYPE_STRING, 34, 6 },		// 136
	{ OPTYPE_BUILTIN, 4, 0 },		// 137
	{ OPTYPE_BUILTIN, 5, 1 },		// 138
	{ OPTYPE_REF, 34, 6 },		// 139
	{ OPTYPE_BUILTIN, 8, 1 },		// 140
	{ OPTYPE_REF, 33, 9 },		// 141
	{ OPTYPE_JUMP, 126, 0 },		// 142
	{ OPTYPE_RETURN, 0, 0 },		// 143
	{ OPTYPE_JUMP, 115, 0 },		// 144
	{ OPTYPE_JUMP, 135, 0 },		// 145
	{ OPTYPE_RETURN, 0, 0 },		// 146
	{ SCOPTYPE_WORD, 4, 1 }, 
	{ SCOPTYPE_WORD, 9, 1 }, 
	{ SCOPTYPE_WORD, 15, 1 }, 
	{ SCOPTYPE_WORD, 21, 1 }, 
	{ SCOPTYPE_WORD, 26, 1 }, 
	{ SCOPTYPE_WORD, 31, 1 }, 
	{ SCOPTYPE_WORD, 36, 3 }, 
	{ SCOPTYPE_WORD, 46, 0 }, 
	{ SCOPTYPE_WORD, 51, 0 }, 
	{ SCOPTYPE_WORD, 56, 0 }, 
	{ SCOPTYPE_WORD, 61, 0 }, 
	{ SCOPTYPE_WORD, 66, 0 }, 
	{ SCOPTYPE_WORD, 71, 0 }, 
	{ SCOPTYPE_WORD, 75, 0 }, 
	{ SCOPTYPE_WORD, 79, 0 }, 
	{ SCOPTYPE_WORD, 86, 0 }, 
	{ SCOPTYPE_WORD, 91, 0 }, 
	{ SCOPTYPE_WORD, 106, 0 }, 
	{ SCOPTYPE_WORD, 115, 0 }, 
	{ SCOPTYPE_WORD, 124, 0 }, 
	{ SCOPTYPE_WORD, 126, 0 }, 
	{ SCOPTYPE_WORD, 135, 0 }, 
	{ SCOPTYPE_WORD, 144, 0 }, 
	{ SCOPTYPE_VAR, 0, 0 }, 
	{ SCOPTYPE_VAR, 0, 0 }, 
	{ SCOPTYPE_VAR, 0, 0 }, 
	{ SCOPTYPE_VAR, 0, 0 }, 
	{ SCOPTYPE_VAR, 0, 0 }, 
	{ SCOPTYPE_END, 0, 0 } 
};
//#LOGO ENDFILE

LogoBuiltinWord builtins[] = {
  { "REDR", &redRaw, 1 },
  { "GREENR", &greenRaw, 1 },
  { "BLUER", &blueRaw, 1 },
  { "BUTTON1", &button1, 0 },
  { "BUTTON2", &button2, 0 },
};
ArduinoTimeProvider time;
ArduinoFlashString strings(strings_rgb);
ArduinoFlashCode code((const PROGMEM short *)code_rgb);
LogoFunctionPrimitives primitives(builtins, sizeof(builtins));
Logo logo(&primitives, &time, Logo::core, &strings, &code);

void showErr(int mode, int n) {

  Serial.print("err ");
  Serial.print(mode);
  Serial.print(", ");
  Serial.println(n);

}

// At the start
void setup() {

  // Setup the LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // the RGB pins
  pinMode(RED_PIN, OUTPUT);
  analogWrite(RED_PIN, 255);
  pinMode(BLUE_PIN, OUTPUT);
  analogWrite(BLUE_PIN, 255);
  pinMode(GREEN_PIN, OUTPUT);
  analogWrite(GREEN_PIN, 255);
 
  // the button pin
  pinMode(BUTTON1_PIN, INPUT);
  pinMode(BUTTON2_PIN, INPUT);

  Serial.begin(9600);
 
}

// Go around and around
void loop() {

  int err = logo.step();
  if (err && err != LG_STOP) {
    showErr(2, err);
  }
  
}

