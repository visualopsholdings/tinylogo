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

#include <Wire.h>

#define I2C_ADDRESS 8
#define LED_PIN     13
#define BLUE_PIN    2
#define RED_PIN     3
#define GREEN_PIN   4

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

RingBuffer buffer;
Cmd cmd;
char cmdbuf[64];

// the strings for the program in flash
static const char strings_rgb[] PROGMEM = {
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
	"C\n"
	"N\n"
	"B\n"
	"G\n"
	"R\n"
};
static const short code_rgb[][INST_LENGTH] PROGMEM = {
	{ OPTYPE_NOOP, 0, 0 },		// 0
	{ OPTYPE_HALT, 0, 0 },		// 1
	{ OPTYPE_POPREF, 0, 0 },		// 2
	{ OPTYPE_INT, 100, 0 },		// 3
	{ OPTYPE_BUILTIN, 7, 1 },		// 4
	{ OPTYPE_REF, 10, 1 },		// 5
	{ OPTYPE_BUILTIN, 9, 1 },		// 6
	{ OPTYPE_INT, 100, 0 },		// 7
	{ OPTYPE_BUILTIN, 10, 1 },		// 8
	{ OPTYPE_INT, 255, 0 },		// 9
	{ OPTYPE_RETURN, 0, 0 },		// 10
	{ OPTYPE_POPREF, 1, 0 },		// 11
	{ OPTYPE_BUILTIN, 0, 0 },		// 12
	{ OPTYPE_JUMP, 2, 1 },		// 13
	{ OPTYPE_REF, 11, 1 },		// 14
	{ OPTYPE_RETURN, 0, 0 },		// 15
	{ OPTYPE_POPREF, 1, 0 },		// 16
	{ OPTYPE_BUILTIN, 1, 0 },		// 17
	{ OPTYPE_JUMP, 2, 1 },		// 18
	{ OPTYPE_REF, 11, 1 },		// 19
	{ OPTYPE_RETURN, 0, 0 },		// 20
	{ OPTYPE_POPREF, 1, 0 },		// 21
	{ OPTYPE_BUILTIN, 2, 0 },		// 22
	{ OPTYPE_JUMP, 2, 1 },		// 23
	{ OPTYPE_REF, 11, 1 },		// 24
	{ OPTYPE_RETURN, 0, 0 },		// 25
	{ OPTYPE_POPREF, 2, 0 },		// 26
	{ OPTYPE_POPREF, 3, 0 },		// 27
	{ OPTYPE_POPREF, 4, 0 },		// 28
	{ OPTYPE_JUMP, 11, 1 },		// 29
	{ OPTYPE_REF, 14, 1 },		// 30
	{ OPTYPE_JUMP, 16, 1 },		// 31
	{ OPTYPE_REF, 13, 1 },		// 32
	{ OPTYPE_JUMP, 21, 1 },		// 33
	{ OPTYPE_REF, 12, 1 },		// 34
	{ OPTYPE_RETURN, 0, 0 },		// 35
	{ OPTYPE_JUMP, 26, 3 },		// 36
	{ OPTYPE_INT, 100, 0 },		// 37
	{ OPTYPE_INT, 75, 0 },		// 38
	{ OPTYPE_INT, 0, 0 },		// 39
	{ OPTYPE_RETURN, 0, 0 },		// 40
	{ OPTYPE_JUMP, 26, 3 },		// 41
	{ OPTYPE_INT, 100, 0 },		// 42
	{ OPTYPE_INT, 0, 0 },		// 43
	{ OPTYPE_INT, 0, 0 },		// 44
	{ OPTYPE_RETURN, 0, 0 },		// 45
	{ OPTYPE_JUMP, 26, 3 },		// 46
	{ OPTYPE_INT, 0, 0 },		// 47
	{ OPTYPE_INT, 100, 0 },		// 48
	{ OPTYPE_INT, 0, 0 },		// 49
	{ OPTYPE_RETURN, 0, 0 },		// 50
	{ OPTYPE_JUMP, 26, 3 },		// 51
	{ OPTYPE_INT, 0, 0 },		// 52
	{ OPTYPE_INT, 0, 0 },		// 53
	{ OPTYPE_INT, 100, 0 },		// 54
	{ OPTYPE_RETURN, 0, 0 },		// 55
	{ OPTYPE_JUMP, 26, 3 },		// 56
	{ OPTYPE_INT, 0, 0 },		// 57
	{ OPTYPE_INT, 0, 0 },		// 58
	{ OPTYPE_INT, 0, 0 },		// 59
	{ OPTYPE_RETURN, 0, 0 },		// 60
	{ SCOPTYPE_WORD, 2, 1 }, 
	{ SCOPTYPE_WORD, 11, 1 }, 
	{ SCOPTYPE_WORD, 16, 1 }, 
	{ SCOPTYPE_WORD, 21, 1 }, 
	{ SCOPTYPE_WORD, 26, 3 }, 
	{ SCOPTYPE_WORD, 36, 0 }, 
	{ SCOPTYPE_WORD, 41, 0 }, 
	{ SCOPTYPE_WORD, 46, 0 }, 
	{ SCOPTYPE_WORD, 51, 0 }, 
	{ SCOPTYPE_WORD, 56, 0 }, 
	{ SCOPTYPE_VAR, 0, 0 }, 
	{ SCOPTYPE_VAR, 0, 0 }, 
	{ SCOPTYPE_VAR, 0, 0 }, 
	{ SCOPTYPE_VAR, 0, 0 }, 
	{ SCOPTYPE_VAR, 0, 0 }, 
	{ SCOPTYPE_END, 0, 0 } 
};

LogoBuiltinWord builtins[] = {
  { "REDR", &redRaw, 1 },
  { "GREENR", &greenRaw, 1 },
  { "BLUER", &blueRaw, 1 },
};
ArduinoTimeProvider time;
ArduinoFlashString strings(strings_rgb);
ArduinoFlashCode code((const PROGMEM short *)code_rgb);
Logo logo(builtins, sizeof(builtins), &time, Logo::core, &strings, &code);

void flashErr(int mode, int n) {

  Serial.print("err ");
  Serial.print(mode);
  Serial.print(", ");
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

  // the RGB pins
  pinMode(RED_PIN, OUTPUT);
  analogWrite(RED_PIN, 255);
  pinMode(BLUE_PIN, OUTPUT);
  analogWrite(BLUE_PIN, 255);
  pinMode(GREEN_PIN, OUTPUT);
  analogWrite(GREEN_PIN, 255);
 
  Serial.begin(9600);
 
  // Setup I2C bus address
  Wire.begin(I2C_ADDRESS);
  
  // Register handler for when data comes in
  Wire.onReceive(receiveEvent);

}

// recieve data on I2C and write it into the buffer
void receiveEvent(int howMany) {
  while (Wire.available()) {
    buffer.write(Wire.read());
  }
}

// Go around and around
void loop() {

  // consume the serial data into the buffer as it comes in.
   while (Serial.available()) {
    buffer.write(Serial.read());
  }

  // The buffer is filled in an interrupt as it comes in

  // accept the buffer into the command parser
  cmd.accept(&buffer);
  
  // when there is a valid command
  if (cmd.ready()) {

    // read it in
    cmd.read(cmdbuf, sizeof(cmdbuf));

    // reset the code but keep all our words we have defined.
    logo.resetcode();

    int err = logo.callword(cmdbuf);
    if (err) {
      flashErr(1, err);
    }
  }
  
  // just execute each LOGO word
  int err = logo.step();
  
  // And if there was an error doing that, apart from STOP (at the end)
  // flash the LED
  if (err && err != LG_STOP) {
    flashErr(2, err);
  }

}

