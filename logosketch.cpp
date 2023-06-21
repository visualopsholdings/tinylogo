/*
  logosketch.cpp
    
  Author: Paul Hamilton (paul@visualops.com)
  Date: 14-Jun-2023
  
  This work is licensed under the Creative Commons Attribution 4.0 International License. 
  To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/ or 
  send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  https://github.com/visualopsholdings/tinylogo
*/

#include "logosketch.hpp"

#include <Arduino.h>

LogoSketch::LogoSketch(char strings[] PROGMEM, const PROGMEM short *code, LogoBuiltinWord builtins[], int size): 
  _strings(strings), _code(code), _primitives(builtins, size), 
  _logo(&_primitives, &_time, Logo::core, &_strings, &_code) {
}

void LogoSketch::setup()  {

  Serial.begin(9600);
  
  int err = _logo.callword("SETUP");
  if (!err) {
    err = _logo.run();
    if (err && err != LG_STOP) {
      showErr(0, err);
    }
    _logo.resetcode();
  }

}

void LogoSketch::loop() {

  // consume the serial data into the buffer as it comes in.
  while (Serial.available()) {
    _buffer.write(Serial.read());
  }
 
  // accept the buffer into the command parser
  _cmd.accept(&_buffer);
 
  // when there is a valid command
  if (_cmd.ready()) {
  
    // read it in
    _cmd.read(_cmdbuf, sizeof(_cmdbuf));
 
    // reset the code but keep all our words we have defined.
    _logo.resetcode();
    
    int err = _logo.callword(_cmdbuf);
    if (err) {
      showErr(1, err);
    }
  }

  int err = _logo.step();
  if (err && err != LG_STOP) {
    showErr(2, err);
  }
  
}

void LogoSketch::showErr(int mode, int n) {

  Serial.print("err ");
  Serial.print(mode);
  Serial.print(", ");
  Serial.println(n);

}
