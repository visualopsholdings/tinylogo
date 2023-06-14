/*
  Flash a LED with logo code sent from I2C or serial
  
  Author: Paul Hamilton
  Date: 5-May-2023
  
  This work is licensed under the Creative Commons Attribution 4.0 International License. 
  To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/ or 
  send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  https://github.com/visualopsholdings/tinylogo
*/

// if you uncomment this, also uncomment the USE_FLASH_CODE in logo.hpp
#define FLASH_CODE

#include "ringbuffer.hpp"
#include "cmd.hpp"
#include "logo.hpp"
#include "arduinotimeprovider.hpp"
#include "arduinoflashstring.hpp"
#ifdef FLASH_CODE
#include "arduinoflashcode.hpp"
#else
#include "logocompiler.hpp"
#endif

#include <Wire.h>

#define I2C_ADDRESS 8
#define LED_PIN     13
//#define LED_PIN     3

void ledOn() {
  digitalWrite(LED_PIN, HIGH);
}

void ledOff() {
  digitalWrite(LED_PIN, LOW);
}

RingBuffer buffer;
Cmd cmd;
char cmdbuf[64];

#ifdef FLASH_CODE

//#LOGO FILE=../logo/ledflash.lgo NAME=led
static const char strings_led[] PROGMEM = {
// words
	"FLASH\n"
	"GO\n"
	"STOP\n"
// variables
// strings
};
static const short code_led[][INST_LENGTH] PROGMEM = {
	{ OPTYPE_JUMP, 9, 0 },		// 0
	{ OPTYPE_HALT, 0, 0 },		// 1
	{ OPTYPE_BUILTIN, 0, 0 },		// 2
	{ OPTYPE_BUILTIN, 6, 1 },		// 3
	{ OPTYPE_INT, 500, 0 },		// 4
	{ OPTYPE_BUILTIN, 1, 0 },		// 5
	{ OPTYPE_BUILTIN, 6, 1 },		// 6
	{ OPTYPE_INT, 1000, 0 },		// 7
	{ OPTYPE_RETURN, 0, 0 },		// 8
	{ OPTYPE_BUILTIN, 2, 1 },		// 9
	{ OPTYPE_JUMP, 2, 0 },		// 10
	{ OPTYPE_RETURN, 0, 0 },		// 11
	{ OPTYPE_BUILTIN, 1, 0 },		// 12
	{ OPTYPE_RETURN, 0, 0 },		// 13
	{ SCOPTYPE_WORD, 2, 0 }, 
	{ SCOPTYPE_WORD, 9, 0 }, 
	{ SCOPTYPE_WORD, 12, 0 }, 
	{ SCOPTYPE_END, 0, 0 } 
};
//#LOGO ENDFILE

#else
static const char program_led[] PROGMEM = 
  "TO FLASH; ON WAIT 500 OFF WAIT 1000; END;"
  "TO GO; FOREVER FLASH; END;"
  "TO STOP; OFF; END;"
  ;
#endif

LogoBuiltinWord builtins[] = {
  { "ON", &ledOn },
  { "OFF", &ledOff },
};
ArduinoTimeProvider time;
LogoFunctionPrimitives primitives(builtins, sizeof(builtins));
#ifdef FLASH_CODE
ArduinoFlashString strings(strings_led);
ArduinoFlashCode code((const PROGMEM short *)code_led);
Logo logo(&primitives, &time, Logo::core, &strings, &code);
#else
Logo logo(&primitives, &time, Logo::core);
LogoCompiler compiler(&logo);
#endif


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

   // Setup the serial port
  Serial.begin(9600);

  // Setup I2C bus address
  Wire.begin(I2C_ADDRESS);
  
  // Register handler for when data comes in
  Wire.onReceive(receiveEvent);

#ifndef FLASH_CODE
 // Compile a little program into the LOGO interpreter :-)
  ArduinoFlashString str(program_led);
  compiler.compile(&str);
  int err = logo.geterr();
  if (err) {
    showErr(1, err + 2);
  }
 
  // this would make it just run straight away
//  compiler.compile("GO");
#endif

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
 
  // The buffer is also filled in an interrupt as it comes in

  // accept the buffer into the command parser
  cmd.accept(&buffer);
 
  // when there is a valid command
  if (cmd.ready()) {
  
    // read it in
    cmd.read(cmdbuf, sizeof(cmdbuf));
 
    // reset the code but keep all our words we have defined.
    logo.resetcode();
    
#ifdef FLASH_CODE
    int err = logo.callword(cmdbuf);
 #else
     // // compile whatever it is into the LOGO interpreter and if there's
    // // a compile error flash the LED
    compiler.compile(cmdbuf);
    int err = logo.geterr();
 #endif
    if (err) {
      showErr(1, err);
    }
  }

  // just execute each LOGO word
  int err = logo.step();
  
  // And if there was an error doing that, apart from STOP (at the end)
  // flash the LED
  if (err && err != LG_STOP) {
    showErr(2, err);
  }

}

