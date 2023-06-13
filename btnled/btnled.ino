/*
  Flash a LED with a button
  
  Author: Paul Hamilton
  Date: 8-Jun-2023
  
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
#define BUTTON_PIN  7

void ledOn() {
  digitalWrite(LED_PIN, HIGH);
}

void ledOff() {
  digitalWrite(LED_PIN, LOW);
}

void button(Logo &logo) {
  logo.pushint(digitalRead(BUTTON_PIN));
}

//#LOGO FILE=../logo/btnledflash.lgo NAME=led
static const char strings_led[] PROGMEM = {
	"FLASH\n"
	"TESTSTATE\n"
	"TESTVALUE\n"
	"OLDVALUE\n"
	"VALUE\n"
};
static const short code_led[][INST_LENGTH] PROGMEM = {
	{ OPTYPE_BUILTIN, 2, 1 },		// 0
	{ OPTYPE_JUMP, 17, 0 },		// 1
	{ OPTYPE_HALT, 0, 0 },		// 2
	{ OPTYPE_BUILTIN, 0, 0 },		// 3
	{ OPTYPE_BUILTIN, 6, 1 },		// 4
	{ OPTYPE_INT, 1000, 0 },		// 5
	{ OPTYPE_BUILTIN, 1, 0 },		// 6
	{ OPTYPE_RETURN, 0, 0 },		// 7
	{ OPTYPE_BUILTIN, 1, 1 },		// 8
	{ OPTYPE_STRING, 3, 8 },		// 9
	{ OPTYPE_REF, 4, 5 },		// 10
	{ OPTYPE_BUILTIN, 5, 1 },		// 11
	{ OPTYPE_REF, 4, 5 },		// 12
	{ OPTYPE_BUILTIN, 7, 1 },		// 13
	{ OPTYPE_INT, 1, 0 },		// 14
	{ OPTYPE_JUMP, 3, 0 },		// 15
	{ OPTYPE_RETURN, 0, 0 },		// 16
	{ OPTYPE_BUILTIN, 1, 1 },		// 17
	{ OPTYPE_STRING, 4, 5 },		// 18
	{ OPTYPE_BUILTIN, 2, 0 },		// 19
	{ OPTYPE_BUILTIN, 5, 1 },		// 20
	{ OPTYPE_REF, 4, 5 },		// 21
	{ OPTYPE_BUILTIN, 8, 1 },		// 22
	{ OPTYPE_REF, 3, 8 },		// 23
	{ OPTYPE_JUMP, 8, 0 },		// 24
	{ OPTYPE_RETURN, 0, 0 },		// 25
	{ SCOPTYPE_WORD, 3, 0 }, 
	{ SCOPTYPE_WORD, 8, 0 }, 
	{ SCOPTYPE_WORD, 17, 0 }, 
	{ SCOPTYPE_END, 0, 0 } 
};
//#LOGO ENDFILE

LogoBuiltinWord builtins[] = {
  { "ON", &ledOn },
  { "OFF", &ledOff },
  { "BUTTON", &button },
};
ArduinoTimeProvider time;
ArduinoFlashString strings(strings_led);
ArduinoFlashCode code((const PROGMEM short *)code_led);
LogoFunctionPrimitives primitives(builtins, sizeof(builtins));
Logo logo(&primitives, &time, Logo::core, &strings, &code);

void showErr(int mode, int n) {
  Serial.print(mode);
  Serial.print(", ");
  Serial.println(n);
}

// At the start
void setup() {

  // Setup the LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // the button pin
  pinMode(BUTTON_PIN, INPUT);

  // Setup the serial port
  Serial.begin(9600);

}

// Go around and around
void loop() {

  int err = logo.step();
  if (err && err != LG_STOP) {
    showErr(2, err);
  }

}

