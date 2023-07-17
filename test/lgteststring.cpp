/*
  lgteststring.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 6-May-2023
  
  Tiny Logo string tests.
  
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

BOOST_AUTO_TEST_CASE( positiveToi )
{
  cout << "=== positiveToi ===" << endl;
  
  LogoSimpleString str("12345");
  BOOST_CHECK_EQUAL(str.toi(0, str.length()), 12345);

}

BOOST_AUTO_TEST_CASE( negativeToi )
{
  cout << "=== negativeToi ===" << endl;
  
  LogoSimpleString str("-12345");
  BOOST_CHECK_EQUAL(str.toi(0, str.length()), -12345);

}

BOOST_AUTO_TEST_CASE( nonNumToi )
{
  cout << "=== nonNumToi ===" << endl;
  
  LogoSimpleString str("12A34");
  BOOST_CHECK_EQUAL(str.toi(0, str.length()), 0);

}

BOOST_AUTO_TEST_CASE( inMiddleToi )
{
  cout << "=== inMiddleToi ===" << endl;
  
  LogoSimpleString str("a;12345;bbbb");
  BOOST_CHECK_EQUAL(str.toi(2, 5), 12345);

}

BOOST_AUTO_TEST_CASE( atEndToi )
{
  cout << "=== atEndToi ===" << endl;
  
  LogoSimpleString str("a;bbbbb;12345");
  BOOST_CHECK_EQUAL(str.toi(8, 5), 12345);
  BOOST_CHECK_EQUAL(str.toi(8, 9), 12345);

}

BOOST_AUTO_TEST_CASE( strFind )
{
  cout << "=== strFind ===" << endl;
  
  LogoSimpleString str("a;[a];12345");
  BOOST_CHECK_EQUAL(str.find('[', 0, str.length()), 2);
  BOOST_CHECK_EQUAL(str.find('[', 1, str.length()), 2);
  BOOST_CHECK_EQUAL(str.find('[', 3, str.length()), -1);
  BOOST_CHECK_EQUAL(str.find('[', 4, str.length()), -1);

}

BOOST_AUTO_TEST_CASE( fixedStrings )
{
  cout << "=== fixedStrings ===" << endl;
  
  LogoSimpleString strings("MULT\nA\nB\n");
  LogoSimpleString str1(";A");
  BOOST_CHECK_EQUAL(strings.ncmp2(&str1, 1, 5, 1), 0);
  LogoSimpleString str2("MULTxxxxx");
  BOOST_CHECK_EQUAL(strings.ncmp2(&str2, 0, 0, 4), 0);
  LogoSimpleString str3("B");
  BOOST_CHECK_EQUAL(strings.ncmp2(&str3, 0, 7, 1), 0);
  LogoSimpleString str4("MULTX");
  BOOST_CHECK_EQUAL(strings.ncmp2(&str4, 0, 5, 1), -1);
  BOOST_CHECK_EQUAL(strings.ncmp2(&str4, 0, 3, 1), -1);

}

BOOST_AUTO_TEST_CASE( escapeSpaces )
{
  cout << "=== escapeSpaces ===" << endl;
  
  LogoStringResult result;
  const char *s = "asa+bccc++ddddd";
  result._simple.set(s, strlen(s));
  char buf[32];
  result.ncpyesc(buf, sizeof(buf));
  BOOST_CHECK_EQUAL(buf, "asa bccc  ddddd");

}


