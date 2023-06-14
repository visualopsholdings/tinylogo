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

#ifndef H_logosketch
#define H_logosketch

#include "logo.hpp"
#include "arduinotimeprovider.hpp"
#include "arduinoflashstring.hpp"
#include "arduinoflashcode.hpp"
#include "ringbuffer.hpp"
#include "cmd.hpp"

class LogoSketch {

public:
  LogoSketch(char strings[] PROGMEM, const PROGMEM short *code, LogoBuiltinWord builtins[]=0, int size=0);
  
  void setup();
  void loop();
  
private:
  ArduinoTimeProvider _time;
  ArduinoFlashString _strings;
  ArduinoFlashCode _code;
  LogoFunctionPrimitives _primitives;
  Logo _logo;
  RingBuffer _buffer;
  Cmd _cmd;
  char _cmdbuf[64];

  void showErr(int mode, int n);

};

#endif // H_logosketch

