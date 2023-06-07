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
  
  Logo logo;
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

#ifdef LOGO_SENTENCES
BOOST_AUTO_TEST_CASE( dosentences )
{
  cout << "=== dosentences ===" << endl;
  
  Logo logo;
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
  
  Logo logo;
  LogoCompiler compiler(&logo);

  char str[50];
  strcpy(str, "[ON] [WAIT 10] [OFF] [WAIT 20];bbbb");
  compiler.dosentences(str, strlen(str), 0);
  BOOST_CHECK_EQUAL(str, "&0 &1 &2 &3;bbbb");
  
}
#endif

BOOST_AUTO_TEST_CASE( staticPrimitivesfind )
{
  cout << "=== staticPrimitivesfind ===" << endl;
  
  LogoStaticPrimitives primitives;
  LogoSimpleString string("ABC,DEFG:2,DEF");
  primitives.set(&string, 0, string.length());

  LogoSimpleString string3("ABC");
  BOOST_CHECK_EQUAL(primitives.find(&string3, 0, string3.length()), 0);
  
  LogoSimpleString string2("DEFG");
  BOOST_CHECK_EQUAL(primitives.find(&string2, 0, string2.length()), 1);
  
  LogoSimpleString string4("DEF");
  BOOST_CHECK_EQUAL(primitives.find(&string4, 0, string4.length()), 2);
  
  LogoSimpleString string5("XYZ");
  BOOST_CHECK_EQUAL(primitives.find(&string5, 0, string5.length()), -1);
  
}

BOOST_AUTO_TEST_CASE( staticPrimitivesname )
{
  cout << "=== staticPrimitivesname ===" << endl;
  
  LogoStaticPrimitives primitives;
  LogoSimpleString string("ABC,DEFG:2,DEF");
  primitives.set(&string, 0, string.length());

  char name[128];
  primitives.name(0, name, sizeof(name));
  BOOST_CHECK_EQUAL(name, "ABC");

  primitives.name(1, name, sizeof(name));
  BOOST_CHECK_EQUAL(name, "DEFG");

  primitives.name(2, name, sizeof(name));
  BOOST_CHECK_EQUAL(name, "DEF");

}
