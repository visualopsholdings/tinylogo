/*
  arduinoflashcode.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 22-May-2023

  This work is licensed under the Creative Commons Attribution 4.0 International License. 
  To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/ or 
  send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  https://github.com/visualopsholdings/tinylogo  
*/

#include "arduinoflashcode.hpp"

short ArduinoFlashInstruction::operator[](int index) const {
#if defined(ARDUINO) && defined(__AVR__)
  short val = pgm_read_word(_inst + index);
#else
  short val = *(_inst + index);
#endif
  return *reinterpret_cast<short *>(&val);
}

