/*
  arduinoflashstring.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 22-May-2023

  This work is licensed under the Creative Commons Attribution 4.0 International License. 
  To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/ or 
  send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  https://github.com/visualopsholdings/tinylogo  
*/

#include "arduinoflashstring.hpp"

size_t ArduinoFlashString::length() const {

  return strlen_P(_str);
  
}

char ArduinoFlashString::operator[](int index) const {

  return static_cast<char>(pgm_read_byte(_str + index));
  
}

void ArduinoFlashString::ncpy(char *to, size_t offset, size_t len) const {

  strncpy_P(to, _str + offset, len);
  
}

int ArduinoFlashString::ncmp(const char *to, size_t offset, size_t len) const {

  return strncmp_P(to, _str + offset, len);
  
}
