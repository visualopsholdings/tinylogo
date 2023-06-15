/*
  logostring.cpp
    
  Author: Paul Hamilton (paul@visualops.com)
  Date: 5-May-2023
  
  This work is licensed under the Creative Commons Attribution 4.0 International License. 
  To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/ or 
  send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  https://github.com/visualopsholdings/tinylogo
*/

#include "logostring.hpp"

#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#ifndef ARDUINO
#include <iostream>
using namespace std;
#endif

short LogoString::toi(size_t offset, size_t len) {

  short res = 0;
  short sign = 1;
  short i = offset;

  if ((*this)[i] == '-') {
      sign = -1;
      i++;
  }

  while ((*this)[i] && i < (offset + len)) {
    if (!isdigit((*this)[i])) {
      return 0;
    }
    res = res * 10 + (*this)[i] - '0';
    i++;
  }

  return sign * res;
  
}

short LogoString::find(char c, size_t offset, size_t len) {

  short i = offset;

  while ((*this)[i] && i < (offset + len)) {
    if ((*this)[i] == c) {
      return i;
    }
    i++;
  }

  return -1;
  
}

void LogoSimpleString::ncpy(char *to, size_t offset, size_t len) const { 
  strncpy(to, _code + offset, len);
  to[len] = 0;
}

int LogoSimpleString::ncmp(const char *to, size_t offset, size_t len) const { 
  return strncmp(to, _code + offset, len);
}

int LogoSimpleString::ncasecmp(const char *to, size_t offset, size_t len) const { 
  return strncasecmp(to, _code + offset, len);
}

double LogoSimpleString::tof() {
  return atof(_code);
}

short LogoString::ncmp2(LogoString *to, short offsetto, short offset, short len) const {

//    to->dump("ncmp2 to", 0, to->length());
//    dump("ncmp2", offset, len);
  
	if (len == 0 || to->length() == 0) {
		return 0;
	}
	short i = offset, j=offsetto;
	short imax = length();
	short jmax = to->length();
	do {
		if ((*this)[i] != (*to)[j]) {
      return -1;
		}
		if ((*this)[i++] == 0) {
			break;
		}
		j++;
		if (i > imax || j > jmax) {
      return -1;
		}
	} while (--len != 0);
	
	return 0;
}

size_t LogoStringResult::length() const { 
  if (_fixed) {
    return _fixedlen;
  }
  return _simple.length();
}

int LogoStringResult::ncmp(const char *to) {
  if (_fixed) {
    return _fixed->ncmp(to, _fixedstart, _fixedlen);
  }
  return _simple.ncmp(to, 0, _simple.length());
}

void LogoStringResult::ncpy(char *to, int len) {
  if (_fixed) {
    _fixed->ncpy(to, _fixedstart, min(len, _fixedlen));
    return;
  }
  _simple.ncpy(to, 0, min(len, _simple.length()));
}

short LogoStringResult::toi() {
  if (_fixed) {
    return _fixed->toi(_fixedstart, _fixedlen);
  }
  return _simple.toi(0, _simple.length());
}

double LogoStringResult::tof() {
  if (_fixed) {
    // don't know how to do this yet...
    return 0;
  }
  return _simple.tof();
}

void LogoString::dump(const char *msg, short start, short len) const {
#ifndef ARDUINO
  cout << msg << " {";
  for (int i=0, j=start; i<len; i++, j++) {
    cout << (*this)[j];
  }
  cout << "}" << endl;
#endif
}

