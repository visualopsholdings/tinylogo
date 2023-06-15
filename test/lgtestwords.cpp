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

vector<string> gCmds;

//#define PRINT_RESULT

#include "testtimeprovider.hpp"

void ledOn(Logo &logo) {
  gCmds.push_back("LED ON");
#ifdef PRINT_RESULT
  cout << gCmds.back() << endl;
#endif
}

void ledOff(Logo &logo) {
  gCmds.push_back("LED OFF");
#ifdef PRINT_RESULT
  cout << gCmds.back() << endl;
#endif
}

BOOST_AUTO_TEST_CASE( make )
{
  cout << "=== make ===" << endl;
  
  Logo logo(0, 0, Logo::core);
  LogoCompiler compiler(&logo);

  compiler.compile("MAKE \"VAR 10");
  compiler.compile(":VAR");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  gCmds.clear();
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popint(), 10);
  BOOST_CHECK(logo.stackempty());
  DEBUG_DUMP(false);
  
}

BOOST_AUTO_TEST_CASE( makeChange )
{
  cout << "=== makeChange ===" << endl;
  
  LogoBuiltinWord empty[] = {};
  TestTimeProvider time;
  LogoFunctionPrimitives primitives(empty, 0);
  Logo logo(&primitives, &time, Logo::core);
  LogoCompiler compiler(&logo);

  compiler.compile("MAKE \"num 100");
  compiler.compile("MAKE \"num 200");
  compiler.compile("WAIT :num");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  gCmds.clear();
  BOOST_CHECK_EQUAL(logo.run(), 0);
  DEBUG_DUMP(false);
  BOOST_CHECK_EQUAL(gCmds.size(), 1);
  BOOST_CHECK_EQUAL(gCmds[0], "WAIT 200");
  
}

BOOST_AUTO_TEST_CASE( forever )
{
  cout << "=== forever ===" << endl;
  
  LogoBuiltinWord builtins[] = {
    { "ON", &ledOn },
    { "OFF", &ledOff },
  };
  TestTimeProvider time;
  LogoFunctionPrimitives primitives(builtins, sizeof(builtins));
  Logo logo(&primitives, &time, Logo::core);
  LogoCompiler compiler(&logo);

  compiler.compile("TO DOIT;ON WAIT 10 OFF WAIT 20;END");
  compiler.compile("1 2 FOREVER DOIT");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  gCmds.clear();
//  DEBUG_STEP_DUMP(100, false);
  for (int i=0; i<100; i++) {
    BOOST_CHECK_EQUAL(logo.step(), 0);
  }
  BOOST_CHECK_EQUAL(gCmds.size(), 39);
  BOOST_CHECK_EQUAL(gCmds[0], "LED ON");
  BOOST_CHECK_EQUAL(gCmds[1], "WAIT 10");
  BOOST_CHECK_EQUAL(gCmds[2], "LED OFF");
  BOOST_CHECK_EQUAL(gCmds[3], "WAIT 20");
  BOOST_CHECK_EQUAL(gCmds[4], "LED ON");
   
}

BOOST_AUTO_TEST_CASE( repeat )
{
  cout << "=== repeat ===" << endl;
  
  LogoBuiltinWord builtins[] = {
    { "ON", &ledOn },
    { "OFF", &ledOff },
  };
  TestTimeProvider time;
  LogoFunctionPrimitives primitives(builtins, sizeof(builtins));
  Logo logo(&primitives, &time, Logo::core);
  LogoCompiler compiler(&logo);

  compiler.compile("TO DOIT;ON WAIT 10 OFF WAIT 20;END");
  compiler.compile("REPEAT 3 DOIT");
  DEBUG_DUMP(false);
  BOOST_CHECK_EQUAL(logo.geterr(), 0);

  gCmds.clear();
  DEBUG_STEP_DUMP(20, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  
  BOOST_CHECK_EQUAL(gCmds.size(), 12);
  for (int i=0; i<10; i++) {
    BOOST_CHECK_EQUAL(gCmds[0], "LED ON");
    BOOST_CHECK_EQUAL(gCmds[1], "WAIT 10");
    BOOST_CHECK_EQUAL(gCmds[2], "LED OFF");
    BOOST_CHECK_EQUAL(gCmds[3], "WAIT 20");
  }
  
}

BOOST_AUTO_TEST_CASE( eqWord )
{
  cout << "=== eqWord ===" << endl;
  
  Logo logo(0, 0, Logo::core);
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
  
  Logo logo(0, 0, Logo::core);
  LogoCompiler compiler(&logo);

  compiler.compile("1 != 3");
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
  
  Logo logo(0, 0, Logo::core);
  LogoCompiler compiler(&logo);

  compiler.compile("3 > 1");
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
  
  Logo logo(0, 0, Logo::core);
  LogoCompiler compiler(&logo);

  compiler.compile("3 >= 1");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popint(), 1);
  
  logo.reset();
  compiler.reset();
  compiler.compile("10 >= 10");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);
  
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popint(), 1);
  
  BOOST_CHECK(logo.stackempty());
}

BOOST_AUTO_TEST_CASE( ltWord )
{
  cout << "=== ltWord ===" << endl;
  
  Logo logo(0, 0, Logo::core);
  LogoCompiler compiler(&logo);

  compiler.compile("3 < 1");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popint(), 0);
  
  logo.reset();
  compiler.reset();
  compiler.compile("10 < 11");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);
  
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popint(), 1);
  
  BOOST_CHECK(logo.stackempty());
}

BOOST_AUTO_TEST_CASE( lteWord )
{
  cout << "=== lteWord ===" << endl;
  
  Logo logo(0, 0, Logo::core);
  LogoCompiler compiler(&logo);

  compiler.compile("3 <= 1");
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
  
  Logo logo(0, 0, Logo::core);
  LogoCompiler compiler(&logo);

  compiler.compile("NOT 1");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popint(), 0);
  
  logo.reset();
  compiler.reset();
  compiler.compile("NOT 0");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);
  
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popint(), 1);
  
  BOOST_CHECK(logo.stackempty());
}

BOOST_AUTO_TEST_CASE( ifelseFalse )
{
  cout << "=== ifelseFalse ===" << endl;
  
  Logo logo(0, 0, Logo::core);
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
  
  Logo logo(0, 0, Logo::core);
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
  
  Logo logo(0, 0, Logo::core);
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
  
  Logo logo(0, 0, Logo::core);
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
  
  Logo logo(0, 0, Logo::core);
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
  
  Logo logo(0, 0, Logo::core);
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
  
  Logo logo(0, 0, Logo::core);
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

#ifdef LOGO_SENTENCES

BOOST_AUTO_TEST_CASE( ifelseSentences )
{
  cout << "=== ifelseSentences ===" << endl;
  
  Logo logo(0, 0, Logo::core);
  LogoCompiler compiler(&logo);

  compiler.compile("IFELSE [1] [2] [3]");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popint(), 2);
  BOOST_CHECK(logo.stackempty());

}
#endif

BOOST_AUTO_TEST_CASE( ifelseLiteralTrueCond )
{
  cout << "=== ifelseLiteralTrueCond ===" << endl;
  
  Logo logo(0, 0, Logo::core);
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
  
  Logo logo(0, 0, Logo::core);
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
  
  Logo logo(0, 0, Logo::core);
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
  
  Logo logo(0, 0, Logo::core);
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
  
  Logo logo(0, 0, Logo::core);
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
  
  Logo logo(0, 0, Logo::core);
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
  
  Logo logo(0, 0, Logo::core);
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
  
  Logo logo(0, 0, Logo::core);
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
  
  Logo logo(0, 0, Logo::core);
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
  
  Logo logo(0, 0, Logo::core);
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
    strstream str;
    str << "WAIT " << ms;
    gCmds.push_back(str.str());
#ifdef PRINT_RESULT
    cout << gCmds.back() << endl;
#endif
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
  
  LogoBuiltinWord builtins[] = {
    { "ON", &ledOn },
  };
  TestWordTimeProvider time;
  LogoFunctionPrimitives primitives(builtins, sizeof(builtins));
  Logo logo(&primitives, &time, Logo::core);
  LogoCompiler compiler(&logo);
 
  compiler.compile("WAIT 1000 ON");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  time.settime(900);
  
  gCmds.clear();
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(gCmds.size(), 2);

}

BOOST_AUTO_TEST_CASE( waitWordNotReady )
{
  cout << "=== waitWordNotReady ===" << endl;
  
  LogoBuiltinWord builtins[] = {
    { "ON", &ledOn },
  };
  TestWordTimeProvider time;
  LogoFunctionPrimitives primitives(builtins, sizeof(builtins));
  Logo logo(&primitives, &time, Logo::core);
  LogoCompiler compiler(&logo);
 
  compiler.compile("WAIT 1000 ON");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  time.settime(900);
  
  gCmds.clear();
  for (int i=0; i<10; i++) {
    BOOST_CHECK_EQUAL(logo.step(), 0);
  }
  BOOST_CHECK_EQUAL(gCmds.size(), 1);

}

BOOST_AUTO_TEST_CASE( arithmetic )
{
  cout << "=== arithmetic ===" << endl;
  
  Logo logo(0, 0, Logo::core);
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
  
  Logo logo(0, 0, Logo::core);
  LogoCompiler compiler(&logo);

  // sanity check :-)
  double d = ((3 - 1) * 4.0) / 3.0;
  BOOST_CHECK(d > 2.6);
  BOOST_CHECK(d < 2.7);

  // ((3 - 1) * 4) / 3
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

BOOST_AUTO_TEST_CASE( arithmeticColors )
{
  cout << "=== arithmeticColors ===" << endl;
  
  Logo logo(0, 0, Logo::core);
  LogoCompiler compiler(&logo);

  // sanity check :-)
  BOOST_CHECK_EQUAL(((100 - 0) / 100.0) * 255.0, 255);
  BOOST_CHECK_EQUAL(((100 - 100) / 100.0) * 255.0, 0);
  BOOST_CHECK_EQUAL(((100 - 80) / 100.0) * 255.0, 51);

  // ((100 - C) / 100) * 255
  // 255 * 1 / 100 / 100 - C
  compiler.compile("TO SCLR :C; 255*1/   100 / 100 - :C; END");
  compiler.compile("SCLR 0");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

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
