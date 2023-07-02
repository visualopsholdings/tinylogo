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
#if defined(ESP32) && defined(USE_WIFI)
#include <WiFi.h>
#endif
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

//  logo.dumpstack(0, false);

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
          Serial.print(' ');
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

void LogoWords::pinrgb(Logo &logo) {

  int channel = logo.popint();
  int pin = logo.popint();
  
  logo.setpin(channel, pin);
  
#ifdef ARDUINO
#ifdef ESP32
  ledcSetup(channel, 12000, 8); // 12 kHz PWM, 8-bit resolution
  ledcAttachPin(pin, channel);
#else
  pinMode(pin, OUTPUT);
#endif

#else
  cout << "rgb pin " << pin << " [" << channel << "]" << endl;
#endif
  
}

void LogoWords::rgbout(Logo &logo) {

  int value = logo.popint();
  int channel = logo.popint();
  
  int pin = logo.getpin(channel);
  
#ifdef ARDUINO
#ifdef ESP32
  ledcWrite(channel, value);
#else
  // 0 is OFF, 255 is FULL.
  analogWrite(pin, 255 - value);
#endif
#else
  cout << "rgbout " << pin << " [" << channel << "]" << "=" << value << endl;
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

void LogoWords::type(Logo &logo) {

  if (logo.isstacklist()) {
    LogoSimpleString str("LIST");
    logo.pop();
    logo.pushstring(&str);
  }
  else if (logo.isstackstring()) {
    LogoSimpleString str("STRING");
    logo.pop();
    logo.pushstring(&str);
  }
  else if (logo.isstackint()) {
    LogoSimpleString str("INTEGER");
    logo.pop();
    logo.pushstring(&str);
  }
  else if (logo.isstackdouble()) {
    LogoSimpleString str("DOUBLE");
    logo.pop();
    logo.pushstring(&str);
  }
  else {
    LogoSimpleString str("UNKNOWN");
    logo.pop();
    logo.pushstring(&str);
  }
  
}

void LogoWords::machineinfo(Logo &logo) {

#ifdef ARDUINO
    Serial.print("TinyLogo version ");
    Serial.println(MACHINE_VERSION);
    Serial.print("Strings: ");
    Serial.println(STRING_POOL_SIZE);
    Serial.print("Code: ");
    Serial.println(CODE_SIZE);
    Serial.print("Stack: ");
    Serial.println(MAX_STACK);
    Serial.print("Vars: ");
    Serial.println(MAX_VARS);
#else
    logo.out() << "TinyLogo" << endl;
    logo.out() << "Strings: " << STRING_POOL_SIZE << endl;
    logo.out() << "Code: " << CODE_SIZE << endl;
    logo.out() << "Stack: " << MAX_STACK << endl;
    logo.out() << "Vars: " << MAX_VARS << endl;
#endif
 
}

void LogoWords::wifistation(Logo &logo) {

#ifdef ARDUINO
#if defined(ESP32) && defined(USE_WIFI)
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
#else
  Serial.println("Wifi not supported");
#endif
#else
  logo.out() << "Wifi Station Mode" << endl;
#endif // ARDUINO

}

void LogoWords::wifiscan(Logo &logo) {

  List l = logo.newlist();
  
#ifdef ARDUINO
#if defined(ESP32) && defined(USE_WIFI)
  int n = WiFi.scanNetworks();
  for (int i = 0; i < n; ++i) {
    LogoSimpleString s(WiFi.SSID(i).c_str());
    ListNodeVal val(LTYPE_STRING, logo.addstring(&s, 0, s.length()), s.length());
    l.push(val);
  }
#else
  LogoSimpleString s("Wifi not supported");
  ListNodeVal val(LTYPE_STRING, logo.addstring(&s, 0, s.length()), s.length());
  l.push(val);
#endif
#else
  for (int i=0; i<4; i++) {
    LogoSimpleString s("wifi");
    ListNodeVal val(LTYPE_STRING, logo.addstring(&s, 0, s.length()), s.length());
    l.push(val);
  }
#endif // ARDUINO
  
  logo.pushlist(l);
  
}

void LogoWords::wificonnect(Logo &logo) {

  LogoStringResult pwd;
  logo.popstring(&pwd);
  LogoStringResult ap;
  logo.popstring(&ap);
  
  char aps[64];
  ap.ncpy(aps, sizeof(aps));
  char pwds[64];
  pwd.ncpy(pwds, sizeof(pwds));
  
#ifdef ARDUINO
#if defined(ESP32) && defined(USE_WIFI)
  WiFi.begin(aps, pwds);
  bool finished = false;
  bool success = false;
  while (!finished) {
    int status = WiFi.status();
    switch (status) {
    case WL_CONNECTED:
      finished = true;
      success = true;
      break;
    case WL_IDLE_STATUS:
      Serial.print('.');
      delay(1000);
      break;
    case WL_CONNECT_FAILED:
    case WL_CONNECTION_LOST:
    case WL_DISCONNECTED:
    case WL_NO_SHIELD:
      finished = true;
      break;
    }
  }
  LogoSimpleString str(success ? WiFi.localIP().toString().c_str() : "failed");
#else
  LogoSimpleString str("Wifi not supported");
#endif
#else
  LogoSimpleString str("192.168.0.1");
#endif // ARDUINO

  logo.pushstring(&str);
}





