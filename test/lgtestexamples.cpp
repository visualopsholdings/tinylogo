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

using namespace std;

BOOST_AUTO_TEST_CASE( page3_plural )
{
  cout << "=== page3_plural ===" << endl;
  
  Logo logo(0);
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
  
  Logo logo(0);
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

