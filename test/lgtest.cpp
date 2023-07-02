/*
  lgtest.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 6-May-2023
  
  Tiny Logo general tests.
  
  This work is licensed under the Creative Commons Attribution 4.0 International License. 
  To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/ or 
  send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  https://github.com/visualopsholdings/tinylogo
*/

#include "../logo.hpp"
#include "../logocompiler.hpp"
#include "../arduinoflashcode.hpp"
#include "../arduinoflashstring.hpp"

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/auto_unit_test.hpp>

#include <iostream>
#include <vector>
#include <strstream>

using namespace std;

BOOST_AUTO_TEST_CASE( compound )
{
  cout << "=== compound ===" << endl;
  
  Logo logo;
  LogoCompiler compiler(&logo);

  compiler.compile("print \"ON print \"OFF");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);
  stringstream s;
  logo.setout(&s);

  DEBUG_STEP_DUMP(3, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(s.str(), "=== ON\n=== OFF\n");
  
}

BOOST_AUTO_TEST_CASE( defineSimpleWord )
{
  cout << "=== defineSimpleWord ===" << endl;
  
  Logo logo;
  LogoCompiler compiler(&logo);

  compiler.compile("to ON; print \"ON; end");
  compiler.compile("ON");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  stringstream s;
  logo.setout(&s);

  DEBUG_STEP_DUMP(3, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(s.str(), "=== ON\n");
  
}

BOOST_AUTO_TEST_CASE( defineCompoundWord )
{
  cout << "=== defineCompoundWord ===" << endl;
  
  Logo logo;
  LogoCompiler compiler(&logo);

  compiler.compile("TO ON; print \"ON; END");
  compiler.compile("TO OFF; print \"OFF; END");
  compiler.compile("TO TEST1; ON WAIT 100 OFF WAIT 20; END;");
  compiler.compile("TO TEST2; OFF WAIT 30 ON WAIT 40; END;");
  compiler.compile("TEST1; TEST2;");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  stringstream s;
  logo.setout(&s);

  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(s.str(), "=== ON\n=== OFF\n=== OFF\n=== ON\n");
  
}

BOOST_AUTO_TEST_CASE( defineMixedCase )
{
  cout << "=== defineCompoundWord ===" << endl;
  
  Logo logo;
  LogoCompiler compiler(&logo);

  compiler.compile("TO ON; print \"ON; END");
  compiler.compile("TO OFF; print \"OFF; END");
  compiler.compile("to test1; ON WAIT 100 OFF WAIT 20; END;");
  compiler.compile("TO Test2; OFF wait 30 ON WAIT 40; end;");
  compiler.compile("test1; Test2;");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  stringstream s;
  logo.setout(&s);

  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(s.str(), "=== ON\n=== OFF\n=== OFF\n=== ON\n");
  
}

BOOST_AUTO_TEST_CASE( nestedWord )
{
  cout << "=== nestedWord ===" << endl;
  
  Logo logo;
  LogoCompiler compiler(&logo);

  compiler.compile("TO ON; print \"ON; END");
  compiler.compile("TO OFF; print \"OFF; END");
  compiler.compile("TO TEST1; ON WAIT 100 OFF WAIT 20; END;");
  compiler.compile("TO TEST2; TEST1; END;");
  compiler.compile("TEST2;");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  stringstream s;
  logo.setout(&s);

  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(s.str(), "=== ON\n=== OFF\n");
  
}

BOOST_AUTO_TEST_CASE( defineCompoundWordRun1 )
{
  cout << "=== defineCompoundWord ===" << endl;
  
  Logo logo;
  LogoCompiler compiler(&logo);

  compiler.compile("TO ON; print \"ON; END");
  compiler.compile("TO OFF; print \"OFF; END");
  compiler.compile("TO TEST1; ON WAIT 10 OFF WAIT 20; END;");
  compiler.compile("TO TEST2; OFF WAIT 30 ON WAIT 40; END;");
  compiler.compile("TEST1;");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  stringstream s;
  logo.setout(&s);

  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(s.str(), "=== ON\n=== OFF\n");
  
}

BOOST_AUTO_TEST_CASE( defineEmptyWord )
{
  cout << "=== defineEmptyWord ===" << endl;
  
  Logo logo;
  LogoCompiler compiler(&logo);

  compiler.compile("TO TEST1; END;");
  compiler.compile("TEST1");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  stringstream s;
  logo.setout(&s);

  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(s.str(), "");
  
}

BOOST_AUTO_TEST_CASE( arityLiteral1 )
{
  cout << "=== arityLiteral1 ===" << endl;
  
  Logo logo;
  LogoCompiler compiler(&logo);

  compiler.compile("TO W :N;PRINT :N;END");
  compiler.compile("W 20");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  stringstream s;
  logo.setout(&s);

  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(s.str(), "=== 20\n");
  
}

BOOST_AUTO_TEST_CASE( arityWord1 )
{
  cout << "=== arityWord1 ===" << endl;
  
  Logo logo;
  LogoCompiler compiler(&logo);

  compiler.compile("TO TIME; 20; END");
  compiler.compile("TO W :N;PRINT :N;END");
  compiler.compile("W TIME");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  stringstream s;
  logo.setout(&s);

  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(s.str(), "=== 20\n");
  
}

BOOST_AUTO_TEST_CASE( arityLiteral )
{
  cout << "=== arityLiteral ===" << endl;
  
  Logo logo;
  LogoCompiler compiler(&logo);

  compiler.compile("TO ARGS2 :A :B;PRINT :A PRINT :B;END");
  compiler.compile("ARGS2 20 XXXX");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  stringstream s;
  logo.setout(&s);

  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(s.str(), "=== XXXX\n=== 20\n");
  
}

BOOST_AUTO_TEST_CASE( arityWord )
{
  cout << "=== arityWord ===" << endl;
  
  Logo logo;
  LogoCompiler compiler(&logo);

  compiler.compile("TO TIME; 20; END");
  compiler.compile("TO AAA; XXXX; END");
  compiler.compile("TO ARGS2 :A :B;PRINT :A PRINT :B;END");
  compiler.compile("ARGS2 TIME AAA");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  stringstream s;
  logo.setout(&s);

  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(s.str(), "=== XXXX\n=== 20\n");
  
}

BOOST_AUTO_TEST_CASE( arityNested )
{
  cout << "=== arityNested ===" << endl;
  
  Logo logo;
  LogoCompiler compiler(&logo);

  compiler.compile("TO AR1 :N2; PRINT :N2; END");
  compiler.compile("TO AR2 :N1; :N1; END");
  compiler.compile("AR1 AR2 20");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);
  
  stringstream s;
  logo.setout(&s);

  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(s.str(), "=== 20\n");
  
}

BOOST_AUTO_TEST_CASE( seperateLines )
{
  cout << "=== seperateLines ===" << endl;
  
  Logo logo;
  LogoCompiler compiler(&logo);

  compiler.compile("TO SARG :S; PRINT :S; END");
  compiler.compile("TO TEST\n");
  compiler.compile("  SARG \"XXX\n");
  compiler.compile("END\n");
  compiler.compile("TEST");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  stringstream s;
  logo.setout(&s);

  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(s.str(), "=== XXX\n");
  
}

BOOST_AUTO_TEST_CASE( newLines )
{
  cout << "=== newLines ===" << endl;
  
  Logo logo;
  LogoCompiler compiler(&logo);

  compiler.compile("TO SARG :S; PRINT :S; END");
  compiler.compile("TO TEST\n\tSARG \"XXX\nEND\nTEST");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  stringstream s;
  logo.setout(&s);

  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(s.str(), "=== XXX\n");
  
}

BOOST_AUTO_TEST_CASE( restart )
{
  cout << "=== restart ===" << endl;
  
  Logo logo;
  LogoCompiler compiler(&logo);

  compiler.compile("TO SARG :S; PRINT :S; END");
  compiler.compile("TO TEST\n\tSARG \"XXX\nEND\nTEST");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  stringstream s;
  logo.setout(&s);

  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(s.str(), "=== XXX\n");
  
  stringstream s2;
  logo.setout(&s2);
  logo.restart();
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(s2.str(), "=== XXX\n");
  
}

BOOST_AUTO_TEST_CASE( literalsOnStack )
{
  cout << "=== literalsOnStack ===" << endl;
  
  Logo logo;
  LogoCompiler compiler(&logo);

  compiler.compile("TO NUM; 1 2 3; END; NUM");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popint(), 3);
  BOOST_CHECK_EQUAL(logo.popint(), 2);
  BOOST_CHECK_EQUAL(logo.popint(), 1);
  DEBUG_DUMP(false);
  
}

BOOST_AUTO_TEST_CASE( sizes )
{
  cout << "=== sizes ===" << endl;
  
  // half this size on an arduino
  BOOST_CHECK_EQUAL(sizeof(tLogoInstruction), 6);
  BOOST_CHECK_EQUAL(sizeof(LogoWord), 6);
  BOOST_CHECK_EQUAL(sizeof(LogoVar), 10);
  
}

BOOST_AUTO_TEST_CASE( arguments )
{
  cout << "=== arguments ===" << endl;
  
  Logo logo;
  LogoCompiler compiler(&logo);

  compiler.compile("TO MULT :A :B; :A * :B; END;");
  compiler.compile("MULT 10 20");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  DEBUG_STEP_DUMP(20, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popint(), 200);
  DEBUG_DUMP(false);
  
}

BOOST_AUTO_TEST_CASE( argumentsNoColons )
{
  cout << "=== argumentsNoColons ===" << endl;
  
  Logo logo;
  LogoCompiler compiler(&logo);

  compiler.compile("TO MULT A B; :A * :B; END;");
  compiler.compile("MULT 10 20");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  DEBUG_STEP_DUMP(20, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popint(), 200);
  DEBUG_DUMP(false);
  
}

static const char program_flash[] PROGMEM = 
  "TO MULT :A :B; :A * :B; END;"
  "MULT 10 20";
  ;

BOOST_AUTO_TEST_CASE( flash )
{
  cout << "=== flash ===" << endl;
  
  Logo logo;
  LogoCompiler compiler(&logo);

  ArduinoFlashString program(program_flash);
  compiler.compile(&program);
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  DEBUG_STEP_DUMP(20, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popint(), 200);
  DEBUG_DUMP(false);
  
}

static const char strings_fixedStrings[] PROGMEM = {
	"MULT\n"
	"A\n"
	"B\n"
};
  
static const char program_fixedStrings[] PROGMEM = 
  "TO MULT :A :B; :A * :B; END;"
  "MULT 10 20";
//   "TO $1 :$2 :$3; :$2 * :$3; END;"
//   "$1 10 20"
  ;

BOOST_AUTO_TEST_CASE( fixedStrings )
{
  cout << "=== fixedStrings ===" << endl;
  
  ArduinoFlashString strings(strings_fixedStrings);
  Logo logo(0, &strings);
  LogoCompiler compiler(&logo);

  ArduinoFlashString program(program_fixedStrings);
  compiler.compile(&program);
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

//  logo.dumpstringscode(&compiler, "strings_fixedStrings");
  
  DEBUG_STEP_DUMP(20, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popint(), 200);
  DEBUG_DUMP(false);
  
}

BOOST_AUTO_TEST_CASE( getstring )
{
  cout << "=== getstring ===" << endl;
  
  Logo logo;

  const char *s1 = "XXX";
  LogoSimpleString ss1(s1);
  short len1 = ss1.length();
  short str1 = logo.addstring(&ss1, 0, len1);

  const char *s2 = "YYY";
  LogoSimpleString ss2(s2);
  short len2 = ss2.length();
  short str2 = logo.addstring(&ss2, 0, len2);

  LogoStringResult result;
  logo.getstring(&result, str1, len1);
  BOOST_CHECK_EQUAL(result.ncmp(s1), 0);
  
  logo.getstring(&result, str2, len2);
  BOOST_CHECK_EQUAL(result.ncmp(s2), 0);
  
}

BOOST_AUTO_TEST_CASE( stringcmp )
{
  cout << "=== stringcmp ===" << endl;
  
  LogoSimpleString strings(strings_fixedStrings);
  Logo logo(0, &strings);

  LogoSimpleString s1("XXX");
  short len1 = s1.length();
  short str1 = logo.addstring(&s1, 0, len1);

  LogoSimpleString s2("YYY");
  short len2 = s2.length();
  short str2 = logo.addstring(&s2, 0, len2);

  LogoSimpleString t1("XXX");
  BOOST_CHECK(logo.stringcmp(&t1, 0, t1.length(), str1, len1));
  LogoSimpleString t2("YYY");
  BOOST_CHECK(logo.stringcmp(&t2, 0, t2.length(), str2, len2));
  BOOST_CHECK(!logo.stringcmp(&t1, 0, t1.length(), str1, len1+1));
  LogoSimpleString t3("AAA");
  BOOST_CHECK(!logo.stringcmp(&t3, 0, t3.length(), str1, len1));
  LogoSimpleString t4("MULT");
  BOOST_CHECK(logo.stringcmp(&t4, 0, t4.length(), 0, 4));
  LogoSimpleString t5("SSSS");
  BOOST_CHECK(!logo.stringcmp(&t5, 0, t5.length(), 0, 4));
  LogoSimpleString t6("A");
  BOOST_CHECK(logo.stringcmp(&t6, 0, t6.length(), 1, 1));
  LogoSimpleString t7("B");
  BOOST_CHECK(logo.stringcmp(&t7, 0, t7.length(), 2, 1));
  BOOST_CHECK(!logo.stringcmp(&t7, 0, t7.length(), 1, 1));
  LogoSimpleString t8("C");
  BOOST_CHECK(!logo.stringcmp(&t8, 0, t8.length(), 2, 1));
  BOOST_CHECK(!logo.stringcmp(&t8, 0, t8.length(), 4, 1));
  
}

static const char static_program[] PROGMEM = 
  "TO ON; print \"ON; END;"
  "TO OFF; print \"OFF; END;"
  "TO TEST1; ON WAIT 10 OFF WAIT 20; END;"
  "TO TEST2; OFF WAIT 30 ON WAIT 40; END;"
  "TEST1;"
  ;

BOOST_AUTO_TEST_CASE( staticProgUse )
{
  cout << "=== staticProgUse ===" << endl;
  
  Logo logo;
  LogoCompiler compiler(&logo);
  
  compiler.compile(static_program);
  
  stringstream s;
  logo.setout(&s);

  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(s.str(), "=== ON\n=== OFF\n");
  
}

static const char strings_staticCode[] PROGMEM = {
// words
	"ON\n"
	"OFF\n"
	"TEST1\n"
	"TEST2\n"
// variables
// strings
	"ON\n"
	"OFF\n"
};
static const short code_staticCode[][INST_LENGTH] PROGMEM = {
	{ OPTYPE_JUMP, 8, 0 },		// 0
	{ OPTYPE_HALT, 0, 0 },		// 1
	{ OPTYPE_BUILTIN, 18, 1 },		// 2
	{ OPTYPE_STRING, 0, 2 },		// 3
	{ OPTYPE_RETURN, 0, 0 },		// 4
	{ OPTYPE_BUILTIN, 18, 1 },		// 5
	{ OPTYPE_STRING, 1, 3 },		// 6
	{ OPTYPE_RETURN, 0, 0 },		// 7
	{ OPTYPE_JUMP, 2, 0 },		// 8
	{ OPTYPE_BUILTIN, 6, 1 },		// 9
	{ OPTYPE_INT, 10, 0 },		// 10
	{ OPTYPE_JUMP, 5, 0 },		// 11
	{ OPTYPE_BUILTIN, 6, 1 },		// 12
	{ OPTYPE_INT, 20, 0 },		// 13
	{ OPTYPE_RETURN, 0, 0 },		// 14
	{ OPTYPE_JUMP, 5, 0 },		// 15
	{ OPTYPE_BUILTIN, 6, 1 },		// 16
	{ OPTYPE_INT, 30, 0 },		// 17
	{ OPTYPE_JUMP, 2, 0 },		// 18
	{ OPTYPE_BUILTIN, 6, 1 },		// 19
	{ OPTYPE_INT, 40, 0 },		// 20
	{ OPTYPE_RETURN, 0, 0 },		// 21
	{ SCOPTYPE_WORD, 2, 0 }, 
	{ SCOPTYPE_WORD, 5, 0 }, 
	{ SCOPTYPE_WORD, 8, 0 }, 
	{ SCOPTYPE_WORD, 15, 0 }, 
	{ SCOPTYPE_END, 0, 0 } 
};

BOOST_AUTO_TEST_CASE( staticCodeUse )
{
  cout << "=== staticCodeUse ===" << endl;
  
  ArduinoFlashCode code((const PROGMEM short *)code_staticCode);
  ArduinoFlashString strings(strings_staticCode);
  Logo logo(0, &strings, &code);

  stringstream s;
  logo.setout(&s);

  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(s.str(), "=== ON\n=== OFF\n");
  
}

BOOST_AUTO_TEST_CASE( callwordStatic )
{
  cout << "=== callwordStatic ===" << endl;
  
  ArduinoFlashCode code((const PROGMEM short *)code_staticCode);
  ArduinoFlashString strings(strings_staticCode);
  Logo logo(0, &strings, &code);

  stringstream s;
  logo.setout(&s);

  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(s.str(), "=== ON\n=== OFF\n");
  
  logo.resetcode();
  BOOST_CHECK_EQUAL(logo.callword("TEST2"), 0);
  
  stringstream s2;
  logo.setout(&s2);

  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(s2.str(), "=== OFF\n=== ON\n");
  
}

BOOST_AUTO_TEST_CASE( callwordCompiled )
{
  cout << "=== callwordCompiled ===" << endl;
  
  Logo logo;
  LogoCompiler compiler(&logo);

  compiler.compile("to AAAA; print \"AAAA; end");
  compiler.compile("to BBBB; print \"BBBB; end");
  compiler.compile("AAAA");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  BOOST_CHECK_EQUAL(compiler.callword("BBBB"), 0);
  
  stringstream s1;
  logo.setout(&s1);

  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(s1.str(), "=== BBBB\n");
  
  stringstream s2;
  logo.setout(&s2);

  logo.restart();
  DEBUG_STEP_DUMP(3, false);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(s2.str(), "=== AAAA\n");
  
}

static const char strings_setvar[] PROGMEM = {
// words
	"TEST1\n"
// variables
// strings
};
static const short code_setvar[][INST_LENGTH] PROGMEM = {
	{ OPTYPE_JUMP, 2, 0 },		// 0
	{ OPTYPE_HALT, 0, 0 },		// 1
	{ OPTYPE_REF, 1, 3 },		// 2
	{ OPTYPE_RETURN, 0, 0 },		// 3
	{ SCOPTYPE_WORD, 2, 0 }, 
	{ SCOPTYPE_END, 0, 0 } 
};

BOOST_AUTO_TEST_CASE( setvar )
{
  cout << "=== setvar ===" << endl;
  
  ArduinoFlashCode code((const PROGMEM short *)code_setvar);
  ArduinoFlashString strings(strings_setvar);
  Logo logo(0, &strings, &code);

  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popint(), 0);
  
  logo.resetcode();
  BOOST_CHECK_EQUAL(logo.callword("VAR=2"), 0);
  BOOST_CHECK_EQUAL(logo.run(), 0);

  logo.resetcode();
  BOOST_CHECK_EQUAL(logo.callword("TEST1"), 0);
  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(logo.popint(), 2);
  
}

BOOST_AUTO_TEST_CASE( findfixed )
{
  cout << "=== findfixed ===" << endl;
  
  char fixedstrings[] = {
	"TE\n"
	"TEST\n"
	"OFF\n"
	"O\n"
	"OF\n"
  };
  LogoSimpleString strings(fixedstrings);
  Logo logo(0, &strings);

  LogoSimpleString str1("TE");
  BOOST_CHECK_EQUAL(logo.findfixed(&str1, 0, str1.length()), 0);
  
  LogoSimpleString str2("TEST");
  BOOST_CHECK_EQUAL(logo.findfixed(&str2, 0, str2.length()), 1);
  
  LogoSimpleString str3("OFF");
  BOOST_CHECK_EQUAL(logo.findfixed(&str3, 0, str3.length()), 2);
  
  LogoSimpleString str4("O");
  BOOST_CHECK_EQUAL(logo.findfixed(&str4, 0, str4.length()), 3);
  
  LogoSimpleString str5("OF");
  BOOST_CHECK_EQUAL(logo.findfixed(&str5, 0, str5.length()), 4);
  
}

BOOST_AUTO_TEST_CASE( fullStrings )
{
  cout << "=== fullStrings ===" << endl;
  
  Logo logo;
  LogoCompiler compiler(&logo);

  compiler.compile("print \"TP-LinkXXXX\"");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  stringstream s;
  logo.setout(&s);

  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(s.str(), "=== TP-LinkXXXX\n");
  
}

BOOST_AUTO_TEST_CASE( fullStringsSpace )
{
  cout << "=== fullStringsSpace ===" << endl;
  
  Logo logo;
  LogoCompiler compiler(&logo);

  compiler.compile("print \"A\\ string");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  stringstream s;
  logo.setout(&s);

  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(s.str(), "=== A\\ string\n");
  
}

BOOST_AUTO_TEST_CASE( fullStringsNumber )
{
  cout << "=== fullStringsNumber ===" << endl;
  
  Logo logo;
  LogoCompiler compiler(&logo);

  compiler.compile("print \"69488764\"");
  BOOST_CHECK_EQUAL(logo.geterr(), 0);
  DEBUG_DUMP(false);

  stringstream s;
  logo.setout(&s);

  BOOST_CHECK_EQUAL(logo.run(), 0);
  BOOST_CHECK_EQUAL(s.str(), "=== 69488764\n");
  
}


