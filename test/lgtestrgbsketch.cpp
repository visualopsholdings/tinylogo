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
static const char strings_flash[] PROGMEM = 
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
  ;

// the initial program goes into FLASH
static const char program_flash[] PROGMEM = 
  "TO SCLR :C; 100 - :C / 100  * 255; END;"
  "TO REDC :N; REDR SCLR :N; END;"
  "TO GREENC :N; GREENR SCLR :N; END;"
  "TO BLUEC :N; BLUER SCLR :N; END;"
  // arguments are reversed!
  "TO SETALL :B :G :R; REDC :R GREENC :G BLUEC :B; END;"
  "TO AMBER; SETALL 100 75 0; END;"
  "TO RED; SETALL 100 0 0; END;"
  "TO GREEN; SETALL 0 100 0; END;"
  "TO BLUE; SETALL 0 0 100; END;"
  ;

BOOST_AUTO_TEST_CASE( rgbSketch )
{
  cout << "=== rgbSketch ===" << endl;
  
  LogoBuiltinWord builtins[] = {
  { "ON", &ledOn },
  { "OFF", &ledOff },
  { "REDR", &redRaw, 1 },
  { "GREENR", &greenRaw, 1 },
  { "BLUER", &blueRaw, 1 },
  };
#ifdef REAL_TIME
  RealTimeProvider time;
#else
  TestTimeProvider time;
#endif
 ArduinoFlashString strings(strings_flash);
 Logo logo(builtins, sizeof(builtins), &time, Logo::core, &strings);
//  Logo logo(builtins, sizeof(builtins), &time, Logo::core);
  LogoCompiler compiler(&logo);

  ArduinoFlashString str(program_flash);
  compiler.compile(&str);
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);
  
//  logo.dumpstringscode(&compiler);

  logo.resetcode();
  compiler.compile("AMBER");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  gCmds.clear();
  DEBUG_STEP_DUMP(20, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  
  BOOST_CHECK_EQUAL(gCmds.size(), 3);
  BOOST_CHECK_EQUAL(gCmds[0], "RED 0");
  BOOST_CHECK_EQUAL(gCmds[1], "GREEN 63");
  BOOST_CHECK_EQUAL(gCmds[2], "BLUE 255");

}
