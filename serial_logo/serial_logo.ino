/*
  Flash a LED with logo code sent from serial data
  
  Author: Paul Hamilton
  Date: 5-May-2023
  
  This work is licensed under the Creative Commons Attribution 4.0 International License. 
  To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/ or 
  send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  https://github.com/visualopsholdings/tinylogo
*/

#include "ringbuffer.hpp"
#include "cmd.hpp"
#include "logo.hpp"
#include "logocompiler.hpp"
#include "arduinotimeprovider.hpp"

#define LED_PIN     13

void ledOn() {
  digitalWrite(LED_PIN, HIGH);
}

void ledOff() {
  digitalWrite(LED_PIN, LOW);
}

RingBuffer buffer;
Cmd cmd;
char cmdbuf[64];

LogoBuiltinWord builtins[] = {
  { "ON", &ledOn },
  { "OFF", &ledOff },
};

ArduinoTimeProvider time;
Logo logo(builtins, sizeof(builtins), &time, Logo::core);
LogoCompiler compiler(&logo);

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
  
  // Setup the serial port
  Serial.begin(9600);

  // Compile a little program into the LOGO interpreter :-)
  compiler.compile("TO FLASH; ON WAIT 100 OFF WAIT 1000; END;");
  compiler.compile("TO GO; FOREVER FLASH; END;");
  compiler.compile("TO STOP; OFF; END;");
  int err = logo.geterr();
  if (err) {
    flashErr(1, err + 2);
  }
 
  // this would make it just run straight away
//  compiler.compile("GO");
}

// Go around and around
void loop() {

  // consume the serial data into the buffer as it comes in.
  while (Serial.available()) {
    buffer.write(Serial.read());
  }
 
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

