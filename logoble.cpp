/*
  logoble.cpp
    
  Author: Paul Hamilton (paul@visualops.com)
  Date: 14-Jul-2023
  
  This work is licensed under the Creative Commons Attribution 4.0 International License. 
  To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/ or 
  send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  https://github.com/visualopsholdings/tinylogo
*/

#include "logo.hpp"
#include "logosketchbase.hpp"

#include <Arduino.h>

#if defined(ESP32) && defined(USE_BT)
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
using namespace std;
#endif

void LogoBLE::loop() {

#if defined(ESP32) && defined(USE_BT)
  // disconnecting
  if (!_btconnected && _btoldconnected) {
      delay(500); // give the bluetooth stack the chance to get things ready
      _btserver->startAdvertising(); // restart advertising
      _btoldconnected = _btconnected;
  }
  // connecting
  if (_btconnected && !_btoldconnected) {
      // do stuff here on connecting
      _btoldconnected = _btconnected;
  }
#endif

}

#if defined(ESP32) && defined(USE_BT)

class MyServerCallbacks: public BLEServerCallbacks {

public:
  MyServerCallbacks(Logo *logo): _logo(logo) {}
  
  // BLEServerCallbacks
  void onConnect(BLEServer* pServer) {
    _logo->_ble._btconnected = true;
    BLEDevice::startAdvertising();
  }
  void onDisconnect(BLEServer* pServer) {
    _logo->_ble._btconnected = false;
  }
  
private:
  Logo *_logo;
};

class CompileCharCallbacks: public BLECharacteristicCallbacks {

public:
  CompileCharCallbacks(LogoSketchBase *sketch): _sketch(sketch) {}
  
  // BLECharacteristicCallbacks
  void onWrite(BLECharacteristic *pCharacteristic) {
  
    string val = pCharacteristic->getValue();
    Serial.println(val.c_str());
    _sketch->docompile(val.c_str());
    
  }
  
private:
  LogoSketchBase *_sketch;
};

class RunCharCallbacks: public BLECharacteristicCallbacks {

public:
  RunCharCallbacks(Logo *logo): _logo(logo) {}
  
  // BLECharacteristicCallbacks
  void onWrite(BLECharacteristic *pCharacteristic) {
  
    string val = pCharacteristic->getValue();
    Serial.println(val.c_str());
    if (val == "RESTART") {
      _logo->restart();
    }
    else if (val == "RESETCODE") {
      _logo->resetcode();
    }
    else if (val == "RESET") {
      _logo->reset();
    }
     
  }
  
private:
  Logo *_logo;
};
#endif

#define COMPILE_UUID  "c807fa87-21a3-47b2-a0f8-9cd1da3407b7"
#define RUN_UUID      "9de01783-a173-4d85-a7f6-48c7a82c6a63"

void LogoBLE::start(Logo *logo, LogoSketchBase *sketch, const char *sname, const char *suid) {

#if defined(ESP32) && defined(USE_BT)
  // Create the BLE Device
  BLEDevice::init(sname);

  // Create the BLE Server
  _btserver = BLEDevice::createServer();
  _btserver->setCallbacks(new MyServerCallbacks(logo));

  // Create the BLE Service
  BLEService *service = _btserver->createService(suid);

  // Create a BLE Characteristic for the compiler
  _btcompilechar = service->createCharacteristic(COMPILE_UUID,
                    BLECharacteristic::PROPERTY_WRITE);

  _btcompilechar->setCallbacks(new CompileCharCallbacks(sketch));

  // Create a BLE Characteristic for the runtime
  _btrunchar = service->createCharacteristic(RUN_UUID,
                    BLECharacteristic::PROPERTY_WRITE);

  _btrunchar->setCallbacks(new RunCharCallbacks(logo));

  // Start the service
  service->start();

  // Start advertising
  BLEAdvertising *advert = BLEDevice::getAdvertising();
  advert->addServiceUUID(suid);
  advert->setScanResponse(false);
  advert->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
#endif

}
