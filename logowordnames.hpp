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

// all the core words are here. To make it smaller you can comment them
// in and out here.
LogoBuiltinWord Logo::core[] = {
  { "ERR", LogoWords::err, LogoWords::errArity },
  { "MAKE", LogoWords::make, LogoWords::makeArity },
  { "FOREVER", LogoWords::forever, LogoWords::foreverArity },
  { "REPEAT", LogoWords::repeat, LogoWords::repeatArity },
  { "IFELSE", LogoWords::ifelse, LogoWords::ifelseArity },
  { "IF", LogoWords::ifWord, LogoWords::ifArity },
  { "WAIT", LogoWords::wait, LogoWords::waitArity },
  { "=", LogoWords::eq, LogoWords::eqArity },
  { "!=", LogoWords::neq, LogoWords::neqArity },
  { "-", LogoWords::subtract, LogoWords::subtractArity },
  { "+", LogoWords::add, LogoWords::addArity },
  { "/", LogoWords::divide, LogoWords::divideArity },
  { "*", LogoWords::multiply, LogoWords::multiplyArity },
  { ">", LogoWords::gt, LogoWords::gtArity },
  { ">=", LogoWords::gte, LogoWords::gteArity },
  { "<", LogoWords::lt, LogoWords::ltArity },
  { "<=", LogoWords::lte, LogoWords::lteArity },
  { "DUMPVARS", LogoWords::dumpvars, LogoWords::dumpvarsArity },
  { "PRINT", LogoWords::print, LogoWords::printArity },
  { "!", LogoWords::notWord, LogoWords::notArity },
  { "DREAD", LogoWords::dread, LogoWords::dreadArity },
  { "DHIGH", LogoWords::dhigh, LogoWords::dhighArity },
  { "DLOW", LogoWords::dlow, LogoWords::dlowArity },
  { "PINOUT", LogoWords::pinout, LogoWords::pinoutArity },
  { "PININ", LogoWords::pinin, LogoWords::pininArity },
  { "AOUT", LogoWords::aout, LogoWords::aoutArity },
  { "THING", LogoWords::thing, LogoWords::thingArity },
  { "OUTPUT", LogoWords::output, LogoWords::outputArity },
  { "WORD", LogoWords::wordWord, LogoWords::wordArity },
  { "FIRST", LogoWords::first, LogoWords::firstArity },
  { "PININUP", LogoWords::pininup, LogoWords::pininupArity },
};

#endif // H_logowordnames
