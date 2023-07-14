/*
  logoble.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 14-Jul-2023
  
  Tiniest Logo Intepreter BLE stuff
      
  This work is licensed under the Creative Commons Attribution 4.0 International License. 
  To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/ or 
  send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  https://github.com/visualopsholdings/tinylogo
*/

#ifndef H_logoble
#define H_logoble

// the bluetooth stack on the ESP32 is just huge.
#define USE_BT

#if defined(ESP32) && defined(USE_BT)
class BLEServer;
class BLECharacteristic;
#endif

class Logo;
class LogoSketchBase;

class LogoBLE {

public:

  void start(Logo *logo, LogoSketchBase *sketch, const char *sname, const char *suid);
  void loop();

#if defined(ESP32) && defined(USE_BT)
  BLEServer* _btserver = 0;
  BLECharacteristic* _btcompilechar = 0;
  BLECharacteristic* _btrunchar = 0;
  bool _btconnected = false;
  bool _btoldconnected = false;
#endif

};

#endif // H_logoble
