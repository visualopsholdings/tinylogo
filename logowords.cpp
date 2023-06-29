/*
  logo.cpp
    
  Author: Paul Hamilton (paul@visualops.com)
  Date: 5-May-2023
  
  This work is licensed under the Creative Commons Attribution 4.0 International License. 
  To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/ or 
  send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  https://github.com/visualopsholdings/tinylogo
*/

#include "logowords.hpp"
#include "logo.hpp"
#include "logostring.hpp"

#ifdef ARDUINO
#include <HardwareSerial.h>
#include <Arduino.h>
#else
#include <iostream>
using namespace std;
#endif

void LogoWords::err(Logo &logo) {

  logo.fail(LG_STOP);
  
}

void LogoWords::make(Logo &logo) {

  short n = logo.popint();
  LogoStringResult result;
  logo.popstring(&result);
  
// #ifdef ARDUINO
//   char s[256];
//   result.ncpy(s, sizeof(s));
//   Serial.println(s);
//   Serial.println(n);
// #endif
  
  short var = logo.findvariable(&result);
  if (var < 0) {
    var = logo.newintvar(logo.addstring(&result), result.length(), n);
  }
  else {
    logo.setintvar(var, n);    
  }

}

void LogoWords::thing(Logo &logo) {

  LogoStringResult result;
  logo.popstring(&result);
  short var = logo.findvariable(&result);
  if (var >= 0 && logo.varisint(var)) {
    logo.pushint(logo.varintvalue(var));
  }
  else {
    logo.pushint(0);
  }

}

void LogoWords::forever(Logo &logo) {

  // change the next return address to be 1 minus what we have 
  logo.modifyreturn(-1, -1);

}

void LogoWords::repeat(Logo &logo) {

  short n = logo.popint();

  // change the next return address to be 1 minus what we have 
  logo.modifyreturn(-1, n);

}

bool LogoWords::pushliterals(Logo &logo, short rel) {

  if (logo.codeisint(rel)) {
    logo.pushint(logo.codetoint(rel));
    return true;
  }
  else if (logo.codeisstring(rel)) {
    tStrPool s, l;
    logo.codetostring(rel, &s, &l);
    if (logo.geterr()) {
      return false;
    }
    logo.pushstring(s, l);
    return true;
  }

  return false;
  
}

void LogoWords::ifelse(Logo &logo) {

//  logo.dump("ifelse", false);
  
  // this is a very special trick to allow us to manipulate the PC
  // based on the value of the stack.
  //
  // our code looks like:
	//  builtin IFELSE		(pc)
	//  TEST
	//  THEN
	//  ELSE
	//
  int cond = logo.popint();
  if (cond) {
    if (pushliterals(logo, 0)) {
      logo.jump(3);
    }
    else {
      logo.jumpskip(1);
    }
  }
  else {
    if (pushliterals(logo, 1)) {
      logo.jump(3);
    }
    else {
      logo.jump(2);
    }
  }
    
}

void LogoWords::ifWord(Logo &logo) {

  // this is a very special trick to allow us to manipulate the PC
  // based on the value of the stack.
  //
  // our code looks like:
	//  builtin IF		(pc)
	//  TEST
	//  THEN
	//
  int cond = logo.popint();
  if (cond) {
    if (pushliterals(logo, 0)) {
      logo.jump(2);
    }
  }
  else {
    logo.jump(2);
  }

}

void LogoWords::wait(Logo &logo) {

  // schedule the next word to execute after a certain time.
  logo.schedulenext(logo.popint());
  
}

void LogoWords::eq(Logo &logo) {

  logo.pushint(logo.popint() == logo.popint());
  
}

void LogoWords::neq(Logo &logo) {

  logo.pushint(logo.popint() != logo.popint());
  
}

void LogoWords::gt(Logo &logo) {

  // args are backwards.
  double i1 = logo.popdouble();
  double i2 = logo.popdouble();
  logo.pushint(i2 > i1);
  
}

void LogoWords::lt(Logo &logo) {

  // args are backwards.
  double i1 = logo.popdouble();
  double i2 = logo.popdouble();
  logo.pushint(i2 < i1);
  
}

void LogoWords::gte(Logo &logo) {

  // args are backwards.
  double i1 = logo.popdouble();
  double i2 = logo.popdouble();
  logo.pushint(i2 >= i1);
  
}

void LogoWords::lte(Logo &logo) {

  // args are backwards.
  double i1 = logo.popdouble();
  double i2 = logo.popdouble();
  logo.pushint(i2 <= i1);
  
}

void LogoWords::notWord(Logo &logo) {

  logo.pushint(!logo.popint());
  
}

void LogoWords::subtract(Logo &logo) {

  // args are backwards.
  double a1 = logo.popdouble();
  double a2 = logo.popdouble();
  logo.pushdouble(a2 - a1);
  
}

void LogoWords::add(Logo &logo) {

  double a1 = logo.popdouble();
  double a2 = logo.popdouble();
  logo.pushdouble(a2 + a1);
  
}

void LogoWords::divide(Logo &logo) {

  double a1 = logo.popdouble();
  double a2 = logo.popdouble();
  if (a1 == 0) {
    logo.pushdouble(0);
    return;
  }
  logo.pushdouble(a2 / a1);
  
}

void LogoWords::multiply(Logo &logo) {

  double a1 = logo.popdouble();
  double a2 = logo.popdouble();
  logo.pushdouble(a2 * a1);
  
}

void LogoWords::dumpvars(Logo &logo) {

#ifndef ARDUINO
  logo.dumpvars();
#endif
    
}

void LogoWords::print(Logo &logo) {

  LogoStringResult result;
  char s[256];

  if (logo.isstacklist()) {
    List l = logo.poplist();
    ListNodeVal val;
    tNodeType node = l.head();
#ifndef ARDUINO
    logo.out() << "=== ";
#endif
    bool first = true;
    while (l.iter(&node, &val)) {
      if (logo.getlistval(val, &result)) {
        result.ncpy(s, sizeof(s));
        if (!first) {
#ifdef ARDUINO
          Serial.print(' '');
#else
          logo.out() << " ";
#endif
        }
#ifdef ARDUINO
        Serial.print(s);
#else
        logo.out() << s;
#endif
      }
      first = false;
    }
#ifndef ARDUINO
    logo.out() << endl;
#endif
  }
  else {
    logo.popstring(&result);
    result.ncpy(s, sizeof(s));
#ifdef ARDUINO
    Serial.println(s);
#else
    logo.out() << "=== " << s << endl;
#endif
  }
}

void LogoWords::dread(Logo &logo) {

  int pin = logo.popint();
#ifdef ARDUINO
  logo.pushint(digitalRead(pin));
#else
  cout << "d read pin " << pin << endl;
  logo.pushint(0);
#endif
  
}

void LogoWords::dhigh(Logo &logo) {

  int pin = logo.popint();
#ifdef ARDUINO
  digitalWrite(pin, HIGH);
#else
  cout << "d write high " << pin << endl;
#endif
  
}

void LogoWords::dlow(Logo &logo) {

  int pin = logo.popint();
#ifdef ARDUINO
  digitalWrite(pin, LOW);
#else
  cout << "d write low " << pin << endl;
#endif
  
}

void LogoWords::pinout(Logo &logo) {

  int pin = logo.popint();
#ifdef ARDUINO
  pinMode(pin, OUTPUT);
#else
  cout << "pin out " << pin << endl;
#endif
  
}

void LogoWords::pinin(Logo &logo) {

  int pin = logo.popint();
#ifdef ARDUINO
  pinMode(pin, INPUT);
#else
  cout << "pin in " << pin << endl;
#endif
  
}

void LogoWords::pininup(Logo &logo) {

  int pin = logo.popint();
#ifdef ARDUINO
  pinMode(pin, INPUT_PULLUP);
#else
  cout << "pin in with pullup " << pin << endl;
#endif
  
}

void LogoWords::aout(Logo &logo) {

  int value = logo.popint();
  int pin = logo.popint();
#ifdef ARDUINO
#ifdef __AVR__
  analogWrite(pin, value);
#else
  // need to implement this on the w32 etc.
#endif
#else
  cout << "aout " << pin << "=" << value << endl;
#endif
  
}

void LogoWords::output(Logo &logo) {

  // just leaves whatever is on the stack on the stack.
  
}

void LogoWords::wordWord(Logo &logo) {

  LogoStringResult s1;
  logo.popstring(&s1);
  LogoStringResult s2;
  logo.popstring(&s2);
  
  char s[32];
  s2.ncpy(s, sizeof(s));
  s1.ncpy(s + s2.length(), sizeof(s) - s2.length());
  LogoSimpleString str(s);
  logo.pushstring(&str);
  
}

void LogoWords::first(Logo &logo) {

  LogoStringResult str;
  logo.popstring(&str);
  
  char s[2];
  str.ncpy(s, 1);
    
  LogoSimpleString str2(s);
  logo.pushstring(&str2);
  
}




