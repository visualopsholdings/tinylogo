/*
  logowordnames.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 5-May-2023
  
  The names of the actual words
    
  This work is licensed under the Creative Commons Attribution 4.0 International License. 
  To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/ or 
  send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  https://github.com/visualopsholdings/tinylogo
*/

#ifndef H_logowordnames
#define H_logowordnames

const char Logo::coreNames[] PROGMEM = {
  "ERR\n"
  "MAKE\n"
  "FOREVER\n"
  "REPEAT\n"
  "IFELSE\n"
  "IF\n"
  "WAIT\n"
  "=\n"
  "!=\n"
  "-\n"
  "+\n"
  "/\n"
  "*\n"
  ">\n"
  ">=\n"
  "<\n"
  "<=\n"
  "DUMPVARS\n"
  "PRINT\n"
  "!\n"
  "DREAD\n"
  "DHIGH\n"
  "DLOW\n"
  "PINOUT\n"
  "PININ\n"
  "AOUT\n"
  "THING\n"
  "OUTPUT\n"
  "WORD\n"
  "FIRST\n"
  "PININUP\n"
  "TYPE\n"
  "MACHINEINFO\n"
  "PINRGB\n"
  "RGBOUT\n"
  "WIFISCAN\n"
  "WIFIGET\n"
  "WIFISOCKETS\n"
  "VOPSOPENMSG\n"
  "WIFILOGINTEST\n"
  "WIFILOGIN\n"
  "WIFISTATION\n"
  "WIFICONNECT\n"
  "BTSTART\n"
};

// this arity array matches the above.
const char Logo::coreArity[] PROGMEM = {
  0, // ERR
  2, // MAKE
  0, // FOREVER
  1, // REPEAT
  1, // IFELSE
  1, // IF
  1, // WAIT
  INFIX_ARITY, // =
  INFIX_ARITY, // !=
  INFIX_ARITY, // -
  INFIX_ARITY, // +
  INFIX_ARITY, // /
  INFIX_ARITY, // *
  INFIX_ARITY, // >
  INFIX_ARITY, // >=
  INFIX_ARITY, // <
  INFIX_ARITY, // <=
  0, // DUMPVARS
  1, // PRINT
  1, // !
  1, // DREAD
  1, // DHIGH
  1, // DLOW
  1, // PINOUT
  1, // PININ
  2, // AOUT
  1, // THING
  0, // OUTPUT
  2, // WORD
  1, // FIRST
  1, // PININUP
  1, // TYPE
  0, // MACHINEINFO
  2, // PINRGB
  2, // RGBOUT
  0, // WIFISCAN
  4, // WIFIGET
  3, // WIFISOCKETS
  2, // VOPSOPENMSG
  3, // WIFILOGINTEST
  4, // WIFILOGIN
  0, // WIFISTATION
  2, // WIFICONNECT
  2, // BTSTART
};

// these indexes match the other 2 arrays.
void Logo::callbuiltin(short index) {
  switch (index) {
    case 0: LogoWords::err(*this); break;
    case 1: LogoWords::make(*this); break;
    case 2: LogoWords::forever(*this); break;
    case 3: LogoWords::repeat(*this); break;
    case 4: LogoWords::ifelse(*this); break;
    case 5: LogoWords::ifWord(*this); break;
    case 6: LogoWords::wait(*this); break;
    case 7: LogoWords::eq(*this); break;
    case 8: LogoWords::neq(*this); break;
    case 9: LogoWords::subtract(*this); break;
    case 10: LogoWords::add(*this); break;
    case 11: LogoWords::divide(*this); break;
    case 12: LogoWords::multiply(*this); break;
    case 13: LogoWords::gt(*this); break;
    case 14: LogoWords::gte(*this); break;
    case 15: LogoWords::lt(*this); break;
    case 16: LogoWords::lte(*this); break;
    case 17: LogoWords::dumpvars(*this); break;
    case 18: LogoWords::print(*this); break;
    case 19: LogoWords::notWord(*this); break;
    case 20: LogoWords::dread(*this); break;
    case 21: LogoWords::dhigh(*this); break;
    case 22: LogoWords::dlow(*this); break;
    case 23: LogoWords::pinout(*this); break;
    case 24: LogoWords::pinin(*this); break;
    case 25: LogoWords::aout(*this); break;
    case 26: LogoWords::thing(*this); break;
    case 27: LogoWords::output(*this); break;
    case 28: LogoWords::wordWord(*this); break;
    case 29: LogoWords::first(*this); break;
    case 30: LogoWords::pininup(*this); break;
    case 31: LogoWords::type(*this); break;
    case 32: LogoWords::machineinfo(*this); break;
    case 33: LogoWords::pinrgb(*this); break;
    case 34: LogoWords::rgbout(*this); break;
    case 35: LogoWords::wifiscan(*this); break;
    case 36: LogoWords::wifiget(*this); break;
    case 37: LogoWords::wifisockets(*this); break;
    case 38: LogoWords::vopsopenmsg(*this); break;
    case 39: LogoWords::wifilogintest(*this); break;
    case 40: LogoWords::wifilogin(*this); break;
    case 41: LogoWords::wifistation(*this); break;
    case 42: LogoWords::wificonnect(*this); break;
    case 43: LogoWords::btstart(*this); break;
    default:
      break;
  }
}

#endif // H_logowordnames
