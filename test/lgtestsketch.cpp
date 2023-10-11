/*
  lgtestsketch.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 8-Jul-2023
  
  Tiny Logo sketch tests.
  
  This work is licensed under the Creative Commons Attribution 4.0 International License. 
  To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/ or 
  send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  https://github.com/visualopsholdings/tinylogo
*/

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/auto_unit_test.hpp>

#include "../logo.hpp"
#include "../logowords.hpp"

using namespace std;

BOOST_AUTO_TEST_CASE( extractStringEvent )
{
  cout << "=== extractStringEvent ===" << endl;
  
  LogoSimpleString evt("[\"id\",\"aaaaa\"]");
  char s[32];  
  BOOST_CHECK(LogoWords::extractEventName(&evt, s, sizeof(s)));
  BOOST_CHECK_EQUAL(s, "id");
   
}

BOOST_AUTO_TEST_CASE( extractObjEvent )
{
  cout << "=== extractObjEvent ===" << endl;
  
  LogoSimpleString evt("[\"id\",{ \"type\": \"aaaa\" }]");  
  char s[32];  
  BOOST_CHECK(LogoWords::extractEventName(&evt, s, sizeof(s)));
  BOOST_CHECK_EQUAL(s, "id");
  
}
