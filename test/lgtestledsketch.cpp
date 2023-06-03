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

void ledOn(Logo &logo) {
  gCmds.push_back("LED ON");
#ifdef PRINT_RESULT
   cout << gCmds.back() << endl;
#endif
}

void ledOff(Logo &logo) {
  gCmds.push_back("LED OFF");
#ifdef PRINT_RESULT
   cout << gCmds.back() << endl;
#endif
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

static const char strings_led[] PROGMEM = {
	"FLASH\n"
	"GO\n"
	"STOP\n"
};
static const short code_led[][INST_LENGTH] PROGMEM = {
	{ OPTYPE_NOOP, 0, 0 },		// 0
	{ OPTYPE_HALT, 0, 0 },		// 1
	{ OPTYPE_BUILTIN, 0, 0 },		// 2
	{ OPTYPE_BUILTIN, 5, 1 },		// 3
	{ OPTYPE_INT, 100, 0 },		// 4
	{ OPTYPE_BUILTIN, 1, 0 },		// 5
	{ OPTYPE_BUILTIN, 5, 1 },		// 6
	{ OPTYPE_INT, 1000, 0 },		// 7
	{ OPTYPE_RETURN, 0, 0 },		// 8
	{ OPTYPE_BUILTIN, 2, 1 },		// 9
	{ OPTYPE_JUMP, 2, 0 },		// 10
	{ OPTYPE_RETURN, 0, 0 },		// 11
	{ OPTYPE_BUILTIN, 1, 0 },		// 12
	{ OPTYPE_RETURN, 0, 0 },		// 13
	{ SCOPTYPE_WORD, 2, 0 }, 
	{ SCOPTYPE_WORD, 9, 0 }, 
	{ SCOPTYPE_WORD, 12, 0 }, 
	{ SCOPTYPE_END, 0, 0 } 
};
static const char program_led[] PROGMEM = 
  "TO FLASH; ON WAIT 100 OFF WAIT 1000; END;"
  "TO GO; FOREVER FLASH; END;"
  "TO STOP; OFF; END;"
  ;

BOOST_AUTO_TEST_CASE( ledSketch )
{
  cout << "=== ledSketch ===" << endl;
  
  LogoBuiltinWord builtins[] = {
    { "ON", &ledOn },
    { "OFF", &ledOff },
  };
#ifdef REAL_TIME
  RealTimeProvider time;
#else
  TestTimeProvider time;
#endif
  ArduinoFlashString strings(strings_led);
  ArduinoFlashCode code((const PROGMEM short *)code_led);
  Logo logo(builtins, sizeof(builtins), &time, Logo::core, &strings, &code);
#ifdef LOGO_DEBUG
  LogoCompiler compiler(&logo);
#endif

  gCmds.clear();
  BOOST_CHECK_EQUAL(logo.callword("GO"), 0);
  
#ifdef REAL_TIME
   BOOST_CHECK_EQUAL(logo.run(), 0);
#else
//  DEBUG_STEP_DUMP(20, false);
  for (int i=0; i<20; i++) {
    BOOST_CHECK_EQUAL(logo.step(), 0);
  }
  BOOST_CHECK_EQUAL(gCmds.size(), 9);
  BOOST_CHECK_EQUAL(gCmds[0], "LED ON");
  BOOST_CHECK_EQUAL(gCmds[1], "WAIT 100");
  BOOST_CHECK_EQUAL(gCmds[2], "LED OFF");
  BOOST_CHECK_EQUAL(gCmds[3], "WAIT 1000");
    
  logo.resetcode();
  gCmds.clear();
  BOOST_CHECK_EQUAL(logo.callword("STOP"), 0);
  DEBUG_DUMP(false);
//  DEBUG_STEP_DUMP(10, false);
  for (int i=0; i<10; i++) {
    int err = logo.step();
    BOOST_CHECK(err == 0 || err == LG_STOP);
  }
  BOOST_CHECK_EQUAL(gCmds.size(), 1);
  BOOST_CHECK_EQUAL(gCmds[0], "LED OFF");

  logo.resetcode();
  gCmds.clear();
  BOOST_CHECK_EQUAL(logo.callword("ON"), 0);
  DEBUG_DUMP(false);
//  DEBUG_STEP_DUMP(10, false);
  for (int i=0; i<10; i++) {
    int err = logo.step();
    BOOST_CHECK(err == 0 || err == LG_STOP);
  }
  BOOST_CHECK_EQUAL(gCmds.size(), 1);
  BOOST_CHECK_EQUAL(gCmds[0], "LED ON");
#endif
  
}
