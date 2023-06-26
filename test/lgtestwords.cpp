/*
  lgtestwords.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 6-May-2023
  
  Tiny Logo sketch builtin word tests.
  
  This work is licensed under the Creative Commons Attribution 4.0 International License. 
  To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/ or 
  send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  https://github.com/visualopsholdings/tinylogo
*/

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/auto_unit_test.hpp>

#include "../logo.hpp"
#include "../logocompiler.hpp"

#include <iostream>
#include <vector>
#include <strstream>

using namespace std;

#include "testtimeprovider.hpp"

BOOST_AUTO_TEST_CASE( make )
{
  cout << "=== make ===" << endl;
  
  Logo logo(0);
  LogoCompiler compiler(&logo);

  compiler.compile("MAKE \"VAR 10");
  compiler.compile(":VAR");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popint(), 10);
  BOOST_CHECK(logo.stackempty());
  DEBUG_DUMP(false);
  
}

BOOST_AUTO_TEST_CASE( makeChange )
{
  cout << "=== makeChange ===" << endl;
  
  TestTimeProvider time;
  Logo logo(&time);
  LogoCompiler compiler(&logo);

  compiler.compile("MAKE \"num 100");
  compiler.compile("MAKE \"num 200");
  compiler.compile(":num");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popint(), 200);
  BOOST_CHECK(logo.stackempty());
  DEBUG_DUMP(false);
  
}

BOOST_AUTO_TEST_CASE( makeCompound1 )
{
  cout << "=== makeCompound1 ===" << endl;
  
  Logo logo(0);
  LogoCompiler compiler(&logo);

  compiler.compile("MAKE \"VAR 1");
  compiler.compile("MAKE \"VAR !:VAR");
  compiler.compile(":VAR");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popint(), 0);
  BOOST_CHECK(logo.stackempty());
  DEBUG_DUMP(false);
  
}

BOOST_AUTO_TEST_CASE( makeCompound2 )
{
  cout << "=== makeCompound2 ===" << endl;
  
  Logo logo(0);
  LogoCompiler compiler(&logo);

  compiler.compile("MAKE \"VAR 1");
  compiler.compile("MAKE \"VAR :VAR+1");
  compiler.compile(":VAR");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popint(), 2);
  BOOST_CHECK(logo.stackempty());
  DEBUG_DUMP(false);
  
}

BOOST_AUTO_TEST_CASE( thing )
{
  cout << "=== thing ===" << endl;
  
  Logo logo(0);
  LogoCompiler compiler(&logo);

  compiler.compile("make \"VAR 1");
  compiler.compile("make \"VAR :VAR+1");
  compiler.compile("thing \"VAR");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popint(), 2);
  BOOST_CHECK(logo.stackempty());
  DEBUG_DUMP(false);
  
}

BOOST_AUTO_TEST_CASE( word )
{
  cout << "=== word ===" << endl;
  
  Logo logo(0);
  LogoCompiler compiler(&logo);

  compiler.compile("word \"START \"END");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  DEBUG_STEP_DUMP(10, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  LogoStringResult str;
  logo.popstring(&str);  
  BOOST_CHECK_EQUAL(str.ncmp("STARTEND"), 0);
  BOOST_CHECK(logo.stackempty());
  
}

BOOST_AUTO_TEST_CASE( first )
{
  cout << "=== first ===" << endl;
  
  Logo logo(0);
  LogoCompiler compiler(&logo);

  compiler.compile("first \"START");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  DEBUG_STEP_DUMP(10, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  LogoStringResult str;
  logo.popstring(&str);  
  BOOST_CHECK_EQUAL(str.ncmp("S"), 0);
  BOOST_CHECK(logo.stackempty());
  
}

BOOST_AUTO_TEST_CASE( forever )
{
  cout << "=== forever ===" << endl;
  
  TestTimeProvider time;
  Logo logo(&time);
  LogoCompiler compiler(&logo);

  compiler.compile("TO ON; print \"ON; END");
  compiler.compile("TO OFF; print \"OFF; END");
  compiler.compile("TO DOIT;ON WAIT 10 OFF WAIT 20;END");
  compiler.compile("1 2 FOREVER DOIT");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  stringstream s;
  logo.setout(&s);

  for (int i=0; i<100; i++) {
    BOOST_CHECK_EQUAL(logo.step(), 0);
  }
  BOOST_CHECK_EQUAL(s.str(), "=== ON\n=== OFF\n=== ON\n=== OFF\n=== ON\n=== OFF\n=== ON\n=== OFF\n=== ON\n=== OFF\n=== ON\n");

}

BOOST_AUTO_TEST_CASE( repeat )
{
  cout << "=== repeat ===" << endl;
  
  TestTimeProvider time;
  Logo logo(&time);
  LogoCompiler compiler(&logo);

  compiler.compile("TO ON; print \"ON; END");
  compiler.compile("TO OFF; print \"OFF; END");
  compiler.compile("TO DOIT;ON WAIT 10 OFF WAIT 20;END");
  compiler.compile("REPEAT 3 DOIT");
  DEBUG_DUMP(false);
  BOOST_CHECK_EQUAL(logo.geterr(), 0);

  stringstream s;
  logo.setout(&s);

  BOOST_CHECK_EQUAL(logo.run(), 0);
  
  BOOST_CHECK_EQUAL(s.str(), "=== ON\n=== OFF\n=== ON\n=== OFF\n=== ON\n=== OFF\n");
  
}

BOOST_AUTO_TEST_CASE( eqWord )
{
  cout << "=== eqWord ===" << endl;
  
  Logo logo(0);
  LogoCompiler compiler(&logo);

  compiler.compile("1 = 3");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popint(), 0);
  
  logo.reset();
  compiler.reset();
  compiler.compile("10 = 10");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);
  
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popint(), 1);
  
  BOOST_CHECK(logo.stackempty());
}

BOOST_AUTO_TEST_CASE( neqWord )
{
  cout << "=== neqWord ===" << endl;
  
  Logo logo(0);
  LogoCompiler compiler(&logo);

  compiler.compile("1!= 3");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popint(), 1);
  
  logo.reset();
  compiler.reset();
  compiler.compile("10 != 10");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);
  
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popint(), 0);
  
  BOOST_CHECK(logo.stackempty());
}

BOOST_AUTO_TEST_CASE( gtWord )
{
  cout << "=== gtWord ===" << endl;
  
  Logo logo(0);
  LogoCompiler compiler(&logo);

  compiler.compile("3  >1");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popint(), 1);
  
  logo.reset();
  compiler.reset();
  compiler.compile("10 > 11");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);
  
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popint(), 0);
  
  BOOST_CHECK(logo.stackempty());
}

BOOST_AUTO_TEST_CASE( gteWord )
{
  cout << "=== gteWord ===" << endl;
  
  Logo logo(0);
  LogoCompiler compiler(&logo);

  compiler.compile("3>=1 ");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popint(), 1);
  
  logo.reset();
  compiler.reset();
  compiler.compile("10   >=10");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);
  
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popint(), 1);
  
  BOOST_CHECK(logo.stackempty());
}

BOOST_AUTO_TEST_CASE( ltWord )
{
  cout << "=== ltWord ===" << endl;
  
  Logo logo(0);
  LogoCompiler compiler(&logo);

  compiler.compile("3 <1");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popint(), 0);
  
  logo.reset();
  compiler.reset();
  compiler.compile("10<   11");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);
  
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popint(), 1);
  
  BOOST_CHECK(logo.stackempty());
}

BOOST_AUTO_TEST_CASE( lteWord )
{
  cout << "=== lteWord ===" << endl;
  
  Logo logo(0);
  LogoCompiler compiler(&logo);

  compiler.compile("3<=  1");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popint(), 0);
  
  logo.reset();
  compiler.reset();
  compiler.compile("10 <= 10");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);
  
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popint(), 1);
  
  BOOST_CHECK(logo.stackempty());
}

BOOST_AUTO_TEST_CASE( notWord )
{
  cout << "=== notWord ===" << endl;
  
  Logo logo(0);
  LogoCompiler compiler(&logo);

  compiler.compile("! 1");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popint(), 0);
  
  logo.reset();
  compiler.reset();
  compiler.compile("!0");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);
  
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popint(), 1);
  
  BOOST_CHECK(logo.stackempty());
}

BOOST_AUTO_TEST_CASE( ifelseFalse )
{
  cout << "=== ifelseFalse ===" << endl;
  
  Logo logo(0);
  LogoCompiler compiler(&logo);

  compiler.compile("TO TEST; 0; END");
  compiler.compile("TO THEN; 2; END");
  compiler.compile("TO ELSE; 3; END");
  compiler.compile("IFELSE TEST THEN ELSE");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  DEBUG_STEP_DUMP(10, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popint(), 3);
  BOOST_CHECK(logo.stackempty());

}

BOOST_AUTO_TEST_CASE( ifelseTrue )
{
  cout << "=== ifelseTrue ===" << endl;
  
  Logo logo(0);
  LogoCompiler compiler(&logo);

  compiler.compile("TO TEST; 1; END");
  compiler.compile("TO THEN; 2; END");
  compiler.compile("TO ELSE; 3; END");
  compiler.compile("IFELSE TEST THEN ELSE");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  DEBUG_STEP_DUMP(10, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popint(), 2);
  BOOST_CHECK(logo.stackempty());

}

BOOST_AUTO_TEST_CASE( ifFalse )
{
  cout << "=== ifFalse ===" << endl;
  
  Logo logo(0);
  LogoCompiler compiler(&logo);

  compiler.compile("IF 0 2");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  DEBUG_STEP_DUMP(10, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
//  BOOST_CHECK_EQUAL(logo.popint(), 3);
  BOOST_CHECK(logo.stackempty());

}

BOOST_AUTO_TEST_CASE( ifTrue )
{
  cout << "=== ifTrue ===" << endl;
  
  Logo logo(0);
  LogoCompiler compiler(&logo);

  compiler.compile("IF 1 2");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  DEBUG_STEP_DUMP(10, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popint(), 2);
  BOOST_CHECK(logo.stackempty());

}

BOOST_AUTO_TEST_CASE( ifTrueNested )
{
  cout << "=== ifTrueNested ===" << endl;
  
  Logo logo(0);
  LogoCompiler compiler(&logo);

  compiler.compile("TO T1;2;END");
  compiler.compile("IF 1 > 0 T1");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  DEBUG_STEP_DUMP(10, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popint(), 2);
  BOOST_CHECK(logo.stackempty());

}

BOOST_AUTO_TEST_CASE( ifFalseNested )
{
  cout << "=== ifFalseNested ===" << endl;
  
  Logo logo(0);
  LogoCompiler compiler(&logo);

  compiler.compile("TO T1;2;END");
  compiler.compile("IF 0 > 1 T1");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  DEBUG_STEP_DUMP(10, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK(logo.stackempty());

}

BOOST_AUTO_TEST_CASE( ifelseCond )
{
  cout << "=== ifelseCond ===" << endl;
  
  Logo logo(0);
  LogoCompiler compiler(&logo);

//  compiler.compile("TO COND;1 = 0;END");
  compiler.compile("TO COND;1 = 1;END");
  compiler.compile("IFELSE COND 2 3");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  DEBUG_STEP_DUMP(10, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
//  BOOST_CHECK_EQUAL(logo.popint(), 3);
  BOOST_CHECK_EQUAL(logo.popint(), 2);
  BOOST_CHECK(logo.stackempty());

}

BOOST_AUTO_TEST_CASE( ifelseLiteralTrueCond )
{
  cout << "=== ifelseLiteralTrueCond ===" << endl;
  
  Logo logo(0);
  LogoCompiler compiler(&logo);

  compiler.compile("IFELSE 1 2 3");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  DEBUG_STEP_DUMP(10, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popint(), 2);
  BOOST_CHECK(logo.stackempty());

}

BOOST_AUTO_TEST_CASE( ifelseLiteralFalseCond )
{
  cout << "=== ifelseLiteralFalseCond ===" << endl;
  
  Logo logo(0);
  LogoCompiler compiler(&logo);

  compiler.compile("IFELSE 0 2 3");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  DEBUG_STEP_DUMP(10, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popint(), 3);
  BOOST_CHECK(logo.stackempty());

}

BOOST_AUTO_TEST_CASE( ifelseTrueLiteralNumBranches )
{
  cout << "=== ifelseTrueLiteralNumBranches ===" << endl;
  
  Logo logo(0);
  LogoCompiler compiler(&logo);

  compiler.compile("IFELSE 1 2 3");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  DEBUG_STEP_DUMP(10, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popint(), 2);
  BOOST_CHECK(logo.stackempty());

}

BOOST_AUTO_TEST_CASE( ifelseFalseLiteralNumBranches )
{
  cout << "=== ifelseFalseLiteralNumBranches ===" << endl;
  
  Logo logo(0);
  LogoCompiler compiler(&logo);

  compiler.compile("IFELSE 0 2 3");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  DEBUG_STEP_DUMP(10, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popint(), 3);
  BOOST_CHECK(logo.stackempty());

}

BOOST_AUTO_TEST_CASE( ifelseTrueLiteralStringBranches )
{
  cout << "=== ifelseTrueLiteralStringBranches ===" << endl;
  
  Logo logo(0);
  LogoCompiler compiler(&logo);

  compiler.compile("IFELSE 1 \"A \"B");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  DEBUG_STEP_DUMP(10, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  LogoStringResult str;
  logo.popstring(&str);  
  BOOST_CHECK_EQUAL(str.ncmp("A"), 0);
  BOOST_CHECK(logo.stackempty());

}

BOOST_AUTO_TEST_CASE( ifelseFalseLiteralStringBranches )
{
  cout << "=== ifelseTrueLiteralStringBranches ===" << endl;
  
  Logo logo(0);
  LogoCompiler compiler(&logo);

  compiler.compile("IFELSE 0 \"A \"B");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  DEBUG_STEP_DUMP(10, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  LogoStringResult str;
  logo.popstring(&str);  
  BOOST_CHECK_EQUAL(str.ncmp("B"), 0);
  BOOST_CHECK(logo.stackempty());

}

BOOST_AUTO_TEST_CASE( ifelseVarRef )
{
  cout << "=== ifelseVarRef ===" << endl;
  
  Logo logo(0);
  LogoCompiler compiler(&logo);

  compiler.compile("MAKE \"VAR 1");
  compiler.compile("IFELSE :VAR \"A \"B");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  DEBUG_STEP_DUMP(10, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  LogoStringResult str;
  logo.popstring(&str);  
  BOOST_CHECK_EQUAL(str.ncmp("A"), 0);
  BOOST_CHECK(logo.stackempty());

}

BOOST_AUTO_TEST_CASE( ifelseCondVarRefStringRet )
{
  cout << "=== ifelseCondVarRefStringRet ===" << endl;
  
  Logo logo(0);
  LogoCompiler compiler(&logo);

  compiler.compile("TO COND;:VAR;END");
  compiler.compile("MAKE \"VAR 1");
  compiler.compile("IFELSE COND \"A \"B");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  DEBUG_STEP_DUMP(10, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  LogoStringResult str;
  logo.popstring(&str);  
  BOOST_CHECK_EQUAL(str.ncmp("A"), 0);
  BOOST_CHECK(logo.stackempty());

}

BOOST_AUTO_TEST_CASE( ifelseMissingVar )
{
  cout << "=== ifelseMissingVar ===" << endl;
  
  Logo logo(0);
  LogoCompiler compiler(&logo);

  compiler.compile("IFELSE :VAR \"A \"B");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  DEBUG_STEP_DUMP(10, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  LogoStringResult str;
  logo.popstring(&str);  
  BOOST_CHECK_EQUAL(str.ncmp("B"), 0);
  BOOST_CHECK(logo.stackempty());

}

BOOST_AUTO_TEST_CASE( ifelseGT )
{
  cout << "=== ifelseGT ===" << endl;
  
  Logo logo(0);
  LogoCompiler compiler(&logo);

  compiler.compile("TO COND;2 > 3;END");
  compiler.compile("IFELSE COND 5 6");
//  compiler.compile("IFELSE 2 > 3 5 6");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  DEBUG_STEP_DUMP(10, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popint(), 6);
  BOOST_CHECK(logo.stackempty());

}

class TestWordTimeProvider: public LogoTimeProvider {

public:

  unsigned long currentms() {
    return _time;
  }
  void delayms(unsigned long ms) {
    _time += ms;
  }
  bool testing(short ms) {
    return false;
  }
  void settime(unsigned long time) {
    _time = time;
  }
  
private:
  unsigned long _time;
  
};

BOOST_AUTO_TEST_CASE( waitWordFired )
{
  cout << "=== waitWordFired ===" << endl;
  
  TestWordTimeProvider time;
  Logo logo(&time);
  LogoCompiler compiler(&logo);
 
  compiler.compile("TO ON; print \"ON; END");
  compiler.compile("WAIT 1000 ON");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  time.settime(900);
  
  stringstream s;
  logo.setout(&s);

  BOOST_CHECK_EQUAL(logo.run(), 0);
  
  BOOST_CHECK_EQUAL(s.str(), "=== ON\n");

}

BOOST_AUTO_TEST_CASE( waitWordNotReady )
{
  cout << "=== waitWordNotReady ===" << endl;
  
  TestWordTimeProvider time;
  Logo logo(&time);
  LogoCompiler compiler(&logo);
 
  compiler.compile("TO ON; print \"ON; END");
  compiler.compile("WAIT 1000 ON");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  time.settime(900);
  
  stringstream s;
  logo.setout(&s);

  for (int i=0; i<1000; i++) {
    BOOST_CHECK_EQUAL(logo.step(), 0);
  }
  BOOST_CHECK_EQUAL(s.str(), "");

  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(s.str(), "=== ON\n");
  
}

BOOST_AUTO_TEST_CASE( arithmetic )
{
  cout << "=== arithmetic ===" << endl;
  
  Logo logo(0);
  LogoCompiler compiler(&logo);

  compiler.compile("3-1");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  DEBUG_STEP_DUMP(8, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popdouble(), 2);
  BOOST_CHECK(logo.stackempty());
  
}

BOOST_AUTO_TEST_CASE( arithmeticCompound )
{
  cout << "=== arithmeticCompound ===" << endl;
  
  Logo logo(0);
  LogoCompiler compiler(&logo);

  // sanity check :-)
  double d = ((3 - 1) * 4.0) / 3.0;
  BOOST_CHECK(d > 2.6);
  BOOST_CHECK(d < 2.7);

  compiler.compile("1 / 3 / 4 * 3 - 1");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  DEBUG_STEP_DUMP(8, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  d = logo.popdouble();
  BOOST_CHECK(d > 2.6);
  BOOST_CHECK(d < 2.7);
  BOOST_CHECK(logo.stackempty());
  
}

BOOST_AUTO_TEST_CASE( arithmeticGrouping )
{
  cout << "=== arithmeticGrouping ===" << endl;
  
  Logo logo(0);
  LogoCompiler compiler(&logo);

  compiler.compile("3 - 1 * 4");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  DEBUG_STEP_DUMP(8, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popdouble(), -1);
  
  logo.resetcode();
  compiler.compile("(3 - 1) * 4");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  DEBUG_STEP_DUMP(8, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popdouble(), 8);

  logo.resetcode();
  compiler.compile("3 - (1 * 4)");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  DEBUG_STEP_DUMP(10, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popdouble(), -1);
  
}

BOOST_AUTO_TEST_CASE( nestedGrouping )
{
  cout << "=== nestedGrouping ===" << endl;
  
  Logo logo(0);
  LogoCompiler compiler(&logo);

  compiler.compile("((3 - ((1 * 4))))");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

 DEBUG_STEP_DUMP(10, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popdouble(), -1);
  
}

BOOST_AUTO_TEST_CASE( arithmeticNoWSGrouping )
{
  cout << "=== arithmeticNoWSGrouping ===" << endl;
  
  Logo logo(0);
  LogoCompiler compiler(&logo);

  logo.resetcode();
  compiler.compile("3-(1*4)");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  DEBUG_STEP_DUMP(10, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popdouble(), -1);

}

BOOST_AUTO_TEST_CASE( arithmeticColors )
{
  cout << "=== arithmeticColors ===" << endl;
  
  Logo logo(0);
  LogoCompiler compiler(&logo);

  // sanity check :-)
  BOOST_CHECK_EQUAL(((100 - 0) / 100.0) * 255.0, 255);
  BOOST_CHECK_EQUAL(((100 - 100) / 100.0) * 255.0, 0);
  BOOST_CHECK_EQUAL(((100 - 80) / 100.0) * 255.0, 51);

  compiler.compile("TO SCLR :C; ((100 - :C) / 100) * 255; END");
  compiler.compile("SCLR 0");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  DEBUG_STEP_DUMP(10, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popdouble(), 255);
  BOOST_CHECK(logo.stackempty());

  logo.resetcode();
  compiler.compile("SCLR 100");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);
  
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popdouble(), 0);
  BOOST_CHECK(logo.stackempty());

  logo.resetcode();
  compiler.compile("SCLR 80");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);
  
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popdouble(), 51);
  BOOST_CHECK(logo.stackempty());
}

