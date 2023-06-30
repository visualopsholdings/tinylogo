/*
  lgtestexamples.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 17-Jun-2023
  
  Tiny Logo examples from the UCBLogo manual
  
  This work is licensed under the Creative Commons Attribution 4.0 International License. 
  To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/ or 
  send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  https://github.com/visualopsholdings/tinylogo
*/

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/auto_unit_test.hpp>

#include "../logo.hpp"
#include "../logocompiler.hpp"
#include "../arduinoflashcode.hpp"

using namespace std;

BOOST_AUTO_TEST_CASE( page3_plural )
{
  cout << "=== page3_plural ===" << endl;
  
  Logo logo;
  LogoCompiler compiler(&logo);

  compiler.compile("to PLURAL :WORD");
  compiler.compile("  word :WORD \"S");
  compiler.compile("end");
  compiler.compile("PLURAL \"COMPUTER");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  DEBUG_STEP_DUMP(10, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  LogoStringResult str;
  logo.popstring(&str);  
  BOOST_CHECK_EQUAL(str.ncmp("COMPUTERS"), 0);
  BOOST_CHECK(logo.stackempty());
  
}

BOOST_AUTO_TEST_CASE( page3_increment )
{
  cout << "=== page3_increment ===" << endl;
  
  Logo logo;
  LogoCompiler compiler(&logo);

  compiler.compile("to INCREMENT :MY.VAR");
  compiler.compile("  make :MY.VAR (thing :MY.VAR)+1");
  compiler.compile("end");
  compiler.compile("make \"X 5");
  compiler.compile("INCREMENT \"X");
  compiler.compile("output :X");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popint(), 6);
  BOOST_CHECK(logo.stackempty());
  DEBUG_DUMP(false);
  
}

static const char strings_setup[] PROGMEM = {
// words
	"SETUP\n"
// variables
// strings
	"VAR1\n"
	"VAR2\n"
	"SETUP\n"
};
static const short code_setup[][INST_LENGTH] PROGMEM = {
	{ OPTYPE_BUILTIN, 1, 0 },		// 0
	{ OPTYPE_STRING, 1, 4 },		// 1
	{ OPTYPE_INT, 1, 0 },		// 2
	{ OPTYPE_BUILTIN, 1, 0 },		// 3
	{ OPTYPE_STRING, 2, 4 },		// 4
	{ OPTYPE_INT, 1, 0 },		// 5
	{ OPTYPE_BUILTIN, 18, 0 },		// 6
	{ OPTYPE_REF, 1, 4 },		// 7
	{ OPTYPE_BUILTIN, 8, 0 },		// 8
	{ OPTYPE_REF, 2, 4 },		// 9
	{ OPTYPE_HALT, 0, 0 },		// 10
	{ OPTYPE_BUILTIN, 18, 0 },		// 11
	{ OPTYPE_STRING, 0, 5 },		// 12
	{ OPTYPE_RETURN, 0, 0 },		// 13
	{ SCOPTYPE_WORD, 11, 0 }, 
	{ SCOPTYPE_END, 0, 0 } 
};

BOOST_AUTO_TEST_CASE( sketch_setup )
{
  cout << "=== sketch_setup ===" << endl;
  
  ArduinoFlashCode code((const PROGMEM short *)code_setup);
  ArduinoFlashString strings(strings_setup);
  Logo logo(0, &strings, &code);
  LogoCompiler compiler(&logo);

  DEBUG_DUMP(false);
  
  stringstream s;
  logo.setout(&s);

  BOOST_CHECK_EQUAL(logo.callword("SETUP"), 0);
  DEBUG_STEP_DUMP(4, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(s.str(), "=== SETUP\n");

  logo.resetcode();

  stringstream s2;
  logo.setout(&s2);
  DEBUG_STEP_DUMP(4, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
   BOOST_CHECK_EQUAL(s2.str(), "=== 0\n");
 
}
