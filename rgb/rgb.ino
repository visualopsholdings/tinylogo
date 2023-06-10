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
#define BUTTON_PIN  7

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

void button(Logo &logo) {
  logo.pushint(digitalRead(BUTTON_PIN));
}

RingBuffer buffer;
Cmd cmd;
char cmdbuf[64];

int buttonValue = 0;
int buttonState = 0;

// the strings for the program in flash
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
	"OFF\n"
	"NEXTSTATE\n"
	"FIRSTSTATE\n"
	"NOTRED\n"
	"PRESS\n"
	"TESTSTATE\n"
	"TESTVALUE\n"
	"C\n"
	"N\n"
	"B\n"
	"G\n"
	"R\n"
};
static const short code_rgb[][INST_LENGTH] PROGMEM = {
	{ OPTYPE_JUMP, 49, 0 },		// 0
	{ OPTYPE_BUILTIN, 2, 1 },		// 1
	{ OPTYPE_JUMP, 103, 0 },		// 2
	{ OPTYPE_HALT, 0, 0 },		// 3
	{ OPTYPE_POPREF, 0, 0 },		// 2
	{ OPTYPE_INT, 100, 0 },		// 3
	{ OPTYPE_BUILTIN, 9, 1 },		// 4
	{ OPTYPE_REF, 17, 1 },		// 5
	{ OPTYPE_RETURN, 0, 0 },		// 6
	{ OPTYPE_POPREF, 0, 0 },		// 7
	{ OPTYPE_JUMP, 2, 1 },		// 8
	{ OPTYPE_REF, 17, 1 },		// 9
	{ OPTYPE_BUILTIN, 11, 1 },		// 10
	{ OPTYPE_INT, 100, 0 },		// 11
	{ OPTYPE_RETURN, 0, 0 },		// 12
	{ OPTYPE_POPREF, 0, 0 },		// 13
	{ OPTYPE_JUMP, 7, 1 },		// 14
	{ OPTYPE_REF, 17, 1 },		// 15
	{ OPTYPE_BUILTIN, 12, 1 },		// 16
	{ OPTYPE_INT, 255, 0 },		// 17
	{ OPTYPE_RETURN, 0, 0 },		// 18
	{ OPTYPE_POPREF, 1, 0 },		// 19
	{ OPTYPE_BUILTIN, 0, 0 },		// 20
	{ OPTYPE_JUMP, 13, 1 },		// 21
	{ OPTYPE_REF, 18, 1 },		// 22
	{ OPTYPE_RETURN, 0, 0 },		// 23
	{ OPTYPE_POPREF, 1, 0 },		// 24
	{ OPTYPE_BUILTIN, 1, 0 },		// 25
	{ OPTYPE_JUMP, 13, 1 },		// 26
	{ OPTYPE_REF, 18, 1 },		// 27
	{ OPTYPE_RETURN, 0, 0 },		// 28
	{ OPTYPE_POPREF, 1, 0 },		// 29
	{ OPTYPE_BUILTIN, 2, 0 },		// 30
	{ OPTYPE_JUMP, 13, 1 },		// 31
	{ OPTYPE_REF, 18, 1 },		// 32
	{ OPTYPE_RETURN, 0, 0 },		// 33
	{ OPTYPE_POPREF, 2, 0 },		// 34
	{ OPTYPE_POPREF, 3, 0 },		// 35
	{ OPTYPE_POPREF, 4, 0 },		// 36
	{ OPTYPE_JUMP, 19, 1 },		// 37
	{ OPTYPE_REF, 21, 1 },		// 38
	{ OPTYPE_JUMP, 24, 1 },		// 39
	{ OPTYPE_REF, 20, 1 },		// 40
	{ OPTYPE_JUMP, 29, 1 },		// 41
	{ OPTYPE_REF, 19, 1 },		// 42
	{ OPTYPE_RETURN, 0, 0 },		// 43
	{ OPTYPE_JUMP, 34, 3 },		// 44
	{ OPTYPE_INT, 100, 0 },		// 45
	{ OPTYPE_INT, 75, 0 },		// 46
	{ OPTYPE_INT, 0, 0 },		// 47
	{ OPTYPE_RETURN, 0, 0 },		// 48
	{ OPTYPE_JUMP, 34, 3 },		// 49
	{ OPTYPE_INT, 100, 0 },		// 50
	{ OPTYPE_INT, 0, 0 },		// 51
	{ OPTYPE_INT, 0, 0 },		// 52
	{ OPTYPE_RETURN, 0, 0 },		// 53
	{ OPTYPE_JUMP, 34, 3 },		// 54
	{ OPTYPE_INT, 0, 0 },		// 55
	{ OPTYPE_INT, 100, 0 },		// 56
	{ OPTYPE_INT, 0, 0 },		// 57
	{ OPTYPE_RETURN, 0, 0 },		// 58
	{ OPTYPE_JUMP, 34, 3 },		// 59
	{ OPTYPE_INT, 0, 0 },		// 60
	{ OPTYPE_INT, 0, 0 },		// 61
	{ OPTYPE_INT, 0, 0 },		// 62
	{ OPTYPE_RETURN, 0, 0 },		// 63
	{ OPTYPE_REF, 22, 5 },		// 64
	{ OPTYPE_BUILTIN, 10, 1 },		// 65
	{ OPTYPE_INT, 1, 0 },		// 66
	{ OPTYPE_RETURN, 0, 0 },		// 67
	{ OPTYPE_BUILTIN, 1, 1 },		// 68
	{ OPTYPE_STRING, 27, 5 },		// 69
	{ OPTYPE_INT, 0, 0 },		// 70
	{ OPTYPE_RETURN, 0, 0 },		// 71
	{ OPTYPE_BUILTIN, 4, 1 },		// 72
	{ OPTYPE_REF, 32, 5 },		// 73
	{ OPTYPE_BUILTIN, 7, 1 },		// 74
	{ OPTYPE_INT, 1, 0 },		// 75
	{ OPTYPE_JUMP, 54, 0 },		// 76
	{ OPTYPE_JUMP, 44, 0 },		// 77
	{ OPTYPE_RETURN, 0, 0 },		// 78
	{ OPTYPE_BUILTIN, 5, 1 },		// 79
	{ OPTYPE_REF, 37, 5 },		// 80
	{ OPTYPE_BUILTIN, 13, 1 },		// 81
	{ OPTYPE_INT, 2, 0 },		// 82
	{ OPTYPE_JUMP, 68, 0 },		// 83
	{ OPTYPE_BUILTIN, 1, 1 },		// 84
	{ OPTYPE_STRING, 42, 5 },		// 85
	{ OPTYPE_JUMP, 64, 0 },		// 86
	{ OPTYPE_BUILTIN, 4, 1 },		// 87
	{ OPTYPE_REF, 47, 5 },		// 88
	{ OPTYPE_BUILTIN, 7, 1 },		// 89
	{ OPTYPE_INT, 0, 0 },		// 90
	{ OPTYPE_JUMP, 49, 0 },		// 91
	{ OPTYPE_JUMP, 72, 0 },		// 92
	{ OPTYPE_RETURN, 0, 0 },		// 93
	{ OPTYPE_BUILTIN, 1, 1 },		// 94
	{ OPTYPE_STRING, 52, 8 },		// 95
	{ OPTYPE_REF, 60, 5 },		// 96
	{ OPTYPE_BUILTIN, 5, 1 },		// 97
	{ OPTYPE_REF, 65, 5 },		// 98
	{ OPTYPE_BUILTIN, 7, 1 },		// 99
	{ OPTYPE_INT, 1, 0 },		// 100
	{ OPTYPE_JUMP, 79, 0 },		// 101
	{ OPTYPE_RETURN, 0, 0 },		// 102
	{ OPTYPE_BUILTIN, 1, 1 },		// 103
	{ OPTYPE_STRING, 70, 5 },		// 104
	{ OPTYPE_BUILTIN, 3, 0 },		// 105
	{ OPTYPE_BUILTIN, 5, 1 },		// 106
	{ OPTYPE_REF, 75, 5 },		// 107
	{ OPTYPE_BUILTIN, 8, 1 },		// 108
	{ OPTYPE_REF, 80, 8 },		// 109
	{ OPTYPE_JUMP, 94, 0 },		// 110
	{ OPTYPE_RETURN, 0, 0 },		// 111
	{ SCOPTYPE_WORD, 2, 1 }, 
	{ SCOPTYPE_WORD, 7, 1 }, 
	{ SCOPTYPE_WORD, 13, 1 }, 
	{ SCOPTYPE_WORD, 19, 1 }, 
	{ SCOPTYPE_WORD, 24, 1 }, 
	{ SCOPTYPE_WORD, 29, 1 }, 
	{ SCOPTYPE_WORD, 34, 3 }, 
	{ SCOPTYPE_WORD, 44, 0 }, 
	{ SCOPTYPE_WORD, 49, 0 }, 
	{ SCOPTYPE_WORD, 54, 0 }, 
	{ SCOPTYPE_WORD, 59, 0 }, 
	{ SCOPTYPE_WORD, 64, 0 }, 
	{ SCOPTYPE_WORD, 68, 0 }, 
	{ SCOPTYPE_WORD, 72, 0 }, 
	{ SCOPTYPE_WORD, 79, 0 }, 
	{ SCOPTYPE_WORD, 94, 0 }, 
	{ SCOPTYPE_WORD, 103, 0 }, 
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
  { "BUTTON", &button, 0 },
};
ArduinoTimeProvider time;
ArduinoFlashString strings(strings_rgb);
ArduinoFlashCode code((const PROGMEM short *)code_rgb);
LogoFunctionPrimitives primitives(builtins, sizeof(builtins));
Logo logo(&primitives, &time, Logo::core, &strings, &code);

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
 
  // the button pin
  pinMode(BUTTON_PIN, INPUT);

  Serial.begin(9600);
 
  // Setup I2C bus address
  Wire.begin(I2C_ADDRESS);
  
  // Register handler for when data comes in
  Wire.onReceive(receiveEvent);

  int err = logo.callword("RED");
  if (err && err != LG_STOP) {
    flashErr(2, err);
  }
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
  
  // int newValue = digitalRead(BUTTON_PIN);
  // if (newValue != buttonValue) {
  //   buttonValue = newValue;
  //   if (buttonValue == HIGH) {
  //     buttonState++;
  //     if (buttonState > 2) {
  //       buttonState = 0;
  //     }
  //     logo.resetcode();
  //     int err = 0;
  //     switch (buttonState) {
  //     case 0:
  //       err = logo.callword("RED");
  //       break;
  //     case 1:
  //       err = logo.callword("GREEN");
  //       break;
  //     default:
  //       err = logo.callword("AMBER");
  //     }
  //     if (err && err != LG_STOP) {
  //       flashErr(2, err);
  //     }
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

