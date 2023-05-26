/*
  lgtest.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 6-May-2023
  
  Tiny Logo general tests.
  
  This work is licensed under the Creative Commons Attribution 4.0 International License. 
  To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/ or 
  send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  https://github.com/visualopsholdings/tinylogo
*/

#include "../logo.hpp"
#include "../logocompiler.hpp"

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/auto_unit_test.hpp>

#include <iostream>
#include <vector>
#include <strstream>

using namespace std;

vector<string> gCmds;

//#define PRINT_RESULT

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

void wait(Logo &logo) {
  strstream str;
  str << "WAIT " << logo.popint();
  gCmds.push_back(str.str());
#ifdef PRINT_RESULT
  cout << gCmds.back() << endl;
#endif
}

BOOST_AUTO_TEST_CASE( builtin )
{
  cout << "=== builtin ===" << endl;
  
  LogoBuiltinWord builtins[] = {
    { "ON", &ledOn }
  };
  Logo logo(builtins, sizeof(builtins), 0);
  LogoCompiler compiler(&logo);

  compiler.compile("ON");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);
  
  gCmds.clear();
  BOOST_CHECK_EQUAL(logo.step(), 0);
  BOOST_CHECK_EQUAL(gCmds[0], "LED ON");
  DEBUG_DUMP(false);
  
}

BOOST_AUTO_TEST_CASE( compound )
{
  cout << "=== compound ===" << endl;
  
  LogoBuiltinWord builtins[] = {
    { "ON", &ledOn },
    { "OFF", &ledOff },
    { "WAIT", &wait, 1 }
  };
  Logo logo(builtins, sizeof(builtins), 0);
  LogoCompiler compiler(&logo);

  compiler.compile("ON WAIT 100 OFF WAIT 20");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  gCmds.clear();
  DEBUG_STEP_DUMP(3, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(gCmds[0], "LED ON");
  BOOST_CHECK_EQUAL(gCmds[1], "WAIT 100");
  BOOST_CHECK_EQUAL(gCmds[2], "LED OFF");
  BOOST_CHECK_EQUAL(gCmds[3], "WAIT 20");
  
}

BOOST_AUTO_TEST_CASE( defineSimpleWord )
{
  cout << "=== defineSimpleWord ===" << endl;
  
  LogoBuiltinWord builtins[] = {
    { "ON", &ledOn },
    { "OFF", &ledOff }
  };
  Logo logo(builtins, sizeof(builtins), 0);
  LogoCompiler compiler(&logo);

  compiler.compile("TO TURNON; ON; END;");
  compiler.compile("TURNON;");
  compiler.compile("OFF;");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  gCmds.clear();
  BOOST_CHECK_EQUAL(logo.step(), 0);
  DEBUG_DUMP(false);
  BOOST_CHECK_EQUAL(logo.step(), 0);
  DEBUG_DUMP(false);
  BOOST_CHECK_EQUAL(logo.step(), 0);
  DEBUG_DUMP(false);
  BOOST_CHECK_EQUAL(logo.step(), 0);
  DEBUG_DUMP(false);
  BOOST_CHECK_EQUAL(gCmds.size(), 2);
  BOOST_CHECK_EQUAL(gCmds[0], "LED ON");
  BOOST_CHECK_EQUAL(gCmds[1], "LED OFF");
  
}

BOOST_AUTO_TEST_CASE( defineCompoundWord )
{
  cout << "=== defineCompoundWord ===" << endl;
  
  LogoBuiltinWord builtins[] = {
    { "ON", &ledOn },
    { "OFF", &ledOff },
    { "WAIT", &wait, 1 }
  };
  Logo logo(builtins, sizeof(builtins), 0);
  LogoCompiler compiler(&logo);

  compiler.compile("TO TEST1; ON WAIT 100 OFF WAIT 20; END;");
  compiler.compile("TO TEST2; OFF WAIT 30 ON WAIT 40; END;");
  compiler.compile("TEST1; TEST2;");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  gCmds.clear();
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(gCmds.size(), 8);
  BOOST_CHECK_EQUAL(gCmds[0], "LED ON");
  BOOST_CHECK_EQUAL(gCmds[1], "WAIT 100");
  BOOST_CHECK_EQUAL(gCmds[2], "LED OFF");
  BOOST_CHECK_EQUAL(gCmds[3], "WAIT 20");
  BOOST_CHECK_EQUAL(gCmds[4], "LED OFF");
  BOOST_CHECK_EQUAL(gCmds[5], "WAIT 30");
  BOOST_CHECK_EQUAL(gCmds[6], "LED ON");
  BOOST_CHECK_EQUAL(gCmds[7], "WAIT 40");
  
}

BOOST_AUTO_TEST_CASE( nestedWord )
{
  cout << "=== nestedWord ===" << endl;
  
  LogoBuiltinWord builtins[] = {
    { "ON", &ledOn },
    { "OFF", &ledOff },
    { "WAIT", &wait, 1 }
  };
  Logo logo(builtins, sizeof(builtins), 0);
  LogoCompiler compiler(&logo);

  compiler.compile("TO TEST1; ON WAIT 100 OFF WAIT 20; END;");
  compiler.compile("TO TEST2; TEST1; END;");
  compiler.compile("TEST2;");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  gCmds.clear();
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(gCmds.size(), 4);
  BOOST_CHECK_EQUAL(gCmds[0], "LED ON");
  BOOST_CHECK_EQUAL(gCmds[1], "WAIT 100");
  BOOST_CHECK_EQUAL(gCmds[2], "LED OFF");
  BOOST_CHECK_EQUAL(gCmds[3], "WAIT 20");
  
}

BOOST_AUTO_TEST_CASE( defineCompoundWordRun1 )
{
  cout << "=== defineCompoundWord ===" << endl;
  
  LogoBuiltinWord builtins[] = {
    { "ON", &ledOn },
    { "OFF", &ledOff },
    { "WAIT", &wait, 1 }
  };
  Logo logo(builtins, sizeof(builtins), 0);
  LogoCompiler compiler(&logo);

  compiler.compile("TO TEST1; ON WAIT 10 OFF WAIT 20; END;");
  compiler.compile("TO TEST2; OFF WAIT 30 ON WAIT 40; END;");
  compiler.compile("TEST1;");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  gCmds.clear();
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(gCmds.size(), 4);
  BOOST_CHECK_EQUAL(gCmds[0], "LED ON");
  BOOST_CHECK_EQUAL(gCmds[1], "WAIT 10");
  BOOST_CHECK_EQUAL(gCmds[2], "LED OFF");
  BOOST_CHECK_EQUAL(gCmds[3], "WAIT 20");
  
}

BOOST_AUTO_TEST_CASE( defineEmptyWord )
{
  cout << "=== defineEmptyWord ===" << endl;
  
  LogoBuiltinWord empty[] = {};
  Logo logo(empty, 0, 0);
  LogoCompiler compiler(&logo);

  compiler.compile("TO TEST1; END;");
  compiler.compile("TEST1");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  gCmds.clear();
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(gCmds.size(), 0);
  
}

BOOST_AUTO_TEST_CASE( sentence )
{
  cout << "=== sentence ===" << endl;
  
  LogoBuiltinWord builtins[] = {
    { "ON", &ledOn },
    { "OFF", &ledOff },
    { "WAIT", &wait, 1 }
  };
  Logo logo(builtins, sizeof(builtins), 0);
  LogoCompiler compiler(&logo);

  compiler.compile("[ON WAIT 10 OFF WAIT 20];");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  gCmds.clear();
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(gCmds.size(), 4);
  BOOST_CHECK_EQUAL(gCmds[0], "LED ON");
  BOOST_CHECK_EQUAL(gCmds[1], "WAIT 10");
  BOOST_CHECK_EQUAL(gCmds[2], "LED OFF");
  BOOST_CHECK_EQUAL(gCmds[3], "WAIT 20");
  
}

BOOST_AUTO_TEST_CASE( sentences )
{
  cout << "=== sentences ===" << endl;
  
  LogoBuiltinWord builtins[] = {
    { "ON", &ledOn },
    { "OFF", &ledOff },
    { "WAIT", &wait, 1 }
  };
  Logo logo(builtins, sizeof(builtins), 0);
  LogoCompiler compiler(&logo);

  compiler.compile("[ON] [WAIT 10] [OFF] [WAIT 20];");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  gCmds.clear();
//   DEBUG_STEP_DUMP(10, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(gCmds.size(), 4);
  BOOST_CHECK_EQUAL(gCmds[0], "LED ON");
  BOOST_CHECK_EQUAL(gCmds[1], "WAIT 10");
  BOOST_CHECK_EQUAL(gCmds[2], "LED OFF");
  BOOST_CHECK_EQUAL(gCmds[3], "WAIT 20");
  
}

void sarg(Logo &logo) {

  char s[WORD_LEN];
  logo.popstring(s, sizeof(s));
//  logo.execword(s);

  strstream str;
  str << "SARG " << s;
  gCmds.push_back(str.str());
#ifdef PRINT_RESULT
  cout << gCmds.back() << endl;
#endif  
}

BOOST_AUTO_TEST_CASE( sentenceInWord )
{
  cout << "=== sentenceInWord ===" << endl;
  
  LogoBuiltinWord builtins[] = {
    { "SARG", &sarg, 1 }
  };
  Logo logo(builtins, sizeof(builtins), 0);
  LogoCompiler compiler(&logo);

  compiler.compile("TO TEST; SARG [XXX]; END;");
  compiler.compile("TEST;");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  gCmds.clear();
  DEBUG_STEP_DUMP(7, 0);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(gCmds.size(), 1);
  BOOST_CHECK_EQUAL(gCmds[0], "SARG XXX");
  
}

BOOST_AUTO_TEST_CASE( arityLiteral1 )
{
  cout << "=== arityLiteral1 ===" << endl;
  
  LogoBuiltinWord builtins[] = {
    { "WAIT", &wait, 1 }
  };
  Logo logo(builtins, sizeof(builtins), 0);
  LogoCompiler compiler(&logo);

  compiler.compile("WAIT 20");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  gCmds.clear();
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(gCmds.size(), 1);
  BOOST_CHECK_EQUAL(gCmds[0], "WAIT 20");
  
}

BOOST_AUTO_TEST_CASE( arityWord1 )
{
  cout << "=== arityWord1 ===" << endl;
  
  LogoBuiltinWord builtins[] = {
    { "WAIT", &wait, 1 }
  };
  Logo logo(builtins, sizeof(builtins), 0);
  LogoCompiler compiler(&logo);

  compiler.compile("TO TIME; 20; END");
  compiler.compile("WAIT TIME");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  gCmds.clear();
  DEBUG_STEP_DUMP(5, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(gCmds.size(), 1);
  BOOST_CHECK_EQUAL(gCmds[0], "WAIT 20");
  
}

void args2(Logo &logo) {
  char s[32];
  logo.popstring(s, sizeof(s));
  int n = logo.popint();
  strstream str;
  str << "ARGS2 " << n << ", " << s;
  gCmds.push_back(str.str());
#ifdef PRINT_RESULT
  cout << gCmds.back() << endl;
#endif
}

BOOST_AUTO_TEST_CASE( arityLiteral )
{
  cout << "=== arityLiteral ===" << endl;
  
  LogoBuiltinWord builtins[] = {
    { "ARGS2", &args2, 2 }
  };
  Logo logo(builtins, sizeof(builtins), 0);
  LogoCompiler compiler(&logo);

  compiler.compile("ARGS2 20 XXXX");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  gCmds.clear();
  DEBUG_STEP_DUMP(10, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(gCmds.size(), 1);
  BOOST_CHECK_EQUAL(gCmds[0], "ARGS2 20, XXXX");
  
}

BOOST_AUTO_TEST_CASE( arityWord )
{
  cout << "=== arityWord ===" << endl;
  
  LogoBuiltinWord builtins[] = {
    { "ARGS2", &args2, 2 }
  };
  Logo logo(builtins, sizeof(builtins), 0);
  LogoCompiler compiler(&logo);

  compiler.compile("TO TIME; 20; END");
  compiler.compile("TO THING; XXXX; END");
  compiler.compile("ARGS2 TIME THING");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  gCmds.clear();
  DEBUG_STEP_DUMP(10, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(gCmds.size(), 1);
  BOOST_CHECK_EQUAL(gCmds[0], "ARGS2 20, XXXX");
  
}

BOOST_AUTO_TEST_CASE( seperateLines )
{
  cout << "=== seperateLines ===" << endl;
  
  LogoBuiltinWord builtins[] = {
    { "SARG", &sarg, 1 }
  };
  Logo logo(builtins, sizeof(builtins), 0);
  LogoCompiler compiler(&logo);

  compiler.compile("TO TEST");
  compiler.compile("  SARG [XXX]");
  compiler.compile("END");
  compiler.compile("TEST");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  gCmds.clear();
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(gCmds.size(), 1);
  BOOST_CHECK_EQUAL(gCmds[0], "SARG XXX");
  
}

BOOST_AUTO_TEST_CASE( newLines )
{
  cout << "=== seperateLines ===" << endl;
  
  LogoBuiltinWord builtins[] = {
    { "SARG", &sarg, 1 }
  };
  Logo logo(builtins, sizeof(builtins), 0);
  LogoCompiler compiler(&logo);

  compiler.compile("TO TEST\n\tSARG [XXX]\nEND\nTEST");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  gCmds.clear();
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(gCmds.size(), 1);
  BOOST_CHECK_EQUAL(gCmds[0], "SARG XXX");
  
}

BOOST_AUTO_TEST_CASE( reset )
{
  cout << "=== reset ===" << endl;
  
  LogoBuiltinWord builtins[] = {
    { "SARG", &sarg, 1 }
  };
  Logo logo(builtins, sizeof(builtins), 0);
  LogoCompiler compiler(&logo);

  compiler.compile("TO TEST\n\tSARG [XXX]\nEND\nTEST");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  gCmds.clear();
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(gCmds.size(), 1);
  BOOST_CHECK_EQUAL(gCmds[0], "SARG XXX");
  
  gCmds.clear();
  logo.restart();
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(gCmds.size(), 1);
  BOOST_CHECK_EQUAL(gCmds[0], "SARG XXX");
  
}

void infixfn(Logo &logo) {

  strstream str;
  str << "INFIX " << logo.popint() << ", " << logo.popint();
  gCmds.push_back(str.str());
#ifdef PRINT_RESULT
  cout << gCmds.back() << endl;
#endif

}

BOOST_AUTO_TEST_CASE( infix )
{
  cout << "=== infix ===" << endl;
  
  LogoBuiltinWord builtins[] = {
    { "INFIX", &infixfn, 1 },
  };
  Logo logo(builtins, sizeof(builtins), 0);
  LogoCompiler compiler(&logo);

  compiler.compile("1 INFIX 5");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  gCmds.clear();
  DEBUG_STEP_DUMP(5, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(gCmds.size(), 1);
  BOOST_CHECK_EQUAL(gCmds[0], "INFIX 5, 1");

}

BOOST_AUTO_TEST_CASE( literalsOnStack )
{
  cout << "=== literalsOnStack ===" << endl;
  
  LogoBuiltinWord empty[] = {};
  Logo logo(empty, 0, 0);
  LogoCompiler compiler(&logo);

  compiler.compile("TO NUM; 1 2 3; END; NUM");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  gCmds.clear();
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popint(), 3);
  BOOST_CHECK_EQUAL(logo.popint(), 2);
  BOOST_CHECK_EQUAL(logo.popint(), 1);
  DEBUG_DUMP(false);
  
}

BOOST_AUTO_TEST_CASE( sizes )
{
  cout << "=== sizes ===" << endl;
  
  // half this size on an arduino
  BOOST_CHECK_EQUAL(sizeof(LogoBuiltinWord), 24);
  BOOST_CHECK_EQUAL(sizeof(LogoInstruction), 6);
#ifdef HAS_VARIABLES
  BOOST_CHECK_EQUAL(sizeof(LogoWord), 4);
  BOOST_CHECK_EQUAL(sizeof(LogoVar), 6);
#else
  BOOST_CHECK_EQUAL(sizeof(LogoWord), 3);
#endif
  
}

#ifdef HAS_VARIABLES

BOOST_AUTO_TEST_CASE( arguments )
{
  cout << "=== arguments ===" << endl;
  
  LogoBuiltinWord empty[] = {};
  Logo logo(empty, 0, 0, Logo::core);
  LogoCompiler compiler(&logo);

//   compiler.compile("TO MULT; :A * :B; END;");
//   compiler.compile("MAKE \"A 10 MAKE \"B 20 MULT");
 compiler.compile("TO MULT :A :B; :A * :B; END;");
 compiler.compile("MULT 10 20");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  gCmds.clear();
  DEBUG_STEP_DUMP(20, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popint(), 200);
  DEBUG_DUMP(false);
  
}

#endif // HAS_VARIABLES

#include "../arduinoflashstring.hpp"

static const char program_flash[] PROGMEM = 
  "TO MULT :A :B; :A * :B; END;"
  "MULT 10 20";
  ;

BOOST_AUTO_TEST_CASE( flash )
{
  cout << "=== flash ===" << endl;
  
  LogoBuiltinWord empty[] = {};
  Logo logo(empty, 0, 0, Logo::core);
  LogoCompiler compiler(&logo);

  ArduinoFlashString program(program_flash);
  compiler.compile(&program);
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  gCmds.clear();
  DEBUG_STEP_DUMP(20, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popint(), 200);
  DEBUG_DUMP(false);
  
}

static const char strings_fixedStrings[] PROGMEM = 
	"MULT\n"
	"A\n"
	"B\n"
  ;
  
static const char program_fixedStrings[] PROGMEM = 
  "TO MULT :A :B; :A * :B; END;"
  "MULT 10 20";
//   "TO $1 :$2 :$3; :$2 * :$3; END;"
//   "$1 10 20"
  ;

BOOST_AUTO_TEST_CASE( fixedStrings )
{
  cout << "=== fixedStrings ===" << endl;
  
  ArduinoFlashString strings(strings_fixedStrings);
  LogoBuiltinWord empty[] = {};
  Logo logo(empty, 0, 0, Logo::core, &strings);
  LogoCompiler compiler(&logo);

  ArduinoFlashString program(program_fixedStrings);
  compiler.compile(&program);
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

//  logo.dumpstringscode(&compiler);
  
  gCmds.clear();
  DEBUG_STEP_DUMP(20, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popint(), 200);
  DEBUG_DUMP(false);
  
}

BOOST_AUTO_TEST_CASE( getstring )
{
  cout << "=== getstring ===" << endl;
  
  LogoBuiltinWord empty[] = {};
  Logo logo(empty, 0, 0, 0, 0);

  const char *s1 = "XXX";
  short len1 = strlen(s1);
  short str1 = logo.addstring(s1, len1);

  const char *s2 = "YYY";
  short len2 = strlen(s2);
  short str2 = logo.addstring(s2, len2);

  char name[32];
  logo.getstring(name, sizeof(name), str1, len1);
  BOOST_CHECK_EQUAL(name, s1);
  
  logo.getstring(name, sizeof(name), str2, len2);
  BOOST_CHECK_EQUAL(name, s2);
  
}

BOOST_AUTO_TEST_CASE( stringcmp )
{
  cout << "=== stringcmp ===" << endl;
  
  ArduinoFlashString strings(strings_fixedStrings);
  LogoBuiltinWord empty[] = {};
  Logo logo(empty, 0, 0, 0, &strings);

  const char *s1 = "XXX";
  short len1 = strlen(s1);
  short str1 = logo.addstring(s1, len1);

  const char *s2 = "YYY";
  short len2 = strlen(s2);
  short str2 = logo.addstring(s2, len2);

  BOOST_CHECK(logo.stringcmp("XXX", 3, str1, len1));
  BOOST_CHECK(logo.stringcmp("YYY", 3, str2, len2));
  BOOST_CHECK(!logo.stringcmp("XXX", 3, str1, len1+1));
  BOOST_CHECK(!logo.stringcmp("AAA", 3, str1, len1));
  BOOST_CHECK(logo.stringcmp("MULT", 4, 0, 4));
  BOOST_CHECK(!logo.stringcmp("SSSS", 4, 0, 4));
  BOOST_CHECK(logo.stringcmp("A", 1, 1, 1));
  BOOST_CHECK(logo.stringcmp("B", 1, 2, 1));
  BOOST_CHECK(!logo.stringcmp("B", 1, 1, 1));
  BOOST_CHECK(!logo.stringcmp("C", 1, 2, 1));
  BOOST_CHECK(!logo.stringcmp("C", 1, 4, 1));
  
}
