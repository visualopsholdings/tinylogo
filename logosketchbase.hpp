/*
  logosketch.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 14-Jun-2023
    
  A quick way to include everything that you need for a sketch with static code
  and strings.
  
  This work is licensed under the Creative Commons Attribution 4.0 International License. 
  To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/ or 
  send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  https://github.com/visualopsholdings/tinylogo
*/

#ifndef H_logosketchbase
#define H_logosketchbase

#include "arduinotimeprovider.hpp"
#include "ringbuffer.hpp"
#include "cmd.hpp"

class LogoSketchBase {

public:
  LogoSketchBase() : _failed(false) {}
  
  void setup(int baud=9600);
  void loop();

  virtual void precompile() = 0;
  virtual int dosetup(const char *cmd) = 0;
  virtual int docommand(const char *cmd) = 0;
  virtual int docompile(const char *cmd) = 0;
  virtual Logo *logo() = 0;
  
protected:
  
  ArduinoTimeProvider _time; // adds 12 bytes of dynamic memory
  Cmd _cmd; // 32 bytes
  char _cmdbuf[STRING_LEN]; // 32 bytes
  bool _failed;
  
  void showErr(int mode, int n);

};

#endif // H_logosketchbase

