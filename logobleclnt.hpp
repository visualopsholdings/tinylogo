/*
  logobleclnt.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 14-Jul-2023
  
  Tiniest Logo Intepreter BLE stuff
      
  This work is licensed under the Creative Commons Attribution 4.0 International License. 
  To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/ or 
  send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  https://github.com/visualopsholdings/tinylogo
*/

#ifndef H_logobleclnt
#define H_logobleclnt

#ifndef ARDUINO
#include <optional>
#include <simpleble/SimpleBLE.h>
#endif

class LogoBLEClient {

public:

#ifndef ARDUINO
  bool send(const std::string &device, const std::string &data);
#endif
  
private:

#ifndef ARDUINO
  void sendToPeripheral(SimpleBLE::Peripheral *peripheral, const std::string &contents);
  std::optional<SimpleBLE::Adapter> getAdapter();
#endif

};

#endif // H_logobleclnt
