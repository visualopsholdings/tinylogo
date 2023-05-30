/*
  lgtestcompiler.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 6-May-2023
  
  Tiny Logo compiler tests.
  
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

BOOST_AUTO_TEST_CASE( scanfor )
{
  cout << "=== scanfor ===" << endl;
  
  LogoBuiltinWord empty[] = {};
  Logo logo(empty, 0, 0);
  LogoCompiler compiler(&logo);

  const char *str = "AAAA;BBBBC;";
  LogoSimpleString string(str, strlen(str));

//   char buf[32];
//   short next = compiler.scanfor(buf, sizeof(buf), &string, string.length(), 0, true);
//   BOOST_CHECK_EQUAL(next, 5);
//   next = compiler.scanfor(buf, sizeof(buf), &string, string.length(), next, true);
//   BOOST_CHECK_EQUAL(next, 11);
//   next = compiler.scanfor(buf, sizeof(buf), &string, string.length(), next, true);
//   BOOST_CHECK_EQUAL(next, -1);

  short start, size;
  short next = compiler.scanfor(&start, &size, &string, 0, string.length(), true);
  BOOST_CHECK_EQUAL(next, 5);
  BOOST_CHECK_EQUAL(string.ncmp("AAAA", start, size), 0);
  next = compiler.scanfor(&start, &size, &string, next, string.length(), true);
  BOOST_CHECK_EQUAL(next, 11);
  BOOST_CHECK_EQUAL(string.ncmp("BBBBC", start, size), 0);
  next = compiler.scanfor(&start, &size, &string, next, string.length(), true);
  BOOST_CHECK_EQUAL(next, -1);

}

BOOST_AUTO_TEST_CASE( dosentences )
{
  cout << "=== dosentences ===" << endl;
  
  LogoBuiltinWord empty[] = {};
  Logo logo(empty, 0, 0);
  LogoCompiler compiler(&logo);

  char str[50];
  strcpy(str, "aaaaa;[ON] [WAIT 10] [OFF] [WAIT 20];bbbb");
  const char *s = strstr(str, "[");
  compiler.dosentences(str, strlen(str), s);
  BOOST_CHECK_EQUAL(str, "aaaaa;&0 &1 &2 &3;bbbb");
  
}

BOOST_AUTO_TEST_CASE( dosentencesStart )
{
  cout << "=== dosentencesStart ===" << endl;
  
  LogoBuiltinWord empty[] = {};
  Logo logo(empty, 0, 0);
  LogoCompiler compiler(&logo);

  char str[50];
  strcpy(str, "[ON] [WAIT 10] [OFF] [WAIT 20];bbbb");
  compiler.dosentences(str, strlen(str), 0);
  BOOST_CHECK_EQUAL(str, "&0 &1 &2 &3;bbbb");
  
}
