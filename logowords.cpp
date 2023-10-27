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
#include <Arduino.h>
#include <HardwareSerial.h>

#ifdef ESP32

char gBuff[256]; // a generic buffer
#include "ringbuffer.hpp"
extern RingBuffer gBuffer;

#ifdef USE_WIFI
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
WiFiClientSecure gSecureClient;
String gCookie;
#include <SocketIOclient.h>
SocketIOclient gWSClient;
char gMsg[256]; // the open socket message.
#endif // USE_WIFI

#endif // ESP32

#else

#include <iostream>
#include <string>

#endif

#ifdef USE_JSON
#include <ArduinoJson.h>
#endif

#ifdef USE_MIDI
#include "MIDIUSB.h"
#endif

using namespace std;

void LogoWords::err(Logo &logo) {

  logo.fail(LG_STOP);
  
}

void LogoWords::make(Logo &logo) {

  if (logo.isstackint()) {
  
    short n = logo.popint();
    LogoStringResult result;
    logo.popstring(&result);
    short var = logo.findvariable(&result);
    if (var < 0) {
      var = logo.newintvar(logo.addstring(&result), result.length(), n);
    }
    else {
      logo.setintvar(var, n);    
    }
  }
  else if (logo.isstackstring()) {
  
    LogoStringResult value;
    logo.popstring(&value);
    LogoStringResult result;
    logo.popstring(&result);
    short var = logo.findvariable(&result);
    if (var < 0) {
      var = logo.newstringvar(logo.addstring(&result), result.length(), logo.addstring(&value), value.length());
    }
    else {
      logo.setstringvar(var, logo.addstring(&value), value.length());    
    }
  }
  else if (logo.isstackdouble()) {
  
    double n = logo.popdouble();
    LogoStringResult result;
    logo.popstring(&result);
    short var = logo.findvariable(&result);
    if (var < 0) {
      var = logo.newdoublevar(logo.addstring(&result), result.length(), n);
    }
    else {
      logo.setdoublevar(var, n);    
    }
  }
  else if (logo.isstacklist()) {
  
    List l = logo.poplist();
    LogoStringResult result;
    logo.popstring(&result);
    short var = logo.findvariable(&result);
    if (var < 0) {
      var = logo.newlistvar(logo.addstring(&result), result.length(), l);
    }
    else {
      logo.setlistvar(var, l);    
    }
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
          Serial.flush();
#else
          logo.out() << " ";
#endif
        }
#ifdef ARDUINO
        Serial.print(s);
        Serial.flush();
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
    Serial.flush();
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
  logo.out() << "d read pin " << pin << endl;
  logo.pushint(0);
#endif
  
}

void LogoWords::dhigh(Logo &logo) {

  int pin = logo.popint();
#ifdef ARDUINO
  digitalWrite(pin, HIGH);
#else
  logo.out() << "d write high " << pin << endl;
#endif
  
}

void LogoWords::dlow(Logo &logo) {

  int pin = logo.popint();
#ifdef ARDUINO
  digitalWrite(pin, LOW);
#else
  logo.out() << "d write low " << pin << endl;
#endif
  
}

void LogoWords::pinout(Logo &logo) {

  int pin = logo.popint();
#ifdef ARDUINO
  pinMode(pin, OUTPUT);
#else
  logo.out() << "pin out " << pin << endl;
#endif
  
}

void LogoWords::pinin(Logo &logo) {

  int pin = logo.popint();
#ifdef ARDUINO
  pinMode(pin, INPUT);
#else
  logo.out() << "pin in " << pin << endl;
#endif
  
}

void LogoWords::pininup(Logo &logo) {

  int pin = logo.popint();
#ifdef ARDUINO
  pinMode(pin, INPUT_PULLUP);
#else
  logo.out() << "pin in with pullup " << pin << endl;
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
  logo.out() << "aout " << pin << "=" << value << endl;
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
  logo.out() << "rgb pin " << pin << " [" << channel << "]" << endl;
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
  logo.out() << "rgbout " << pin << " [" << channel << "]" << "=" << value << endl;
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
    Serial.flush();
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
#endif
#else
  logo.out() << "Wifi Station" << endl;
#endif

}

void LogoWords::wificonnect(Logo &logo) {

  LogoStringResult password;
  logo.popstring(&password);
  
  LogoStringResult ap;
  logo.popstring(&ap);
  
#ifdef ARDUINO
#if defined(ESP32) && defined(USE_WIFI)

  // we put the SSID in gBuff, and need a password buffer.
  char passbuf[128];
    
  password.ncpyesc(passbuf, sizeof(passbuf));
  ap.ncpyesc(gBuff, sizeof(gBuff));

  Serial.print("Wifi: Connecting [");
  Serial.print(gBuff);
  Serial.print("] [");
  Serial.print(passbuf);
  Serial.println("]");
  
  WiFi.begin(gBuff, passbuf);
  bool finished = false;
  bool success = false;
  int discon = 0;
  int i=0;
  while (!finished && i<100) {
    int status = WiFi.status();
    switch (status) {
    case WL_CONNECTED:
      finished = true;
      success = true;
      break;
    case WL_IDLE_STATUS:
      Serial.print('.');
      break;
    case WL_DISCONNECTED:
      Serial.print('-');
      discon++;
      break;
    case WL_CONNECT_FAILED:
      logo.throwException("Connect failed");
      finished = true;
      break;
    case WL_CONNECTION_LOST:
      logo.throwException("Connection lost");
      finished = true;
      break;
    case WL_NO_SHIELD:
      logo.throwException("No Wifi Shield");
      finished = true;
      break;
    case WL_NO_SSID_AVAIL:
      logo.throwException("No SSID available");
      finished = true;
      break;
    default:
      Serial.print("Unknown status ");
      Serial.println(status);
    }
    delay(400);
    i++;
    if ((i % 80) == 0) {
      Serial.println();
    }
    // every 20 disconects, try to connect again.
    if (discon > 20) {
      Serial.print('+');
      WiFi.disconnect();
      WiFi.begin(gBuff, passbuf);
      discon = 0;
    }
  };
  if (!success) {
    if (!finished) {
      logo.throwException("Connection timed out");
    }
    return;
  }
  LogoSimpleString str(WiFi.localIP().toString().c_str());
  logo.pushstring(&str);
#else
  logo.throwException("Wifi not supported");
#endif
#else
  LogoSimpleString str("0.0.0.0");
  logo.pushstring(&str);
#endif

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
  logo.pushlist(l);
#else
  logo.throwException("Wifi not supported");
#endif
#else
  for (int i=0; i<4; i++) {
    LogoSimpleString s("wifi");
    ListNodeVal val(LTYPE_STRING, logo.addstring(&s, 0, s.length()), s.length());
    l.push(val);
  }
  logo.pushlist(l);
#endif // ARDUINO
  
}

void LogoWords::wifiget(Logo &logo) {

  LogoStringResult field;
  logo.popstring(&field);
  
  LogoStringResult request;
  logo.popstring(&request);
  
  int port = logo.popint();
  
  LogoStringResult host;
  logo.popstring(&host);
  
#ifdef ARDUINO
#if defined(ESP32) && defined(USE_WIFI)
  
  if (WiFi.status() != WL_CONNECTED) {
    logo.throwException("Wifi not connected");
    return;
  }
  
  HTTPClient http;

  host.ncpy(gBuff, sizeof(gBuff));

  if (port == 443 || port == 8443) {

    if (gSecureClient.connect(gBuff, port) < 0) {
      gSecureClient.lastError(gBuff, sizeof(gBuff));
      logo.throwException(gBuff);
      return;
    }

    strcpy(gBuff, "https://");
    int len = strlen(gBuff);
    host.ncpy(gBuff + len, sizeof(gBuff) - len);
    if (port != 443) {
      strcat(gBuff, ":");
      int len = strlen(gBuff);
      snprintf(gBuff + len, sizeof(gBuff) - len, "%d", port);
    }
    len = strlen(gBuff);
    request.ncpy(gBuff + len, sizeof(gBuff) - len);

    if (!http.begin(gSecureClient, gBuff)) {
      logo.throwException("Failed to begin request");
      return;
    }
  }
  else {
    strcpy(gBuff, "http://");
    int len = strlen(gBuff);
    host.ncpy(gBuff + len, sizeof(gBuff) - len);
    if (port != 80) {
      strcat(gBuff, ":");
      int len = strlen(gBuff);
      snprintf(gBuff + len, sizeof(gBuff) - len, "%d", port);
    }
    len = strlen(gBuff);
    request.ncpy(gBuff + len, sizeof(gBuff) - len);
    
    if (!http.begin(gBuff)) {
      logo.throwException("Failed to begin request");
      return;
    }
  }
  
  http.addHeader("Cookie", gCookie);   
  
  int code = http.GET();
  if (code <= 0) {
    Serial.print("HTTPClient: ");
    Serial.println(code);
    logo.throwException("HTTP failed");
    return;
  }
  
  if (code != HTTP_CODE_OK && code != 302) {
    strcpy(gBuff, "HTTP Error ");
    int len = strlen(gBuff);
    snprintf(gBuff + len, sizeof(gBuff) - len, "%d", code);
    logo.throwException(gBuff);
    return;
  }
  
  // get result and convert to JSON.
  String result = http.getString();
  DynamicJsonDocument doc(result.length());
  deserializeJson(doc, result.c_str());
  
  // return the field requested
  field.ncpy(gBuff, sizeof(gBuff));
  const char *val = doc[gBuff];
  if (!val) {
    logo.throwException("Field not found");
    return;
  }

  LogoSimpleString str(val);
  logo.pushstring(&str);  
#else
  logo.throwException("Wifi not supported");
#endif
#else
  LogoSimpleString str("Wifi Sending");
  logo.pushstring(&str);
#endif // ARDUINO

}

bool LogoWords::extractEventName(LogoSimpleString *s, char *name, int namelen) {

#ifdef USE_JSON
  DynamicJsonDocument doc(256);
  deserializeJson(doc, s->c_str());

  if (!doc.is<JsonArray>()) {
    return false;
  }
  JsonArray arr = doc.as<JsonArray>();
  if (arr.size() < 2) {
    return false;
  }
  if (!arr[0].is<const char *>()) {
    return false;
  }
  
  const char *n = arr[0].as<const char *>();
  int len = strlen(n);
  if (namelen < len) {
    return false;
  }
  strcpy(name, n);
    
  return true;
#else
  return false;
#endif
}

#ifdef ARDUINO
#if defined(ESP32) && defined(USE_WIFI)
void webSocketEvent(socketIOmessageType_t type, uint8_t * payload, size_t length) {

  switch (type) {

    case sIOtype_CONNECT:
      {
        if (gMsg[0]) {
          gWSClient.sendEVENT(gMsg);
        }
      }
      break;
      
    case sIOtype_EVENT:
      {
        char s[32];
        LogoSimpleString evt((const char*)payload, length);
        if (LogoWords::extractEventName(&evt, s, sizeof(s))) {
          if (strcmp(s, "update") == 0) {
            gBuffer.write("FLASH\n");
          }
        }
      }
      break;
      
  }

}
#endif
#endif

void LogoWords::wifisockets(Logo &logo) {

  LogoStringResult openmsg;
  logo.popstring(&openmsg);
  
  int port = logo.popint();
  
  LogoStringResult host;
  logo.popstring(&host);
  
#ifdef ARDUINO
#if defined(ESP32) && defined(USE_WIFI)

  host.ncpy(gBuff, sizeof(gBuff));
  openmsg.ncpy(gMsg, sizeof(gMsg));

	if (port == 443 || port == 8443) {
    gWSClient.beginSSL(gBuff, port);
	}
	else {
    gWSClient.begin(gBuff, port);
	}

	// event handler
	gWSClient.onEvent(webSocketEvent);

  strcpy(gBuff, "Cookie: ");
  strcat(gBuff, gCookie.c_str());
  gWSClient.setExtraHeaders(gBuff);

	// try ever 5000 again if connection has failed
  gWSClient.setReconnectInterval(5000);
#else
  logo.throwException("Wifi not supported");
#endif
#else
  logo.out() << "Wifi Sockets " << endl;
#endif // ARDUINO

}

void LogoWords::vopsopenmsg(Logo &logo) {

  LogoStringResult docid;
  logo.popstring(&docid);
  
  LogoStringResult userid;
  logo.popstring(&userid);
  
#ifdef ARDUINO
#if defined(ESP32) && defined(USE_WIFI)
  strcpy(gBuff, "[ \"openDocuments\", { \"docs\": [ { \"id\": \"");
  int len = strlen(gBuff);
  docid.ncpy(gBuff + len, sizeof(gBuff) - len);
  strcat(gBuff, "\", \"type\": \"stream\" } ], \"userid\": \"");
  len = strlen(gBuff);
  userid.ncpy(gBuff + len, sizeof(gBuff) - len);
  strcat(gBuff, "\" } ]");

  LogoSimpleString s(gBuff);
  logo.pushstring(&s);
#else
  logo.throwException("Wifi not supported");
#endif
#else
  LogoSimpleString s("Make open message");
  logo.pushstring(&s);
#endif
  
}

void LogoWords::wifilogintest(Logo &logo) {

  LogoStringResult username;
  logo.popstring(&username);
  
  int port = logo.popint();
  
  LogoStringResult host;
  logo.popstring(&host);
  
#ifdef ARDUINO
#if defined(ESP32) && defined(USE_WIFI)

  if (WiFi.status() != WL_CONNECTED) {
    logo.throwException("Wifi not connected");
     return;
  }
  
  if (port == 443 || port == 8443) {
    logo.throwException("Test login is not secure.");
    return;
  }
  
  HTTPClient http;

  host.ncpy(gBuff, sizeof(gBuff));

  strcpy(gBuff, "http://");
  int len = strlen(gBuff);
  host.ncpy(gBuff + len, sizeof(gBuff) - len);
  if (port != 80) {
    strcat(gBuff, ":");
    int len = strlen(gBuff);
    snprintf(gBuff + len, sizeof(gBuff) - len, "%d", port);
  }
  strcat(gBuff, "/login/?username=");
  len = strlen(gBuff);
  username.ncpy(gBuff + len, sizeof(gBuff) - len);
  
  Serial.println(gBuff);
  
  if (!http.begin(gBuff)) {
    logo.throwException("Failed to begin request");
    return;
  }
  
  const char *headerKeys[] = {"Set-Cookie"};
  http.collectHeaders(headerKeys, 1);
  
  int code = http.GET();
  if (code <= 0) {
    Serial.print("HTTPClient: ");
    Serial.println(code);
    logo.throwException("HTTP failed");
    return;
  }
  
  // should be a redrect.
  if (code != 302) {
    strcpy(gBuff, "HTTP Error ");
    int len = strlen(gBuff);
    snprintf(gBuff + len, sizeof(gBuff) - len, "%d", code);
    logo.throwException(gBuff);
    return;
  }
  
  // really just want the cookie from the header
  if (!http.hasHeader("Set-Cookie")) {
    logo.throwException("Cookie missing");
    return;
  }

  gCookie = http.header("Set-Cookie");

  LogoSimpleString str(gCookie.c_str());
  logo.pushstring(&str);
#else
  logo.throwException("Wifi not supported");
#endif
#else
  LogoSimpleString str("cookie");
  logo.pushstring(&str);
#endif
  
}

void LogoWords::wifilogin(Logo &logo) {

  LogoStringResult password;
  logo.popstring(&password);
  
  LogoStringResult username;
  logo.popstring(&username);
  
  int port = logo.popint();
  
  LogoStringResult host;
  logo.popstring(&host);
  
#ifdef ARDUINO
#if defined(ESP32) && defined(USE_WIFI)

  host.ncpy(gBuff, sizeof(gBuff));
  
  if (WiFi.status() != WL_CONNECTED) {
    logo.throwException("Wifi not connected");
    return;
  }
  
  HTTPClient http;

  if (gSecureClient.connect(gBuff, port) < 0) {
    gSecureClient.lastError(gBuff, sizeof(gBuff));
    logo.throwException(gBuff);
    return;
  }

  strcpy(gBuff, "https://");
  int len = strlen(gBuff);
  host.ncpy(gBuff + len, sizeof(gBuff) - len);
  strcat(gBuff, "/login");

  if (!http.begin(gSecureClient, gBuff)) {
    logo.throwException("Failed to begin request");
    return;
  }

  const char *headerKeys[] = {"Set-Cookie"};
  http.collectHeaders(headerKeys, 1);
  
  http.addHeader("Content-Type", "application/json");   

  strcpy(gBuff, "{ \"name\": \"");
  len = strlen(gBuff);
  username.ncpy(gBuff + len, sizeof(gBuff) - len);
  strcat(gBuff, "\", \"password\": \"");
  len = strlen(gBuff);
  password.ncpy(gBuff + len, sizeof(gBuff) - len);
  strcat(gBuff, "\", \"insecure\": false }");  
  
  int code = http.POST((uint8_t *)gBuff, strlen(gBuff));
  if (code <= 0) {
    Serial.print("HTTPClient: ");
    Serial.println(code);
    logo.throwException("HTTP failed");
    return;
  }
  
  // should be a redrect.
  if (code != 200) {
    strcpy(gBuff, "HTTP Error ");
    int len = strlen(gBuff);
    snprintf(gBuff + len, sizeof(gBuff) - len, "%d", code);
    logo.throwException(gBuff);
    return;
  }
  
  // really just want the cookie from the header
  if (!http.hasHeader("Set-Cookie")) {
    logo.throwException("Cookie missing");
    return;
  }

  gCookie = http.header("Set-Cookie");
  
  LogoSimpleString str(gCookie.c_str());
  logo.pushstring(&str);
#else
  logo.throwException("Wifi not supported");
#endif
#else
  LogoSimpleString str("cookie");
  logo.pushstring(&str);
#endif
  
}

void LogoWords::btstart(Logo &logo) {

  LogoStringResult serviceuid;
  logo.popstring(&serviceuid);
  
  LogoStringResult name;
  logo.popstring(&name);
  
#ifdef ARDUINO
#if defined(ESP32) && defined(USE_BT)

  char sname[32];

  name.ncpy(sname, sizeof(sname));
  serviceuid.ncpy(gBuff, sizeof(gBuff));
  
  logo._ble.start(&logo, logo._sketch, sname, gBuff);
  
  Serial.print("BLE: Waiting for a client connection: ");
  Serial.println(sname);

#else
  logo.throwException("BLE not supported");
#endif
#else
  logo.out() << "Start Bluetooth" << endl;
#endif
  
}

void LogoWords::throwWord(Logo &logo) {

  logo.doThrow();
  
}

void LogoWords::intWord(Logo &logo) {

  logo.pushint((int)logo.popdouble());
  
}

void LogoWords::midinoteon(Logo &logo) {

  int velocity = logo.popint();
  int pitch = logo.popint();
  int channel = logo.popint();
#ifdef USE_MIDI
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
  MidiUSB.flush();
#else
#ifdef ARDUINO
  Serial.println("MIDI Note ON");
#else
  logo.out() << "MIDI Note On " << channel << " " << pitch << " " << velocity << endl;
#endif
#endif

}

void LogoWords::midinoteoff(Logo &logo) {

  int pitch = logo.popint();
  int channel = logo.popint();
#ifdef USE_MIDI
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, 0};
  MidiUSB.sendMIDI(noteOff);
  MidiUSB.flush();
#else
#ifdef ARDUINO
  Serial.println("MIDI Note OFF");
#else
  logo.out() << "MIDI Note Off " << channel << " " << pitch << endl;
#endif
#endif

}

