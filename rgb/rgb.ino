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

#define LED_PIN       13
#define BLUE1_PIN     2
#define RED1_PIN      3
#define GREEN1_PIN    4
#define BLUE2_PIN     9
#define RED2_PIN      10
#define GREEN2_PIN    11
#define BUTTON1_PIN   7
#define BUTTON2_PIN   6

void ledOn(Logo &logo) {
  digitalWrite(LED_PIN, HIGH);
}

void ledOff(Logo &logo) {
  digitalWrite(LED_PIN, LOW);
}

void red1Raw(Logo &logo) {
  analogWrite(RED1_PIN, logo.popint());
}

void green1Raw(Logo &logo) {
  analogWrite(GREEN1_PIN, logo.popint());
}

void blue1Raw(Logo &logo) {
  analogWrite(BLUE1_PIN, logo.popint());
}

void red2Raw(Logo &logo) {
  analogWrite(RED2_PIN, logo.popint());
}

void green2Raw(Logo &logo) {
  analogWrite(GREEN2_PIN, logo.popint());
}

void blue2Raw(Logo &logo) {
  analogWrite(BLUE2_PIN, logo.popint());
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
	"SET1\n"
	"SET2\n"
	"AMBER1\n"
	"RED1\n"
	"GREEN1\n"
	"RED2\n"
	"GREEN2\n"
	"OFF1\n"
	"NEXTSTATE1\n"
	"FIRSTSTATE1\n"
	"NOTRED1\n"
	"PRESS1\n"
	"TESTST1\n"
	"TESTB1\n"
	"OFF\n"
	"ON\n"
	"NOTSTATE2\n"
	"PRESS2\n"
	"TESTST2\n"
	"TESTB2\n"
	"TESTB\n"
	"START\n"
	"C\n"
	"B\n"
	"G\n"
	"R\n"
	"STATE1\n"
	"STATE1\n"
	"OLDVALUE1\n"
	"VALUE1\n"
	"STATE2\n"
	"OLDVALUE2\n"
	"VALUE2\n"
	"STATE2\n"
};
static const short code_rgb[][INST_LENGTH] PROGMEM = {
	{ OPTYPE_JUMP, 167, 0 },		// 0
	{ OPTYPE_HALT, 0, 0 },		// 1
	{ OPTYPE_POPREF, 0, 0 },		// 2
	{ OPTYPE_INT, 100, 0 },		// 3
	{ OPTYPE_BUILTIN, 9, 1 },		// 4
	{ OPTYPE_REF, 25, 1 },		// 5
	{ OPTYPE_RETURN, 0, 0 },		// 6
	{ OPTYPE_POPREF, 0, 0 },		// 7
	{ OPTYPE_JUMP, 2, 1 },		// 8
	{ OPTYPE_REF, 25, 1 },		// 9
	{ OPTYPE_BUILTIN, 11, 1 },		// 10
	{ OPTYPE_INT, 100, 0 },		// 11
	{ OPTYPE_RETURN, 0, 0 },		// 12
	{ OPTYPE_POPREF, 0, 0 },		// 13
	{ OPTYPE_JUMP, 7, 1 },		// 14
	{ OPTYPE_REF, 25, 1 },		// 15
	{ OPTYPE_BUILTIN, 12, 1 },		// 16
	{ OPTYPE_INT, 255, 0 },		// 17
	{ OPTYPE_RETURN, 0, 0 },		// 18
	{ OPTYPE_POPREF, 1, 0 },		// 19
	{ OPTYPE_POPREF, 2, 0 },		// 20
	{ OPTYPE_POPREF, 3, 0 },		// 21
	{ OPTYPE_BUILTIN, 0, 0 },		// 22
	{ OPTYPE_JUMP, 13, 1 },		// 23
	{ OPTYPE_REF, 28, 1 },		// 24
	{ OPTYPE_BUILTIN, 1, 0 },		// 25
	{ OPTYPE_JUMP, 13, 1 },		// 26
	{ OPTYPE_REF, 27, 1 },		// 27
	{ OPTYPE_BUILTIN, 2, 0 },		// 28
	{ OPTYPE_JUMP, 13, 1 },		// 29
	{ OPTYPE_REF, 26, 1 },		// 30
	{ OPTYPE_RETURN, 0, 0 },		// 31
	{ OPTYPE_POPREF, 1, 0 },		// 32
	{ OPTYPE_POPREF, 2, 0 },		// 33
	{ OPTYPE_POPREF, 3, 0 },		// 34
	{ OPTYPE_BUILTIN, 3, 0 },		// 35
	{ OPTYPE_JUMP, 13, 1 },		// 36
	{ OPTYPE_REF, 28, 1 },		// 37
	{ OPTYPE_BUILTIN, 4, 0 },		// 38
	{ OPTYPE_JUMP, 13, 1 },		// 39
	{ OPTYPE_REF, 27, 1 },		// 40
	{ OPTYPE_BUILTIN, 5, 0 },		// 41
	{ OPTYPE_JUMP, 13, 1 },		// 42
	{ OPTYPE_REF, 26, 1 },		// 43
	{ OPTYPE_RETURN, 0, 0 },		// 44
	{ OPTYPE_JUMP, 19, 3 },		// 45
	{ OPTYPE_INT, 100, 0 },		// 46
	{ OPTYPE_INT, 75, 0 },		// 47
	{ OPTYPE_INT, 0, 0 },		// 48
	{ OPTYPE_RETURN, 0, 0 },		// 49
	{ OPTYPE_JUMP, 19, 3 },		// 50
	{ OPTYPE_INT, 100, 0 },		// 51
	{ OPTYPE_INT, 0, 0 },		// 52
	{ OPTYPE_INT, 0, 0 },		// 53
	{ OPTYPE_RETURN, 0, 0 },		// 54
	{ OPTYPE_JUMP, 19, 3 },		// 55
	{ OPTYPE_INT, 0, 0 },		// 56
	{ OPTYPE_INT, 100, 0 },		// 57
	{ OPTYPE_INT, 0, 0 },		// 58
	{ OPTYPE_RETURN, 0, 0 },		// 59
	{ OPTYPE_JUMP, 32, 3 },		// 60
	{ OPTYPE_INT, 8, 0 },		// 61
	{ OPTYPE_INT, 2, 0 },		// 62
	{ OPTYPE_INT, 2, 0 },		// 63
	{ OPTYPE_RETURN, 0, 0 },		// 64
	{ OPTYPE_JUMP, 32, 3 },		// 65
	{ OPTYPE_INT, 2, 0 },		// 66
	{ OPTYPE_INT, 8, 0 },		// 67
	{ OPTYPE_INT, 2, 0 },		// 68
	{ OPTYPE_RETURN, 0, 0 },		// 69
	{ OPTYPE_JUMP, 19, 3 },		// 70
	{ OPTYPE_INT, 0, 0 },		// 71
	{ OPTYPE_INT, 0, 0 },		// 72
	{ OPTYPE_INT, 0, 0 },		// 73
	{ OPTYPE_RETURN, 0, 0 },		// 74
	{ OPTYPE_REF, 29, 6 },		// 75
	{ OPTYPE_BUILTIN, 10, 1 },		// 76
	{ OPTYPE_INT, 1, 0 },		// 77
	{ OPTYPE_RETURN, 0, 0 },		// 78
	{ OPTYPE_BUILTIN, 1, 1 },		// 79
	{ OPTYPE_STRING, 29, 6 },		// 80
	{ OPTYPE_INT, 0, 0 },		// 81
	{ OPTYPE_RETURN, 0, 0 },		// 82
	{ OPTYPE_BUILTIN, 4, 1 },		// 83
	{ OPTYPE_REF, 29, 6 },		// 84
	{ OPTYPE_BUILTIN, 7, 1 },		// 85
	{ OPTYPE_INT, 1, 0 },		// 86
	{ OPTYPE_JUMP, 55, 0 },		// 87
	{ OPTYPE_JUMP, 45, 0 },		// 88
	{ OPTYPE_RETURN, 0, 0 },		// 89
	{ OPTYPE_BUILTIN, 1, 1 },		// 90
	{ OPTYPE_STRING, 29, 6 },		// 91
	{ OPTYPE_JUMP, 75, 0 },		// 92
	{ OPTYPE_BUILTIN, 5, 1 },		// 93
	{ OPTYPE_REF, 29, 6 },		// 94
	{ OPTYPE_BUILTIN, 13, 1 },		// 95
	{ OPTYPE_INT, 2, 0 },		// 96
	{ OPTYPE_JUMP, 79, 0 },		// 97
	{ OPTYPE_BUILTIN, 4, 1 },		// 98
	{ OPTYPE_REF, 29, 6 },		// 99
	{ OPTYPE_BUILTIN, 7, 1 },		// 100
	{ OPTYPE_INT, 0, 0 },		// 101
	{ OPTYPE_JUMP, 50, 0 },		// 102
	{ OPTYPE_JUMP, 83, 0 },		// 103
	{ OPTYPE_RETURN, 0, 0 },		// 104
	{ OPTYPE_BUILTIN, 1, 1 },		// 105
	{ OPTYPE_STRING, 31, 9 },		// 106
	{ OPTYPE_REF, 32, 6 },		// 107
	{ OPTYPE_BUILTIN, 5, 1 },		// 108
	{ OPTYPE_REF, 32, 6 },		// 109
	{ OPTYPE_BUILTIN, 7, 1 },		// 110
	{ OPTYPE_INT, 1, 0 },		// 111
	{ OPTYPE_JUMP, 90, 0 },		// 112
	{ OPTYPE_RETURN, 0, 0 },		// 113
	{ OPTYPE_BUILTIN, 1, 1 },		// 114
	{ OPTYPE_STRING, 32, 6 },		// 115
	{ OPTYPE_BUILTIN, 6, 0 },		// 116
	{ OPTYPE_BUILTIN, 5, 1 },		// 117
	{ OPTYPE_REF, 32, 6 },		// 118
	{ OPTYPE_BUILTIN, 8, 1 },		// 119
	{ OPTYPE_REF, 31, 9 },		// 120
	{ OPTYPE_JUMP, 105, 0 },		// 121
	{ OPTYPE_RETURN, 0, 0 },		// 122
	{ OPTYPE_JUMP, 70, 0 },		// 123
	{ OPTYPE_JUMP, 60, 0 },		// 124
	{ OPTYPE_RETURN, 0, 0 },		// 125
	{ OPTYPE_JUMP, 50, 0 },		// 126
	{ OPTYPE_JUMP, 65, 0 },		// 127
	{ OPTYPE_RETURN, 0, 0 },		// 128
	{ OPTYPE_BUILTIN, 19, 1 },		// 129
	{ OPTYPE_REF, 33, 6 },		// 130
	{ OPTYPE_RETURN, 0, 0 },		// 131
	{ OPTYPE_BUILTIN, 1, 1 },		// 132
	{ OPTYPE_STRING, 33, 6 },		// 133
	{ OPTYPE_JUMP, 129, 0 },		// 134
	{ OPTYPE_BUILTIN, 4, 1 },		// 135
	{ OPTYPE_REF, 33, 6 },		// 136
	{ OPTYPE_BUILTIN, 7, 1 },		// 137
	{ OPTYPE_INT, 0, 0 },		// 138
	{ OPTYPE_JUMP, 123, 0 },		// 139
	{ OPTYPE_JUMP, 126, 0 },		// 140
	{ OPTYPE_RETURN, 0, 0 },		// 141
	{ OPTYPE_BUILTIN, 1, 1 },		// 142
	{ OPTYPE_STRING, 34, 9 },		// 143
	{ OPTYPE_REF, 35, 6 },		// 144
	{ OPTYPE_BUILTIN, 5, 1 },		// 145
	{ OPTYPE_REF, 35, 6 },		// 146
	{ OPTYPE_BUILTIN, 7, 1 },		// 147
	{ OPTYPE_INT, 1, 0 },		// 148
	{ OPTYPE_JUMP, 132, 0 },		// 149
	{ OPTYPE_RETURN, 0, 0 },		// 150
	{ OPTYPE_BUILTIN, 1, 1 },		// 151
	{ OPTYPE_STRING, 35, 6 },		// 152
	{ OPTYPE_BUILTIN, 7, 0 },		// 153
	{ OPTYPE_BUILTIN, 5, 1 },		// 154
	{ OPTYPE_REF, 35, 6 },		// 155
	{ OPTYPE_BUILTIN, 8, 1 },		// 156
	{ OPTYPE_REF, 34, 9 },		// 157
	{ OPTYPE_JUMP, 142, 0 },		// 158
	{ OPTYPE_RETURN, 0, 0 },		// 159
	{ OPTYPE_BUILTIN, 5, 1 },		// 160
	{ OPTYPE_REF, 33, 6 },		// 161
	{ OPTYPE_BUILTIN, 7, 1 },		// 162
	{ OPTYPE_INT, 1, 0 },		// 163
	{ OPTYPE_JUMP, 114, 0 },		// 164
	{ OPTYPE_JUMP, 151, 0 },		// 165
	{ OPTYPE_RETURN, 0, 0 },		// 166
	{ OPTYPE_BUILTIN, 1, 1 },		// 167
	{ OPTYPE_STRING, 33, 6 },		// 168
	{ OPTYPE_INT, 1, 0 },		// 169
	{ OPTYPE_JUMP, 65, 0 },		// 170
	{ OPTYPE_JUMP, 50, 0 },		// 171
	{ OPTYPE_BUILTIN, 2, 1 },		// 172
	{ OPTYPE_JUMP, 160, 0 },		// 173
	{ OPTYPE_RETURN, 0, 0 },		// 174
	{ SCOPTYPE_WORD, 2, 1 }, 
	{ SCOPTYPE_WORD, 7, 1 }, 
	{ SCOPTYPE_WORD, 13, 1 }, 
	{ SCOPTYPE_WORD, 19, 3 }, 
	{ SCOPTYPE_WORD, 32, 3 }, 
	{ SCOPTYPE_WORD, 45, 0 }, 
	{ SCOPTYPE_WORD, 50, 0 }, 
	{ SCOPTYPE_WORD, 55, 0 }, 
	{ SCOPTYPE_WORD, 60, 0 }, 
	{ SCOPTYPE_WORD, 65, 0 }, 
	{ SCOPTYPE_WORD, 70, 0 }, 
	{ SCOPTYPE_WORD, 75, 0 }, 
	{ SCOPTYPE_WORD, 79, 0 }, 
	{ SCOPTYPE_WORD, 83, 0 }, 
	{ SCOPTYPE_WORD, 90, 0 }, 
	{ SCOPTYPE_WORD, 105, 0 }, 
	{ SCOPTYPE_WORD, 114, 0 }, 
	{ SCOPTYPE_WORD, 123, 0 }, 
	{ SCOPTYPE_WORD, 126, 0 }, 
	{ SCOPTYPE_WORD, 129, 0 }, 
	{ SCOPTYPE_WORD, 132, 0 }, 
	{ SCOPTYPE_WORD, 142, 0 }, 
	{ SCOPTYPE_WORD, 151, 0 }, 
	{ SCOPTYPE_WORD, 160, 0 }, 
	{ SCOPTYPE_WORD, 167, 0 }, 
	{ SCOPTYPE_VAR, 0, 0 }, 
	{ SCOPTYPE_VAR, 0, 0 }, 
	{ SCOPTYPE_VAR, 0, 0 }, 
	{ SCOPTYPE_VAR, 0, 0 }, 
	{ SCOPTYPE_END, 0, 0 } 
};
//#LOGO ENDFILE

LogoBuiltinWord builtins[] = {
  { "RED1R", &red1Raw, 1 },
  { "GREEN1R", &green1Raw, 1 },
  { "BLUE1R", &blue1Raw, 1 },
  { "RED2R", &red2Raw, 1 },
  { "GREEN2R", &green2Raw, 1 },
  { "BLUE2R", &blue2Raw, 1 },
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
  pinMode(RED1_PIN, OUTPUT);
  analogWrite(RED1_PIN, 255);
  pinMode(BLUE1_PIN, OUTPUT);
  analogWrite(BLUE1_PIN, 255);
  pinMode(GREEN1_PIN, OUTPUT);
  analogWrite(GREEN1_PIN, 255);
 
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

