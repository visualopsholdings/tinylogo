/*
  logobleclnt.cpp
    
  Author: Paul Hamilton (paul@visualops.com)
  Date: 14-Jul-2023
  
  This work is licensed under the Creative Commons Attribution 4.0 International License. 
  To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/ or 
  send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  https://github.com/visualopsholdings/tinylogo
*/

#include "logobleclnt.hpp"

#ifndef ARDUINO

#include <iostream>

using namespace std;

#define COMPILE_UUID  "c807fa87-21a3-47b2-a0f8-9cd1da3407b7"
#define RUN_UUID      "9de01783-a173-4d85-a7f6-48c7a82c6a63"

bool LogoBLEClient::send(const string &device, const string &data) {

    auto adapter_optional = getAdapter();

    if (!adapter_optional.has_value()) {
		  cout << "No adapter" << endl;
      return false;
    }

    auto adapter = adapter_optional.value();

    vector<SimpleBLE::Peripheral> peripherals;

    adapter.set_callback_on_scan_found([&](SimpleBLE::Peripheral peripheral) { peripherals.push_back(peripheral); });
    adapter.set_callback_on_scan_start([]() {});
    adapter.set_callback_on_scan_stop([]() {});    
    adapter.scan_for(500);

    for (size_t i = 0; i < peripherals.size(); i++) {
      if (peripherals[i].identifier() == device) {
        sendToPeripheral(&peripherals[i], data);
        return true;
      }
    }

    return false;
    
}

void LogoBLEClient::sendToPeripheral(SimpleBLE::Peripheral *peripheral, const string &contents) {

  peripheral->connect();
    
  for (auto service : peripheral->services()) {
    optional<SimpleBLE::BluetoothUUID> compile;
    optional<SimpleBLE::BluetoothUUID> run;
    for (auto characteristic : service.characteristics()) {
      if (characteristic.uuid() == COMPILE_UUID) {
        compile = characteristic.uuid();
      }
      else if (characteristic.uuid() == RUN_UUID) {
        run = characteristic.uuid();
      }
    }
    if (compile && run) {
      cout << "Sending code" << endl;
      peripheral->write_request(service.uuid(), *run, "RESETCODE");
      peripheral->write_request(service.uuid(), *compile, contents);
      peripheral->write_request(service.uuid(), *run, "RESTART");
      break;
    }
  }
  
  peripheral->disconnect();
}

optional<SimpleBLE::Adapter> LogoBLEClient::getAdapter() {

    if (!SimpleBLE::Adapter::bluetooth_enabled()) {
        cout << "Bluetooth is not enabled!" << endl;
        return {};
    }

    auto adapter_list = SimpleBLE::Adapter::get_adapters();

    // no adapter found
    if (adapter_list.empty()) {
        cerr << "No adapter was found." << endl;
        return {};
    }

    // only one found, returning directly
    if (adapter_list.size() == 1) {
        auto adapter = adapter_list.at(0);
        cout << "Using adapter: " << adapter.identifier() << " [" << adapter.address() << "]" << endl;
        return adapter;
    }

    return {};
}

#endif
