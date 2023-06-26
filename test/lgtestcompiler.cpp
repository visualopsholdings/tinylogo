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

BOOST_AUTO_TEST_CASE( scanLine )
{
  cout << "=== scanLine ===" << endl;
  
  Logo logo;
  LogoCompiler compiler(&logo);

  const char *str = "AAAA;BBBBC;";
  LogoSimpleString string(str, strlen(str));

  short start, size;
  short next = compiler.scan(&start, &size, &string, string.length(), 0, true);
  BOOST_CHECK_EQUAL(next, 5);
  BOOST_CHECK_EQUAL(size, 4);
  BOOST_CHECK_EQUAL(string.ncmp("AAAA", start, size), 0);
  next = compiler.scan(&start, &size, &string, string.length(), next, true);
  BOOST_CHECK_EQUAL(next, -1);
  BOOST_CHECK_EQUAL(size, 5);
  BOOST_CHECK_EQUAL(string.ncmp("BBBBC", start, size), 0);

}

BOOST_AUTO_TEST_CASE( scanNewLine )
{
  cout << "=== scanNewLine ===" << endl;
  
  Logo logo;
  LogoCompiler compiler(&logo);

  const char *str = "\n";
  LogoSimpleString string(str, strlen(str));

  short start, size;
  short next = compiler.scan(&start, &size, &string, string.length(), 0, true);
  BOOST_CHECK_EQUAL(next, -1);
  BOOST_CHECK_EQUAL(size, 0);

}

BOOST_AUTO_TEST_CASE( scanWordWS )
{
  cout << "=== scanWordWS ===" << endl;
  
  Logo logo;
  LogoCompiler compiler(&logo);

  const char *str = "AAAA BBBBC CCCCC ";
  LogoSimpleString string(str, strlen(str));

  short start, size;
  short next = compiler.scan(&start, &size, &string, string.length(), 0, false);
  BOOST_CHECK_EQUAL(next, 5);
  BOOST_CHECK_EQUAL(size, 4);
  BOOST_CHECK_EQUAL(string.ncmp("AAAA", start, size), 0);
  next = compiler.scan(&start, &size, &string, string.length(), next, false);
  BOOST_CHECK_EQUAL(next, 11);
  BOOST_CHECK_EQUAL(size, 5);
  BOOST_CHECK_EQUAL(string.ncmp("BBBBC", start, size), 0);
  next = compiler.scan(&start, &size, &string, string.length(), next, false);
  BOOST_CHECK_EQUAL(next, -1);
  BOOST_CHECK_EQUAL(size, 5);
  BOOST_CHECK_EQUAL(string.ncmp("CCCCC", start, size), 0);

}

BOOST_AUTO_TEST_CASE( scanWordNonWS1 )
{
  cout << "=== scanWordNonWS1 ===" << endl;
  
  Logo logo;
  LogoCompiler compiler(&logo);

  const char *str = "3-1";
  LogoSimpleString string(str, strlen(str));

  short start, size;
  short next = compiler.scan(&start, &size, &string, string.length(), 0, false);
  BOOST_CHECK_EQUAL(size, 1);
  BOOST_CHECK_EQUAL(string.ncmp("3", start, size), 0);
  BOOST_CHECK_EQUAL(next, 1);
  next = compiler.scan(&start, &size, &string, string.length(), next, false);
  BOOST_CHECK_EQUAL(size, 1);
  BOOST_CHECK_EQUAL(string.ncmp("-", start, size), 0);
  BOOST_CHECK_EQUAL(next, 2);
  next = compiler.scan(&start, &size, &string, string.length(), next, false);
  BOOST_CHECK_EQUAL(size, 1);
  BOOST_CHECK_EQUAL(string.ncmp("1", start, size), 0);
  BOOST_CHECK_EQUAL(next, -1);

}

BOOST_AUTO_TEST_CASE( scanWordNonWS2 )
{
  cout << "=== scanWordNonWS2 ===" << endl;
  
  Logo logo;
  LogoCompiler compiler(&logo);

  const char *str = "AAAA+BBBBC-CCCCC";
  LogoSimpleString string(str, strlen(str));

  short start, size;
  short next = compiler.scan(&start, &size, &string, string.length(), 0, false);
  BOOST_CHECK_EQUAL(size, 4);
  BOOST_CHECK_EQUAL(string.ncmp("AAAA", start, size), 0);
  BOOST_CHECK_EQUAL(next, 4);
  next = compiler.scan(&start, &size, &string, string.length(), next, false);
  BOOST_CHECK_EQUAL(size, 1);
  BOOST_CHECK_EQUAL(string.ncmp("+", start, size), 0);
  BOOST_CHECK_EQUAL(next, 5);
  next = compiler.scan(&start, &size, &string, string.length(), next, false);
  BOOST_CHECK_EQUAL(size, 5);
  BOOST_CHECK_EQUAL(string.ncmp("BBBBC", start, size), 0);
  BOOST_CHECK_EQUAL(next, 10);
  next = compiler.scan(&start, &size, &string, string.length(), next, false);
  BOOST_CHECK_EQUAL(size, 1);
  BOOST_CHECK_EQUAL(string.ncmp("-", start, size), 0);
  BOOST_CHECK_EQUAL(next, 11);
  next = compiler.scan(&start, &size, &string, string.length(), next, false);
  BOOST_CHECK_EQUAL(size, 5);
  BOOST_CHECK_EQUAL(string.ncmp("CCCCC", start, size), 0);
  BOOST_CHECK_EQUAL(next, -1);

}

void alnumswitchcheck(LogoCompiler &compiler, char c) {

  BOOST_CHECK(compiler.switchtoken('A', c, false));
  BOOST_CHECK(compiler.switchtoken(c, 'A', false));
  BOOST_CHECK(compiler.switchtoken('1', c, false));
  BOOST_CHECK(compiler.switchtoken(c, '1', false));
  BOOST_CHECK(compiler.switchtoken('a', c, false));
  BOOST_CHECK(compiler.switchtoken(c, 'a', false));
  
}

void alnumsamecheck(LogoCompiler &compiler, char c1, char c2) {

  BOOST_CHECK(!compiler.switchtoken(c1, c2, false));
  BOOST_CHECK(!compiler.switchtoken(c2, c1, false));

}

void parenswitchheck(LogoCompiler &compiler, char c) {

  BOOST_CHECK(compiler.switchtoken(c, '(', false));
  BOOST_CHECK(compiler.switchtoken('(', c, false));
  BOOST_CHECK(compiler.switchtoken(c, ')', false));
  BOOST_CHECK(compiler.switchtoken(')', c, false));

}

BOOST_AUTO_TEST_CASE( switchtokenWord )
{
  cout << "=== switchtokenWord ===" << endl;
  
  Logo logo;
  LogoCompiler compiler(&logo);

  BOOST_CHECK(!compiler.switchtoken('A', 'A', false));
  BOOST_CHECK(!compiler.switchtoken('1', '1', false));
  BOOST_CHECK(!compiler.switchtoken('a', 'a', false));

  BOOST_CHECK(!compiler.switchtoken('A', '.', false));
  BOOST_CHECK(!compiler.switchtoken('.', 'A', false));
  
  alnumsamecheck(compiler, 'A', 'a');
  alnumsamecheck(compiler, 'A', '1');

  alnumswitchcheck(compiler, '+');
  alnumswitchcheck(compiler, '(');
  alnumswitchcheck(compiler, ')');
    
  BOOST_CHECK(compiler.switchtoken('(', '(', false));
  BOOST_CHECK(compiler.switchtoken(')', ')', false));
  
  parenswitchheck(compiler, '+');
  parenswitchheck(compiler, 'A');
  parenswitchheck(compiler, 'a');
  parenswitchheck(compiler, '1');
  
  // special cases
  BOOST_CHECK(!compiler.switchtoken(':', 'A', false));
  BOOST_CHECK(!compiler.switchtoken(':', 'a', false));

  BOOST_CHECK(!compiler.switchtoken('\"', 'A', false));
  BOOST_CHECK(!compiler.switchtoken('\"', 'a', false));
  
  // internal word defs.
  BOOST_CHECK(!compiler.switchtoken('!', '=', false));
  BOOST_CHECK(!compiler.switchtoken('>', '=', false));
  BOOST_CHECK(!compiler.switchtoken('<', '=', false));
  
}

BOOST_AUTO_TEST_CASE( blankLines )
{
  cout << "=== blankLines ===" << endl;
  
  Logo logo(0);
  LogoCompiler compiler(&logo);

  compiler.compile("TO MULT A B");
  compiler.compile(" ");
  compiler.compile("  :A * :B");
  compiler.compile("\t\n");
  compiler.compile("END");
  compiler.compile("MULT 10 20");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  DEBUG_STEP_DUMP(20, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popint(), 200);
  DEBUG_DUMP(false);

}
