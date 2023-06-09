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

#include "logosketchbase.hpp"
#include "logo.hpp"
#include "logocompiler.hpp"
#include "arduinotimeprovider.hpp"
#include "arduinoflashstring.hpp"
#include "arduinoflashcode.hpp"
#include "ringbuffer.hpp"
#include "cmd.hpp"

class LogoSketch : public LogoSketchBase {

public:
  LogoSketch(char strings[] PROGMEM, const PROGMEM short *code);
  
  // LogoSketchBase
  virtual void precompile();
  virtual int dosetup(const char *cmd);
  virtual int docommand(const char *cmd);
  virtual int docompile(const char *cmd) {}
  virtual Logo *logo() { return &_logo; }

private:
  ArduinoFlashString _strings; // adds 18 bytes
  ArduinoFlashCode _code; // 2 bytes
  Logo _logo; // 700 bytes (stack, string pool and variables)
};

class LogoInlineSketch : public LogoSketchBase {

public:
  LogoInlineSketch(char program[] PROGMEM);
  
  // LogoSketchBase
  virtual void precompile();
  virtual int dosetup(const char *cmd);
  virtual int docommand(const char *cmd);
  virtual int docompile(const char *cmd);
  virtual Logo *logo() { return &_logo; }

private:
  ArduinoFlashString _program; // adds 18 bytes
  LogoCompiler _compiler;
  Logo _logo; // 700 bytes (stack, string pool and variables)
};

#endif // H_logosketch

