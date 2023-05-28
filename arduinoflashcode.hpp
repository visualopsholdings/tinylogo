/*
  arduinoflashcode.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 22-May-2023

  A chunk of code that is stored in flash memory on an arduino
  
  This work is licensed under the Creative Commons Attribution 4.0 International License. 
  To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/ or 
  send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  https://github.com/visualopsholdings/tinylogo  
*/

#ifndef H_arduinoflashcode
#define H_arduinoflashcode

#include "logo.hpp"

#ifdef ARDUINO
#include <avr/pgmspace.h>
#endif

class ArduinoFlashInstruction {

public:
  ArduinoFlashInstruction(const short *inst PROGMEM): _inst(inst) {}
  
  short operator[](int index) const;

private:
  const short *_inst PROGMEM;

};

class ArduinoFlashCode {

public:
  ArduinoFlashCode(const short *code PROGMEM): _code(code) {}
  
  ArduinoFlashInstruction operator[](int index) const {
    ArduinoFlashInstruction inst(_code + index * INST_LENGTH);
    return inst;
  }

private:
  const short *_code PROGMEM;

};

#endif // H_arduinoflashcode
