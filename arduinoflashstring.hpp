/*
  arduinoflashstring.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 22-May-2023

  A string that is stored in flash memory on an arduino
  
  This work is licensed under the Creative Commons Attribution 4.0 International License. 
  To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/ or 
  send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  https://github.com/visualopsholdings/tinylogo  
*/

#ifndef H_arduinoflashstring
#define H_arduinoflashstring

#include "logostring.hpp"

#ifdef ARDUINO

#include <avr/pgmspace.h>

class ArduinoFlashString: public LogoString {

public:
  ArduinoFlashString(const char *str PROGMEM): _str(str) {}
  
  size_t length() const;
  char operator[](int index) const;
  void ncpy(char *to, size_t offset, size_t len) const;
  int ncmp(const char *to, size_t offset, size_t len) const;
  int ncasecmp(const char *to, size_t offset, size_t len) const;

private:
  const char *_str PROGMEM;

};
#else
#define ArduinoFlashString LogoSimpleString
#endif

#endif // H_arduinoflashstring