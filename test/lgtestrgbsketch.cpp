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

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/auto_unit_test.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <thread>
#include <chrono>

#include "../logo.hpp"
#include "../arduinoflashstring.hpp"

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
  "SCLR\n"    // 1
  "C\n"       // 2
  "REDC\n"    // 3
  "N\n"       // 4
  "GREENC\n"  // 5
  "BLUEC\n"   // 6
  "AMBER\n"   // 7
  "RED\n"     // 8
  "GREEN\n"   // 9
  "BLUE\n"    // 10
  "REDR\n"    // 11
  "GREENR\n"  // 12
  "BLUER\n"   // 13
  "ON\n"      // 14
  "OFF\n"     // 15
  "SETALL\n"  // 16
  "R\n"       // 17
  "G\n"       // 18
  "B\n"       // 19
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
//   "TO $1 :$2; 100 - :$2 / 100  * 255; END;"
//   "TO $3 :$4; $11 $1 :$4; END;"
//   "TO $5 :$4; $12 $1 :$4; END;"
//   "TO $6 :$4; $13 $1 :$4; END;"
// //   // arguments are reversed!
//   "TO $16 :$19 :$18 :$17; $3 :$17 $5 :$18 $6 :$19; END;"
//   "TO $7; $16 100 75 0; END;"
//   "TO $8; $16 100 0 0; END;"
//   "TO $9; $16 0 100 0; END;"
//   "TO $10; $16 0 0 100; END;"
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
//   { "$14", &ledOn },
//   { "$15", &ledOff },
//   { "$11", &redRaw, 1 },
//   { "$12", &greenRaw, 1 },
//   { "$13", &blueRaw, 1 },
  };
#ifdef REAL_TIME
  RealTimeProvider time;
#else
  TestTimeProvider time;
#endif
  ArduinoFlashString strings(strings_flash);
  Logo logo(builtins, sizeof(builtins), &time, Logo::core, &strings);
  LogoCompiler compiler(&logo);

  ArduinoFlashString str(program_flash);
  compiler.compile(&str);
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);
  
  logo.resetcode();
  compiler.compile("AMBER");
//    compiler.compile("$7");
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
