/*
  lgtestsketch.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 6-May-2023
  
  Tiny Logo sketch example tests.
  
  This work is licensed under the Creative Commons Attribution 4.0 International License. 
  To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/ or 
  send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  https://github.com/visualopsholdings/tinylogo
*/

#include "../logo.hpp"
#include "../logocompiler.hpp"
#include "../arduinoflashstring.hpp"
#include "../arduinoflashcode.hpp"

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/auto_unit_test.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <thread>
#include <chrono>

#include <iostream>
#include <vector>
#include <strstream>

using namespace std;
using namespace boost::posix_time;

vector<string> gCmds;

//#define PRINT_RESULT
//#define REAL_TIME

#include "testtimeprovider.hpp"

void rgb(Logo &logo, const char *color) {

  strstream str;
  str << color << " " << logo.popint();
  gCmds.push_back(str.str());
#ifdef PRINT_RESULT
  cout << gCmds.back() << endl;
#endif  

}

void ledOn(Logo &logo) {

  gCmds.push_back("LED ON");
#ifdef PRINT_RESULT
  cout << gCmds.back() << endl;
#endif  

}

void ledOff(Logo &logo) {

  gCmds.push_back("LED ON");
#ifdef PRINT_RESULT
  cout << gCmds.back() << endl;
#endif  

}

void redRaw(Logo &logo) {
  rgb(logo, "RED");
}

void greenRaw(Logo &logo) {
  rgb(logo, "GREEN");
}

void blueRaw(Logo &logo) {
  rgb(logo, "BLUE");
}

#ifdef REAL_TIME
class RealTimeProvider: public LogoTimeProvider {

public:

  unsigned long currentms() {
    std::time_t t = 1118158776;
    ptime first = from_time_t(t);
    ptime nowt = microsec_clock::local_time();
    time_duration start = nowt - first;
    return start.total_milliseconds();
  }
  void delayms(unsigned long ms) {
    this_thread::sleep_for(chrono::milliseconds(ms));
  }
  bool testing(short ms) { 
    cout << "WAIT " << ms << endl;
    return false; 
  };
  
};
#endif

// the strings for the program in flash
static const char strings_rgb[] PROGMEM = {
	"SCLR\n"
	"REDC\n"
	"GREENC\n"
	"BLUEC\n"
	"SETALL\n"
	"AMBER\n"
	"RED\n"
	"GREEN\n"
	"BLUE\n"
	"C\n"
	"N\n"
	"B\n"
	"G\n"
	"R\n"
};
static const short code_rgb[][INST_LENGTH] PROGMEM = {
	{ OPTYPE_NOOP, 0, 0 },		// 0
	{ OPTYPE_HALT, 0, 0 },		// 1
	{ OPTYPE_POPREF, 0, 0 },		// 2
	{ OPTYPE_INT, 100, 0 },		// 3
	{ OPTYPE_BUILTIN, 7, 1 },		// 4
	{ OPTYPE_REF, 9, 1 },		// 5
	{ OPTYPE_BUILTIN, 9, 1 },		// 6
	{ OPTYPE_INT, 100, 0 },		// 7
	{ OPTYPE_BUILTIN, 10, 1 },		// 8
	{ OPTYPE_INT, 255, 0 },		// 9
	{ OPTYPE_RETURN, 0, 0 },		// 10
	{ OPTYPE_POPREF, 1, 0 },		// 11
	{ OPTYPE_BUILTIN, 0, 0 },		// 12
	{ OPTYPE_JUMP, 2, 1 },		// 13
	{ OPTYPE_REF, 10, 1 },		// 14
	{ OPTYPE_RETURN, 0, 0 },		// 15
	{ OPTYPE_POPREF, 1, 0 },		// 16
	{ OPTYPE_BUILTIN, 1, 0 },		// 17
	{ OPTYPE_JUMP, 2, 1 },		// 18
	{ OPTYPE_REF, 10, 1 },		// 19
	{ OPTYPE_RETURN, 0, 0 },		// 20
	{ OPTYPE_POPREF, 1, 0 },		// 21
	{ OPTYPE_BUILTIN, 2, 0 },		// 22
	{ OPTYPE_JUMP, 2, 1 },		// 23
	{ OPTYPE_REF, 10, 1 },		// 24
	{ OPTYPE_RETURN, 0, 0 },		// 25
	{ OPTYPE_POPREF, 2, 0 },		// 26
	{ OPTYPE_POPREF, 3, 0 },		// 27
	{ OPTYPE_POPREF, 4, 0 },		// 28
	{ OPTYPE_JUMP, 11, 1 },		// 29
	{ OPTYPE_REF, 13, 1 },		// 30
	{ OPTYPE_JUMP, 16, 1 },		// 31
	{ OPTYPE_REF, 12, 1 },		// 32
	{ OPTYPE_JUMP, 21, 1 },		// 33
	{ OPTYPE_REF, 11, 1 },		// 34
	{ OPTYPE_RETURN, 0, 0 },		// 35
	{ OPTYPE_JUMP, 26, 3 },		// 36
	{ OPTYPE_INT, 100, 0 },		// 37
	{ OPTYPE_INT, 75, 0 },		// 38
	{ OPTYPE_INT, 0, 0 },		// 39
	{ OPTYPE_RETURN, 0, 0 },		// 40
	{ OPTYPE_JUMP, 26, 3 },		// 41
	{ OPTYPE_INT, 100, 0 },		// 42
	{ OPTYPE_INT, 0, 0 },		// 43
	{ OPTYPE_INT, 0, 0 },		// 44
	{ OPTYPE_RETURN, 0, 0 },		// 45
	{ OPTYPE_JUMP, 26, 3 },		// 46
	{ OPTYPE_INT, 0, 0 },		// 47
	{ OPTYPE_INT, 100, 0 },		// 48
	{ OPTYPE_INT, 0, 0 },		// 49
	{ OPTYPE_RETURN, 0, 0 },		// 50
	{ OPTYPE_JUMP, 26, 3 },		// 51
	{ OPTYPE_INT, 0, 0 },		// 52
	{ OPTYPE_INT, 0, 0 },		// 53
	{ OPTYPE_INT, 100, 0 },		// 54
	{ OPTYPE_RETURN, 0, 0 },		// 55
	{ SCOPTYPE_WORD, 2, 1 }, 
	{ SCOPTYPE_WORD, 11, 1 }, 
	{ SCOPTYPE_WORD, 16, 1 }, 
	{ SCOPTYPE_WORD, 21, 1 }, 
	{ SCOPTYPE_WORD, 26, 3 }, 
	{ SCOPTYPE_WORD, 36, 0 }, 
	{ SCOPTYPE_WORD, 41, 0 }, 
	{ SCOPTYPE_WORD, 46, 0 }, 
	{ SCOPTYPE_WORD, 51, 0 }, 
	{ SCOPTYPE_VAR, 0, 0 }, 
	{ SCOPTYPE_VAR, 0, 0 }, 
	{ SCOPTYPE_VAR, 0, 0 }, 
	{ SCOPTYPE_VAR, 0, 0 }, 
	{ SCOPTYPE_VAR, 0, 0 }, 
	{ SCOPTYPE_END, 0, 0 } 
};

BOOST_AUTO_TEST_CASE( rgbSketch )
{
  cout << "=== rgbSketch ===" << endl;
  
  LogoBuiltinWord builtins[] = {
  { "REDR", &redRaw, 1 },
  { "GREENR", &greenRaw, 1 },
  { "BLUER", &blueRaw, 1 },
  };
#ifdef REAL_TIME
  RealTimeProvider time;
#else
  TestTimeProvider time;
#endif
  ArduinoFlashString strings(strings_rgb);
  ArduinoFlashCode code((const PROGMEM short *)code_rgb);
  Logo logo(builtins, sizeof(builtins), &time, Logo::core, &strings, &code);
#ifdef LOGO_DEBUG
  LogoCompiler compiler(&logo);
#endif

  gCmds.clear();
  BOOST_CHECK_EQUAL(logo.callword("AMBER"), 0);
  
#ifdef REAL_TIME
   BOOST_CHECK_EQUAL(logo.run(), 0);
#else
//  DEBUG_STEP_DUMP(20, false);
  for (int i=0; i<100; i++) {
    int err = logo.step();
    BOOST_CHECK(err == 0 || err == LG_STOP);
  }
  BOOST_CHECK_EQUAL(gCmds.size(), 3);
  BOOST_CHECK_EQUAL(gCmds[0], "RED 0");
  BOOST_CHECK_EQUAL(gCmds[1], "GREEN 63");
  BOOST_CHECK_EQUAL(gCmds[2], "BLUE 255");
    
  logo.resetcode();
  gCmds.clear();
  BOOST_CHECK_EQUAL(logo.callword("RED"), 0);
//  DEBUG_STEP_DUMP(10, false);
  for (int i=0; i<100; i++) {
    int err = logo.step();
    BOOST_CHECK(err == 0 || err == LG_STOP);
  }
  BOOST_CHECK_EQUAL(gCmds.size(), 3);
  BOOST_CHECK_EQUAL(gCmds[0], "RED 0");
  BOOST_CHECK_EQUAL(gCmds[1], "GREEN 255");
  BOOST_CHECK_EQUAL(gCmds[2], "BLUE 255");

#endif
   
}
