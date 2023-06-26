/*
  Flash a LED with logo code sent from I2C or serial
  
  Author: Paul Hamilton
  Date: 5-May-2023
  
  This is an example of building the actual compiler into the arduino. the
  sketch example shows how you can put a much larger program in by using
  the flash code functionality.

  This work is licensed under the Creative Commons Attribution 4.0 International License. 
  To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/ or 
  send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  https://github.com/visualopsholdings/tinylogo
*/

// this sketch relies on the USE_FLASH_CODE being commented out in logo.hpp

#include "ringbuffer.hpp"
#include "cmd.hpp"
#include "logo.hpp"
#include "arduinotimeprovider.hpp"
#include "arduinoflashcode.hpp"
#include "logocompiler.hpp"
#include "logowords.hpp"

#include <Wire.h>

#define I2C_ADDRESS 8
#define LED_PIN     13

RingBuffer buffer;
Cmd cmd;
char cmdbuf[32];

static const char program_led[] PROGMEM = 
  "to ON; dhigh 13; end;"
  "to OFF; dlow 13; end;"
  "to FLASH; ON wait 500 OFF wait 1000; end;"
  "to GO; forever FLASH; end;"
  "to STOP; OFF; end;"
  ;

ArduinoTimeProvider time;
Logo logo(&time);
LogoCompiler compiler(&logo);

void showErr(int mode, int n) {
  Serial.print(mode);
  Serial.print(",");
  Serial.println(n);
}

// At the start
void setup() {

  // Setup the LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

   // Setup the serial port
  Serial.begin(9600);

  // // Setup I2C bus address
  Wire.begin(I2C_ADDRESS);
  
  // Register handler for when data comes in
  Wire.onReceive(receiveEvent);

 // Compile a little program into the LOGO interpreter :-)
  ArduinoFlashString str(program_led);
  compiler.compile(&str);
  int err = logo.geterr();
  if (err) {
    showErr(1, err + 2);
  }
 
  // this makes it run straight away.
  compiler.compile("GO");

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
    
    // compile whatever it is into the LOGO interpreter and if there's
    // a compile error flash the LED
    compiler.compile(cmdbuf);
    int err = logo.geterr();
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

