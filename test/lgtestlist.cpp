/*
  lgtestlist.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 28-Jun-2023
  
  Tests for lists
  
  This work is licensed under the Creative Commons Attribution 4.0 International License. 
  To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/ or 
  send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  https://github.com/visualopsholdings/tinylogo
*/

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/auto_unit_test.hpp>

#include "../logo.hpp"
#include "../logocompiler.hpp"
#include "../list.hpp"

using namespace std;

BOOST_AUTO_TEST_CASE( sizes )
{
    cout << "=== sizes ===" << endl;
  
   BOOST_CHECK_EQUAL(sizeof(ListNode), 6);
  
}

BOOST_AUTO_TEST_CASE( emptyList )
{
  cout << "=== emptyList ===" << endl;
  
  ListPool pool;
  List list(&pool);
  BOOST_CHECK_EQUAL(list.length(), 0);
  
}

BOOST_AUTO_TEST_CASE( intList )
{
  cout << "=== intList ===" << endl;
  
  ListPool pool;
  List list(&pool);
  list.push(ListNodeVal::newintval(20));
  BOOST_CHECK_EQUAL(list.length(), 1);
  ListNodeVal val;
  list.first(&val);
  BOOST_CHECK(val.isint());
  BOOST_CHECK_EQUAL(val.intval(), 20);
  
}

BOOST_AUTO_TEST_CASE( multipleIntList )
{
  cout << "=== multipleIntList ===" << endl;
  
  ListPool pool;
  List list(&pool);
  list.push(ListNodeVal::newintval(20));
  list.push(ListNodeVal::newintval(30));
  list.push(ListNodeVal::newintval(50));
  BOOST_CHECK_EQUAL(list.length(), 3);
  ListNodeVal val;
  list.pop(&val);
  BOOST_CHECK(val.isint());
  BOOST_CHECK_EQUAL(val.intval(), 20);
  BOOST_CHECK_EQUAL(list.length(), 2);
  list.pop(&val);
  BOOST_CHECK(val.isint());
  BOOST_CHECK_EQUAL(val.intval(), 30);
  BOOST_CHECK_EQUAL(list.length(), 1);
  list.pop(&val);
  BOOST_CHECK(val.isint());
  BOOST_CHECK_EQUAL(val.intval(), 50);
  BOOST_CHECK_EQUAL(list.length(), 0);
  
}

BOOST_AUTO_TEST_CASE( iter )
{
  cout << "=== iter ===" << endl;
  
  ListPool pool;
  List list(&pool);
  list.push(ListNodeVal::newintval(20));
  list.push(ListNodeVal::newintval(30));
  list.push(ListNodeVal::newintval(50));
  BOOST_CHECK_EQUAL(list.length(), 3);
  ListNodeVal val;
  tNodeType node = list.head();
  while (list.iter(&node, &val)) {
    BOOST_CHECK(val.isint());  
  }
  
}

BOOST_AUTO_TEST_CASE( printStringList )
{
  cout << "=== printStringList ===" << endl;
  
  Logo logo;
  LogoCompiler compiler(&logo);

  compiler.compile("print [I am a list]");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  stringstream s;
  logo.setout(&s);

  DEBUG_STEP_DUMP(10, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(s.str(), "=== I am a list\n");
  
}

BOOST_AUTO_TEST_CASE( printMixedList )
{
  cout << "=== printMixedList ===" << endl;
  
  Logo logo;
  LogoCompiler compiler(&logo);

  compiler.compile("print [This is a 0 and this is a 1.2]");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  stringstream s;
  logo.setout(&s);

  DEBUG_STEP_DUMP(10, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(s.str(), "=== This is a 0 and this is a 1.200000\n");
  
}
