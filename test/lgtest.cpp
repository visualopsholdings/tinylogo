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
#include "../arduinoflashcode.hpp"

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
  LogoFunctionPrimitives primitives(builtins, sizeof(builtins));
  Logo logo(&primitives);
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
  LogoFunctionPrimitives primitives(builtins, sizeof(builtins));
  Logo logo(&primitives);
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
  LogoFunctionPrimitives primitives(builtins, sizeof(builtins));
  Logo logo(&primitives);
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
  LogoFunctionPrimitives primitives(builtins, sizeof(builtins));
  Logo logo(&primitives);
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
  LogoFunctionPrimitives primitives(builtins, sizeof(builtins));
  Logo logo(&primitives);
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
  LogoFunctionPrimitives primitives(builtins, sizeof(builtins));
  Logo logo(&primitives);
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
  
  Logo logo;
  LogoCompiler compiler(&logo);

  compiler.compile("TO TEST1; END;");
  compiler.compile("TEST1");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  gCmds.clear();
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(gCmds.size(), 0);
  
}

void sarg(Logo &logo) {

  char s[LINE_LEN];
  LogoStringResult result;
  logo.popstring(&result);
  result.ncpy(s, sizeof(s));

  strstream str;
  str << "SARG " << s;
  gCmds.push_back(str.str());
#ifdef PRINT_RESULT
  cout << gCmds.back() << endl;
#endif  
}

#ifdef LOGO_SENTENCES

BOOST_AUTO_TEST_CASE( sentence )
{
  cout << "=== sentence ===" << endl;
  
  LogoBuiltinWord builtins[] = {
    { "ON", &ledOn },
    { "OFF", &ledOff },
    { "WAIT", &wait, 1 }
  };
  LogoFunctionPrimitives primitives(builtins, sizeof(builtins));
  Logo logo(&primitives);
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
  LogoFunctionPrimitives primitives(builtins, sizeof(builtins));
  Logo logo(&primitives);
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

BOOST_AUTO_TEST_CASE( sentenceInWord )
{
  cout << "=== sentenceInWord ===" << endl;
  
  LogoBuiltinWord builtins[] = {
    { "SARG", &sarg, 1 }
  };
  LogoFunctionPrimitives primitives(builtins, sizeof(builtins));
  Logo logo(&primitives);
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

#endif // LOGO_SENTENCES

BOOST_AUTO_TEST_CASE( arityLiteral1 )
{
  cout << "=== arityLiteral1 ===" << endl;
  
  LogoBuiltinWord builtins[] = {
    { "WAIT", &wait, 1 }
  };
  LogoFunctionPrimitives primitives(builtins, sizeof(builtins));
  Logo logo(&primitives);
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
  LogoFunctionPrimitives primitives(builtins, sizeof(builtins));
  Logo logo(&primitives);
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

  char s[LINE_LEN];
  LogoStringResult result;
  logo.popstring(&result);
  result.ncpy(s, sizeof(s));

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
  LogoFunctionPrimitives primitives(builtins, sizeof(builtins));
  Logo logo(&primitives);
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
  LogoFunctionPrimitives primitives(builtins, sizeof(builtins));
  Logo logo(&primitives);
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

void ar1(Logo &logo) {
  strstream str;
  str << "AR1 " << logo.popint();
  gCmds.push_back(str.str());
#ifdef PRINT_RESULT
  cout << gCmds.back() << endl;
#endif
}

void ar2(Logo &logo) {

  int val = logo.popint();
  logo.pushint(val * 2);
  
  strstream str;
  str << "AR2 " << val;
  gCmds.push_back(str.str());
#ifdef PRINT_RESULT
  cout << gCmds.back() << endl;
#endif
}

BOOST_AUTO_TEST_CASE( arityNested )
{
  cout << "=== arityNested ===" << endl;
  
  LogoBuiltinWord builtins[] = {
    { "AR1", &ar1, 1 },
    { "AR2", &ar2, 1 }
  };
  LogoFunctionPrimitives primitives(builtins, sizeof(builtins));
  Logo logo(&primitives);
  LogoCompiler compiler(&logo);

  compiler.compile("AR1 AR2 20");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  gCmds.clear();
  DEBUG_STEP_DUMP(10, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(gCmds.size(), 2);
  BOOST_CHECK_EQUAL(gCmds[0], "AR2 20");
  BOOST_CHECK_EQUAL(gCmds[1], "AR1 40");
  
}

BOOST_AUTO_TEST_CASE( seperateLines )
{
  cout << "=== seperateLines ===" << endl;
  
  LogoBuiltinWord builtins[] = {
    { "SARG", &sarg, 1 }
  };
  LogoFunctionPrimitives primitives(builtins, sizeof(builtins));
  Logo logo(&primitives);
  LogoCompiler compiler(&logo);

  compiler.compile("TO TEST\n");
  compiler.compile("  SARG \"XXX\n");
  compiler.compile("END\n");
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
  cout << "=== newLines ===" << endl;
  
  LogoBuiltinWord builtins[] = {
    { "SARG", &sarg, 1 }
  };
  LogoFunctionPrimitives primitives(builtins, sizeof(builtins));
  Logo logo(&primitives);
  LogoCompiler compiler(&logo);

  compiler.compile("TO TEST\n\tSARG \"XXX\nEND\nTEST");
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
  LogoFunctionPrimitives primitives(builtins, sizeof(builtins));
  Logo logo(&primitives);
  LogoCompiler compiler(&logo);

  compiler.compile("TO TEST\n\tSARG \"XXX\nEND\nTEST");
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
  int v1 = logo.popint();
  int v2 = logo.popint();
  str << "INFIX " << v1 << ", " << v2;
  logo.pushint(v1 + v2);
  gCmds.push_back(str.str());
#ifdef PRINT_RESULT
  cout << gCmds.back() << endl;
#endif

}

BOOST_AUTO_TEST_CASE( infix )
{
  cout << "=== infix ===" << endl;
  
  LogoBuiltinWord builtins[] = {
    { "INFIX", &infixfn, 255 },
  };
  LogoFunctionPrimitives primitives(builtins, sizeof(builtins));
  Logo logo(&primitives);
  LogoCompiler compiler(&logo);

  compiler.compile("1 INFIX 5");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  gCmds.clear();
  DEBUG_STEP_DUMP(5, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popint(), 6);
  BOOST_CHECK_EQUAL(gCmds.size(), 1);
  BOOST_CHECK_EQUAL(gCmds[0], "INFIX 5, 1");

}

void ar3(Logo &logo) {
  strstream str;
  str << "AR3 " << logo.popint();
  gCmds.push_back(str.str());
#ifdef PRINT_RESULT
  cout << gCmds.back() << endl;
#endif
}

BOOST_AUTO_TEST_CASE( infixArityNested )
{
  cout << "=== infixArityNested ===" << endl;
  
  LogoBuiltinWord builtins[] = {
    { "AR3", &ar3, 1 },
    { "+", &infixfn, 255 },
  };
  LogoFunctionPrimitives primitives(builtins, sizeof(builtins));
  Logo logo(&primitives);
  LogoCompiler compiler(&logo);

  compiler.compile("TO A;1;END");
//  compiler.compile("AR3 1 + 5 3 A");
  compiler.compile("AR3 1 + 5 A");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  gCmds.clear();
  DEBUG_STEP_DUMP(10, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(gCmds.size(), 2);
  BOOST_CHECK_EQUAL(gCmds[0], "INFIX 5, 1");
  BOOST_CHECK_EQUAL(gCmds[1], "AR3 6");
 
}

BOOST_AUTO_TEST_CASE( literalsOnStack )
{
  cout << "=== literalsOnStack ===" << endl;
  
  Logo logo;
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
  BOOST_CHECK_EQUAL(sizeof(tLogoInstruction), 6);
  BOOST_CHECK_EQUAL(sizeof(LogoWord), 6);
  BOOST_CHECK_EQUAL(sizeof(LogoVar), 10);
  
}

BOOST_AUTO_TEST_CASE( arguments )
{
  cout << "=== arguments ===" << endl;
  
  Logo logo(0, 0, Logo::core);
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

#include "../arduinoflashstring.hpp"

static const char program_flash[] PROGMEM = 
  "TO MULT :A :B; :A * :B; END;"
  "MULT 10 20";
  ;

BOOST_AUTO_TEST_CASE( flash )
{
  cout << "=== flash ===" << endl;
  
  Logo logo(0, 0, Logo::core);
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

static const char strings_fixedStrings[] PROGMEM = {
	"MULT\n"
	"A\n"
	"B\n"
};
  
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
  Logo logo(0, 0, Logo::core, &strings);
  LogoCompiler compiler(&logo);

  ArduinoFlashString program(program_fixedStrings);
  compiler.compile(&program);
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

//  logo.dumpstringscode(&compiler, "strings_fixedStrings");
  
  gCmds.clear();
  DEBUG_STEP_DUMP(20, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popint(), 200);
  DEBUG_DUMP(false);
  
}

BOOST_AUTO_TEST_CASE( getstring )
{
  cout << "=== getstring ===" << endl;
  
  Logo logo;

   const char *s1 = "XXX";
  LogoSimpleString ss1(s1);
  short len1 = ss1.length();
  short str1 = logo.addstring(&ss1, 0, len1);

  const char *s2 = "YYY";
  LogoSimpleString ss2(s2);
  short len2 = ss2.length();
  short str2 = logo.addstring(&ss2, 0, len2);

  LogoStringResult result;
  logo.getstring(&result, str1, len1);
  BOOST_CHECK_EQUAL(result.ncmp(s1), 0);
  
  logo.getstring(&result, str2, len2);
  BOOST_CHECK_EQUAL(result.ncmp(s2), 0);
  
}

BOOST_AUTO_TEST_CASE( stringcmp )
{
  cout << "=== stringcmp ===" << endl;
  
  ArduinoFlashString strings(strings_fixedStrings);
  Logo logo(0, 0, 0, &strings);

  LogoSimpleString s1("XXX");
  short len1 = s1.length();
  short str1 = logo.addstring(&s1, 0, len1);

  LogoSimpleString s2("YYY");
  short len2 = s2.length();
  short str2 = logo.addstring(&s2, 0, len2);

  LogoSimpleString t1("XXX");
  BOOST_CHECK(logo.stringcmp(&t1, 0, t1.length(), str1, len1));
  LogoSimpleString t2("YYY");
  BOOST_CHECK(logo.stringcmp(&t2, 0, t2.length(), str2, len2));
  BOOST_CHECK(!logo.stringcmp(&t1, 0, t1.length(), str1, len1+1));
  LogoSimpleString t3("AAA");
  BOOST_CHECK(!logo.stringcmp(&t3, 0, t3.length(), str1, len1));
  LogoSimpleString t4("MULT");
  BOOST_CHECK(logo.stringcmp(&t4, 0, t4.length(), 0, 4));
  LogoSimpleString t5("SSSS");
  BOOST_CHECK(!logo.stringcmp(&t5, 0, t5.length(), 0, 4));
  LogoSimpleString t6("A");
  BOOST_CHECK(logo.stringcmp(&t6, 0, t6.length(), 1, 1));
  LogoSimpleString t7("B");
  BOOST_CHECK(logo.stringcmp(&t7, 0, t7.length(), 2, 1));
  BOOST_CHECK(!logo.stringcmp(&t7, 0, t7.length(), 1, 1));
  LogoSimpleString t8("C");
  BOOST_CHECK(!logo.stringcmp(&t8, 0, t8.length(), 2, 1));
  BOOST_CHECK(!logo.stringcmp(&t8, 0, t8.length(), 4, 1));
  
}

static const char static_program[] PROGMEM = 
  "TO TEST1; ON WAIT 10 OFF WAIT 20; END;"
  "TO TEST2; OFF WAIT 30 ON WAIT 40; END;"
  "TEST1;"
  ;
static const short code_staticCode[][INST_LENGTH] PROGMEM = {
	{ OPTYPE_JUMP, 2, 0 },		// 0
	{ OPTYPE_HALT, 0, 0 },		// 1
	{ OPTYPE_BUILTIN, 0, 0 },		// 2
	{ OPTYPE_BUILTIN, 2, 0 },		// 3
	{ OPTYPE_INT, 10, 0 },		// 4
	{ OPTYPE_BUILTIN, 1, 0 },		// 5
	{ OPTYPE_BUILTIN, 2, 0 },		// 6
	{ OPTYPE_INT, 20, 0 },		// 7
	{ OPTYPE_RETURN, 0, 0 },		// 8
	{ OPTYPE_BUILTIN, 1, 0 },		// 9
	{ OPTYPE_BUILTIN, 2, 0 },		// 10
	{ OPTYPE_INT, 30, 0 },		// 11
	{ OPTYPE_BUILTIN, 0, 0 },		// 12
	{ OPTYPE_BUILTIN, 2, 0 },		// 13
	{ OPTYPE_INT, 40, 0 },		// 14
	{ OPTYPE_RETURN, 0, 0 },		// 15
	{ SCOPTYPE_WORD, 2, 0 }, 
	{ SCOPTYPE_WORD, 9, 0 }, 
	{ SCOPTYPE_END, 0, 0 } 
};
static const char strings_staticCode[] PROGMEM = {
	"TEST1\n"
	"TEST2\n"
};

BOOST_AUTO_TEST_CASE( staticProgUse )
{
  cout << "=== staticProgUse ===" << endl;
  
  LogoBuiltinWord builtins[] = {
    { "ON", &ledOn },
    { "OFF", &ledOff },
    { "WAIT", &wait, 1 }
  };
  LogoFunctionPrimitives primitives(builtins, sizeof(builtins));
  Logo logo(&primitives);
  LogoCompiler compiler(&logo);
  
  compiler.compile(static_program);
  
  gCmds.clear();
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(gCmds.size(), 4);
  BOOST_CHECK_EQUAL(gCmds[0], "LED ON");
  BOOST_CHECK_EQUAL(gCmds[1], "WAIT 10");
  BOOST_CHECK_EQUAL(gCmds[2], "LED OFF");
  BOOST_CHECK_EQUAL(gCmds[3], "WAIT 20");
  
}
BOOST_AUTO_TEST_CASE( staticCodeUse )
{
  cout << "=== staticCodeUse ===" << endl;
  
  LogoBuiltinWord builtins[] = {
    { "ON", &ledOn },
    { "OFF", &ledOff },
    { "WAIT", &wait, 1 }
  };
  ArduinoFlashCode code((const PROGMEM short *)code_staticCode);
  ArduinoFlashString strings(strings_staticCode);
  LogoFunctionPrimitives primitives(builtins, sizeof(builtins));
  Logo logo(&primitives, 0, 0, &strings, &code);

  gCmds.clear();
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(gCmds.size(), 4);
  BOOST_CHECK_EQUAL(gCmds[0], "LED ON");
  BOOST_CHECK_EQUAL(gCmds[1], "WAIT 10");
  BOOST_CHECK_EQUAL(gCmds[2], "LED OFF");
  BOOST_CHECK_EQUAL(gCmds[3], "WAIT 20");
  
}
