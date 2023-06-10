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

// RingBuffer buffer;
// Cmd cmd;
// char cmdbuf[64];

static const char strings_led[] PROGMEM = {
	"FLASH\n"
};
static const short code_led[][INST_LENGTH] PROGMEM = {
	{ OPTYPE_BUILTIN, 2, 1 },		// 0
	{ OPTYPE_JUMP, 2, 0 },		// 1
	{ OPTYPE_HALT, 0, 0 },		// 2
	{ OPTYPE_BUILTIN, 0, 0 },		// 2
	{ OPTYPE_BUILTIN, 6, 1 },		// 3
	{ OPTYPE_INT, 1000, 0 },		// 4
	{ OPTYPE_BUILTIN, 1, 0 },		// 5
	{ OPTYPE_BUILTIN, 6, 1 },		// 6
	{ OPTYPE_INT, 500, 0 },		// 7
	{ OPTYPE_RETURN, 0, 0 },		// 8
	{ SCOPTYPE_WORD, 2, 0 }, 
	{ SCOPTYPE_END, 0, 0 } 
};

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

void flashErr(int mode, int n) {
  Serial.println(n);
  // it's ok to tie up the device with delays here.
  for (int i=0; i<mode; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    delay(100);
  }
  delay(500);
  for (int i=0; i<n; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    delay(100);
  }
}

// At the start
void setup() {

  // Setup the LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

// the button pin
  pinMode(BUTTON_PIN, INPUT);

  // Setup the serial port
//  Serial.begin(9600);

}

// Go around and around
void loop() {

  // // consume the serial data into the buffer as it comes in.
  // while (Serial.available()) {
  //   buffer.write(Serial.read());
  // }
 
  // // The buffer is also filled in an interrupt as it comes in

  // // accept the buffer into the command parser
  // cmd.accept(&buffer);
 
  // // when there is a valid command
  // if (cmd.ready()) {
  
  //   // read it in
  //   cmd.read(cmdbuf, sizeof(cmdbuf));
 
  //   // reset the code but keep all our words we have defined.
  //   logo.resetcode();
    
  //   int err = logo.callword(cmdbuf);
  //   if (err) {
  //     flashErr(1, err);
  //   }
  // }

  // just execute each LOGO word
  int err = logo.step();
  
  // And if there was an error doing that, apart from STOP (at the end)
  // flash the LED
  if (err && err != LG_STOP) {
    flashErr(2, err);
  }

}

