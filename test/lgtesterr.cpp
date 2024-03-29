/*
  lgtesterr.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 6-May-2023
  
  Tiny Logo error tests.
  
  This work is licensed under the Creative Commons Attribution 4.0 International License. 
  To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/ or 
  send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  https://github.com/visualopsholdings/tinylogo
*/

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/auto_unit_test.hpp>

#include "../logo.hpp"
#include "../logocompiler.hpp"
#include "../arduinoflashstring.hpp"

#include <iostream>
#include <vector>
#include <strstream>

using namespace std;

BOOST_AUTO_TEST_CASE( unknownWord )
{
  cout << "=== unknownWord ===" << endl;
  
  Logo logo;
  LogoCompiler compiler(&logo);

  compiler.compile("XXXX");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

}

BOOST_AUTO_TEST_CASE( unknownWordInWord )
{
  cout << "=== unknownWordInWord ===" << endl;
  
  Logo logo;
  LogoCompiler compiler(&logo);

  compiler.compile("TO TEST; ON; END;");
  DEBUG_DUMP(false);
  BOOST_CHECK_EQUAL(logo.geterr(), 0);

}

BOOST_AUTO_TEST_CASE( tooManyWords )
{
  cout << "=== tooManyWords ===" << endl;
  
  Logo logo;
  LogoCompiler compiler(&logo);

  for (short i=1; i<MAX_WORDS+2; i++) {
    strstream str;
    str << "TO W" << i << "; ON; END;";
    compiler.compile(str.str());
  }
  DEBUG_DUMP();
  BOOST_CHECK(logo.haserr(LG_TOO_MANY_WORDS));
}

BOOST_AUTO_TEST_CASE( outOfStrings )
{
  cout << "=== outOfStrings ===" << endl;
  
  Logo logo;
  LogoCompiler compiler(&logo);
  
  short segn = 16;
  short seg = STRING_POOL_SIZE/segn;
  strstream str;
  for (short i=0; i<(segn + 2); i++) {
    for (short j=0; j<seg; j++) {
      str << "A" << i << j;
    }
    str << ";";
  }

  compiler.compile(str.str());
  DEBUG_DUMP(false);
  
  BOOST_CHECK_EQUAL(logo.geterr(), LG_OUT_OF_STRINGS);
  
}

BOOST_AUTO_TEST_CASE( outOfCode )
{
  cout << "=== outOfCode ===" << endl;
  
  Logo logo;
  LogoCompiler compiler(&logo);
  
  strstream str;
  for (short i=0; i<MAX_CODE+2; i++) {
    str << "A;";
  }
  compiler.compile(str.str());
  DEBUG_DUMP(false);
  BOOST_CHECK_EQUAL(logo.geterr(), LG_OUT_OF_CODE);
  
}

void nevercalled(Logo &logo) {
  BOOST_FAIL( "was called!" );
}

BOOST_AUTO_TEST_CASE( tooManyVariables )
{
  cout << "=== tooManyVariables ===" << endl;
  
  Logo logo;
  LogoCompiler compiler(&logo);

  for (short i=1; i<MAX_VARS+2; i++) {
    strstream str;
    str << "MAKE \"v" << i << " " << i;
    compiler.compile(str.str());
  }
  DEBUG_DUMP(false);
  BOOST_CHECK_EQUAL(logo.geterr(), 0);

  BOOST_CHECK_EQUAL(logo.run(), LG_TOO_MANY_VARS);
  
}
