/*
  logostring.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 5-May-2023
    
  strings are in an abstract class to allow them to be placed in FLASH memory 
  if we need to.
  
  This work is licensed under the Creative Commons Attribution 4.0 International License. 
  To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/ or 
  send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  https://github.com/visualopsholdings/tinylogo
*/

#ifndef H_logostring
#define H_logostring

#include "logo.hpp"

#include <string.h>

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

class LogoString {

public:
  
  virtual size_t length() const = 0;
  virtual char operator[](int index) const = 0;
  virtual void ncpy(char *to, size_t offset, size_t len) const = 0;
  virtual int ncmp(const char *to, size_t offset, size_t len) const = 0;
  virtual int ncasecmp(const char *to, size_t offset, size_t len) const = 0;
 
  short toi(size_t offset, size_t len);
  short find(char c, size_t offset, size_t len);
  short ncmp2(LogoString *to, short offsetto, short offset, short len) const;
  void dump(const char *msg, short start, short len) const;

};

// just a simple string as a sequence of characters, null terminated.

class LogoSimpleString: public LogoString {

public:
  LogoSimpleString(const char *code, short len): _code(code), _len(len) {}
  LogoSimpleString(const char *code): _code(code) {
    _len = strlen(_code);
  }
  LogoSimpleString(): _code(0), _len(0) {}
  
  void set(const char *code, short len) {
    _code = code;
    _len = len;
  }
  
  size_t length() const { 
    return _len; 
  }
  
  char operator[](int index) const { 
    return _code[index]; 
  }
  
  void ncpy(char *to, size_t offset, size_t len) const ;
  int ncmp(const char *to, size_t offset, size_t len) const;
  int ncasecmp(const char *to, size_t offset, size_t len) const;
  double tof();
    
private:
  const char *_code;
  short _len;
  
};

class LogoStringResult {

public:
  LogoStringResult() : _fixed(0), _fixedstart(0), _fixedlen(0) {}
  
  void init() {
    _simple.set(0, 0);
    _fixed = 0;
  }
  
  size_t length() const;
  int ncmp(const char *to);
  void ncpy(char *to, int len);
  short toi();
  double tof();
  
  LogoSimpleString _simple;
  LogoString *_fixed;
  short _fixedstart;
  short _fixedlen;
};

#endif // H_logostring
