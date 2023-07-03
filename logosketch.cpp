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

void LogoSketchBase::setup(int baud)  {

  Serial.begin(baud);
  
  precompile();
  
  int err = logo()->geterr();
  if (err) {
    showErr(0, err + 2);
  }

  err = dosetup("SETUP");
  if (!err) {
    err = logo()->run();
    if (err && err != LG_STOP) {
      showErr(1, err);
    }
    logo()->restart();
  }

}

void LogoSketchBase::sslsetup(const char *host, const char *cert) {
  logo()->sslsetup(host, cert);
}

void LogoSketchBase::loop() {

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
    logo()->resetcode();
    
    // process a command.
    int err = docommand(_cmdbuf);
    if (err) {
      showErr(2, err);
    }

  }

  int err = logo()->step();
  if (err && err != LG_STOP) {
    showErr(3, err);
  }
  
}

void LogoSketchBase::showErr(int mode, int n) {

  Serial.print("e=");
  Serial.print(mode);
  Serial.print(",");
  Serial.println(n);

}

LogoSketch::LogoSketch(char strings[] PROGMEM, const PROGMEM short *code): 
  _strings(strings), _code(code), _logo(&_time, &_strings, &_code) {

}

void LogoSketch::precompile() {
}

int LogoSketch::dosetup(const char *cmd) {

  // we don't know how to compile for now when doing a 
  return docommand(cmd);
  
}

int LogoSketch::docommand(const char *cmd) {

  return _logo.callword(cmd);
  
}

LogoInlineSketch::LogoInlineSketch(char program[] PROGMEM):
  _program(program), _logo(&_time), _compiler(&_logo) {
  
}

void LogoInlineSketch::precompile() {

  // Compile a little program into the LOGO interpreter :-)
  _compiler.compile(&_program);

}

int LogoInlineSketch::dosetup(const char *cmd) {

  return _compiler.callword(cmd);
  
}

int LogoInlineSketch::docommand(const char *cmd) {

  // compile whatever it is into the LOGO interpreter and if there's
  // a compile error flash the LED
  _compiler.compile(cmd);
  return _logo.geterr();
  
}
