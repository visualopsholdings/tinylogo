/*
  logo.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 5-May-2023
  
  Tiniest Logo Intepreter
  
  Keep feeding it commands and it will execute them.
  
  Use it like this:
  
  void ledOn() {
   ... turn a LED on
  }

  void ledOff() {
    ... turn an LED off
  }

  LogoBuiltinWord builtins[] = {
    { "ON", &ledOn },
    { "OFF", &ledOff },
  };
  ArduinoTimeProvider time;
  LogoFunctionPrimitives primitives(builtins, sizeof(builtins));
  Logo logo(&primitives, &time, &Logo::core);
  LogoCompiler compiler(&logo);
  
  compiler.compile("TO GO; FOREVER [ON WAIT 100 OFF WAIT 1000]; END;");

  ... then on some trigger
  compiler.compile("GO");
  int err = logo.geterr();
  if (err) {
    ... do something with the error number
  }
  
  ... and to run it, call this pretty often
  int err = logo.step();
  if (err && err != LG_STOP) {
    ... do something with the error number
  }
  
  ... or just run it till it ends (the above one doesn't)
  int err = logo.run();
  if (err) {
    ... do something with the error number
  }
  
  ... at any time you can start from the top again with
  logo.restart();
  
  ... or completely reset the machines code
  logo.reset();
  
  If your code is so simple it only uses YOUR builtins, you can make 
  the runtime smaller by NOT including the core words (at the moment
  this will only save about 8 bytes):
  
  Logo logo(builtins, sizeof(builtins));
  
  ....
  
  This work is licensed under the Creative Commons Attribution 4.0 International License. 
  To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/ or 
  send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  https://github.com/visualopsholdings/tinylogo
*/

#ifndef H_logo
#define H_logo

//#define LOGO_DEBUG

#ifdef ARDUINO
// un comment this if you want to use the logo runtime directly with flash code. This allows
// for MUCH greater stack and vars.
#define USE_FLASH_CODE
#endif

// on some Arduinos this could be MUCH larger.
// if you run out of space, take a look at using flash memory to store your strings 
// and even code.

// it's really important with these numbers that after you compile your code it 
// leaves about 200 bytes for local variables. Otherwise your sketch won't work.

#ifdef USE_FLASH_CODE
#define STRING_POOL_SIZE    256       // these number of bytes
#else
#define STRING_POOL_SIZE    128       // these number of bytes
#endif
#ifdef USE_FLASH_CODE
#define MAX_CODE            255       // 6 bytes each
#else
#define MAX_CODE            80        // 6 bytes each
#endif
#ifdef USE_FLASH_CODE
#define START_JCODE         80        // the start of where the JCODE lies (the words)
#else
#define START_JCODE         30        // the start of where the JCODE lies (the words)
#endif
#ifdef USE_FLASH_CODE
#define MAX_STACK           64        // 6 bytes each
#else
#define MAX_STACK           16        // 6 bytes each
#endif
#ifdef USE_FLASH_CODE
#define MAX_VARS             20       // 10 bytes each
#else
#define MAX_VARS             8        // 10 bytes each
#endif

#ifdef USE_FLASH_CODE
#ifdef ARDUINO
#define CODE_SIZE           2
#else
#define CODE_SIZE           MAX_CODE
#endif
#else
#define CODE_SIZE           MAX_CODE  
#endif

#define NUM_LEN             12        // these number of bytes
#define SENTENCE_LEN        4         // & and 3 more digits

// some settings that can be used to debug directly on an arduino.
// #define STRING_POOL_SIZE    64        // these number of bytes
// #define MAX_WORDS           8         // 6 bytes each
// #define MAX_CODE            60        // 6 bytes each
// #define START_JCODE         20        // the start of where the JCODE lies (the words)
// #define MAX_STACK           8         // 6 bytes each
// #define MAX_VARS            2         // 10 bytes each

#include <string.h>
#include <stdio.h>
#include <math.h>

#ifndef ARDUINO
#include <ostream>
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

// success is 0
#define LG_STOP               1
#define LG_UNHANDLED_OP_TYPE  2
#define LG_TOO_MANY_WORDS     3
#define LG_OUT_OF_STRINGS     4
#define LG_OUT_OF_CODE        5
#define LG_STACK_OVERFLOW     6
#define LG_WORD_NOT_FOUND     7
#define LG_NO_RETURN_ADDRESS  8
#define LG_TOO_MANY_VARS      9
#define LG_NOT_INT            10
#define LG_NOT_STRING         11
#define LG_NOT_CALLABLE       12
#define LG_EXTRA_IN_DEFINE    13
#define LG_FIXED_NO_NEWLINE   14
#define LG_ARITY_NOT_IMPL     15

#define OPTYPE_NOOP           0 //
#define OPTYPE_RETURN         1 //
#define OPTYPE_HALT           2 //
#define OPTYPE_BUILTIN        3 // FIELD_OP = index of builtin, FIELD_OPAND = category 0 = builtin, 1 = core 
#define OPTYPE_ERR            4 // FIELD_OP = error
#define OPTYPE_JUMP           5 // FIELD_OP = where to jump, FIELD_OPAND = possible arity
#define OPTYPE_STRING         6 // FIELD_OP = index of string (-fixedcount), FIELD_OPAND = length of string
#define OPTYPE_INT            7 // FIELD_OP = literal integer
#define OPTYPE_DOUBLE         8 // FIELD_OP = literal integer part of double, FIELD_OPAND is fractional
#define OPTYPE_REF            9 // FIELD_OP = index of string (-fixedcount) with a var in it, FIELD_OPAND = length of string
#define OPTYPE_POPREF         10 // FIELD_OP = index of var to pop into

// only on the stack
#define SOP_START             100
#define SOPTYPE_ARITY         SOP_START + 1 // FIELD_OP = the arity of the builtin function
#define SOPTYPE_RETADDR       SOP_START + 2 // FIELD_OP = the return address. These are just on the stack
#define SOPTYPE_MRETADDR      SOP_START + 3 // FIELD_OP = the offset to modify by
#define SOPTYPE_SKIP          SOP_START + 4 // skip the next instruction if on the stack under a return

// only in static code
#define SCOP_START            200
// words and vars are layed out in teh same order as the fixed strings.
#define SCOPTYPE_WORD         SCOP_START + 1 // FIELD_OP the jump location, FIELD_OPAND the arity
#define SCOPTYPE_VAR          SCOP_START + 2 // FIELD_OP the variable value
#define SCOPTYPE_END          SCOP_START + 3 // the end of static code.

class Logo;

typedef void (*tLogoFp)(Logo &logo);

// types are predefinced and small.
typedef short tStrPool;
typedef short tType;
typedef unsigned char tVarType;
typedef unsigned char tByte;

// allow the code jump to be a char if the code is small.
#if MAX_CODE <= 256
typedef unsigned char tJump;
#define NO_JUMP   255
#else
typedef short tJump;
#define NO_JUMP   -1
#endif

typedef struct {
  const char    *_name;
  tLogoFp       _code;
  tByte         _arity; // smaller than 256?
} LogoBuiltinWord;

// a single instruction.
#define INST_LENGTH   3 // in shorts

typedef short tLogoInstruction[INST_LENGTH];

#define FIELD_OPTYPE  0
#define FIELD_OP      1
#define FIELD_OPAND   2

typedef struct {
  tVarType           _type; // just OPTYPE_STRING, OPTYPE_INT or OPTYPE_DOUBLE
  tStrPool           _name;
  tStrPool           _namelen;
  tStrPool           _valuelen;
  short              _value; // needs to hold a (possible) literal number
} LogoVar;

class LogoWords {

public:

  static void err(Logo &logo);
  static short errArity;
  
  static void ifelse(Logo &logo);
  static short ifelseArity;
  
  static void ifWord(Logo &logo);
  static short ifArity;
  
  static void repeat(Logo &logo);
  static short repeatArity;
  
  static void forever(Logo &logo);
  static short foreverArity;

  static void make(Logo &logo);
  static short makeArity;

  static void wait(Logo &logo);
  static short waitArity;

  static void subtract(Logo &logo);
  static short subtractArity;

  static void add(Logo &logo);
  static short addArity;

  static void divide(Logo &logo);
  static short divideArity;

  static void multiply(Logo &logo);
  static short multiplyArity;

  static void eq(Logo &logo);
  static short eqArity;

  static void neq(Logo &logo);
  static short neqArity;

  static void gt(Logo &logo);
  static short gtArity;

  static void gte(Logo &logo);
  static short gteArity;

  static void lt(Logo &logo);
  static short ltArity;

  static void lte(Logo &logo);
  static short lteArity;

  static void dumpvars(Logo &logo);
  static short dumpvarsArity;

  static void print(Logo &logo);
  static short printArity;

private:

  static bool pushliterals(Logo &logo, short rel);

};

class LogoTimeProvider {

public:

  virtual unsigned long currentms() = 0;
  virtual void delayms(unsigned long ms) = 0;
  virtual bool testing(short ms) = 0;

};

class LogoScheduler {

public:
  LogoScheduler(LogoTimeProvider *provider): _provider(provider), _lasttime(0), _time(0) {}

  virtual void schedule(short ms);
  virtual bool next() ;
    
private:
  LogoTimeProvider *_provider;
  unsigned long _lasttime;
  short _time;

};

class LogoCompiler;
class ArduinoFlashCode;
class LogoString;
class LogoStringResult;

class LogoPrimitives {

public:
  
  virtual short find(LogoString *str, short start, short slen) = 0;
  virtual void name(short op, char *s, int len) = 0;
  virtual short arity(short op) = 0;
  virtual void exec(short index, Logo *logo) = 0;
  virtual void set(LogoString *str, short start, short slen) = 0;
  
};

class LogoFunctionPrimitives: public LogoPrimitives {

public:
  LogoFunctionPrimitives(LogoBuiltinWord *builtins, short size);
  
  // LogoPrimitives
  virtual short find(LogoString *str, short start, short slen);
  virtual void name(short index, char *s, int len);
  virtual short arity(short index);
  virtual void exec(short index, Logo *logo);
  virtual void set(LogoString *str, short start, short slen) {}

private:
  LogoBuiltinWord *_builtins;
  short _count;
};

class Logo {

public:
  Logo(LogoPrimitives *primitives=0, LogoTimeProvider *time=0, LogoBuiltinWord *core=0, LogoString *strings=0, ArduinoFlashCode *code=0);
  ~Logo() {}
  
  // find any errors in the code.
  short geterr();
  bool haserr(short err);
  
  // main execution
  short step();
  short run();
  void restart(); // run from the top, resets the stack
  void reset(); // reset all the code, words and stack and variables
  void resetcode(); // reset all the code, leaves the words and restarrs
  void resetvars(); // reset the variables
  void fail(short err);
  void schedulenext(short delay);
  int callword(const char *word); // call a word by name if you know it.
  short pc() { return _pc; }
  
  // interface to compiler
  bool hascore() { return _core; }
  void error(short error);
  void outofcode();
  void addop(tJump *next, short type, short op=0, short opand=0);
  void findbuiltin(LogoString *str, short start, short slen, short *index, short *category);
  short addstring(LogoString *str, short start, short slen);
  short addstring(LogoStringResult *stri);
  void getstring(LogoStringResult *stri, tStrPool str, tStrPool len) const;
  bool stringcmp(LogoString *str, short start, short slen, tStrPool stri, tStrPool len) const;
  bool stringcmp(LogoStringResult *str, tStrPool stri, tStrPool len) const;
  const LogoBuiltinWord *getbuiltin(short op, short opand) const;
  void getbuiltinname(short op, short opand, char *s, int len) const;
  void setprimitives(LogoString *str, short start, short slen);
  
  // compiler needs direct access to these?
  
  // the code
  tJump _nextcode;
  tJump _nextjcode; // for allocating new jumps
  
  // dealing with the stack
  bool stackempty();
  short popint();
  void pushint(short n);
  double popdouble();
  void splitdouble(double n, short *op, short *opand);
  double joindouble(short op, short opand) const;
  void pushdouble(double n);
  void popstring(LogoStringResult *result);  
  void pushstring(tStrPool n, tStrPool len);  
  bool pop();
  short findvariable(LogoString *str, short start, short slen) const;
  short findvariable(LogoStringResult *str) const;
  short newintvar(short str, short slen, short n);
  void setintvar(short var, short n);
  
  // logo words can be self modifying code but be careful!
  void modifyreturn(short rel, short n);
  bool codeisint(short rel);
  bool codeisstring(short rel);
  short codetoint(short rel);
  void codetostring(short rel, tStrPool *s, tStrPool *len);
  void jumpskip(short rel);
  void jump(short rel);

#ifdef LOGO_DEBUG
  void outstate() const;
#endif
 
#ifndef ARDUINO
  void entab(short indent) const;
  void dumpcode(const LogoCompiler *compiler, bool all=true) const;
  void dumpstack(const LogoCompiler *compiler, bool all=true) const;
  void dumpvars(const LogoCompiler *compiler) const;
  void dumpvars() const;
  void dumpstaticwords(const LogoCompiler *compiler) const;
  int stringslist(LogoCompiler *compiler, char *buf, int len) const;
  int varstringslist(LogoCompiler *compiler, char *buf, int len) const;
  void dumpstringscode(LogoCompiler *compiler, const char *varname, std::ostream &str) const;
  void dumpinst(LogoCompiler *compiler, const char *varname, std::ostream &str) const;
  void optypename(short optype, std::ostream &str) const;
  void printvarstring(const LogoVar &var, std::ostream &str) const;
  void dumpvarsstrings(const LogoCompiler *compiler, std::ostream &str) const;
  void dumpvarscode(const LogoCompiler *compiler, std::ostream &str) const;
  void printvar(const LogoVar &var) const;
  void printvarcode(const LogoVar &var, std::ostream &str) const;
  void dump(short indent, short type, short op, short opand) const;
  void mark(short i, short mark, const char *name) const;
#endif

  static LogoBuiltinWord core[];

private:
//  friend class LogoCompiler;
  
  // the pool of all strings
  LogoString *_fixedstrings;
  short _fixedcount;
  char _strings[STRING_POOL_SIZE];
  tStrPool _nextstring;
  
  // the builtin words.
  LogoPrimitives *_primitives;
  
  // the core words if needed.
  LogoBuiltinWord *_core;
  short _corecount;
  
  // buffer to hold a number conversion
  char _numbuf[NUM_LEN];
   
  // the code
  tLogoInstruction _code[CODE_SIZE];
  tJump _pc;
  ArduinoFlashCode *_staticcode;
  
  // the stack
  tLogoInstruction _stack[MAX_STACK];
  short _tos;
  
  // the variables
  LogoVar _variables[MAX_VARS];
  short _varcount;

  short getvarfromref(short op, short opand);
  
  // the schdeuler for WAIT
  LogoScheduler _schedule;
  
  // parser
  bool parsestring(short type, short op, short oplen, LogoStringResult *str);

  // fixed strings
  short findfixed(LogoString *str, short start, short slen) const;
  bool fixedcmp(LogoString *stri, short strstart, short slen, tStrPool str, tStrPool len) const;
  bool getfixed(LogoStringResult *reuslt, tStrPool str) const;

  // the machine
  bool push(short type, short op=0, short opand=0);
  short parseint(short type, short op, short opand);
  double parsedouble(short type, short op, short opand);
  short doreturn();
  short dobuiltin();
  bool doarity();
  bool doinfix();
  bool call(short jump, tByte opand2);
  short instField(short pc, short field) const;
  
};

#ifdef LOGO_DEBUG
#ifdef ARDUINO
#define DEBUG_DUMP(all)
#define DEBUG_DUMP_MSG(msg, all)
#define DEBUG_STEP_DUMP(count, all)
#else
#define DEBUG_DUMP(all)                                     compiler.dump(all)
#define DEBUG_DUMP_MSG(msg, all)                            compiler.dump(msg, all)
#define DEBUG_STEP_DUMP(count, all)                         BOOST_CHECK_EQUAL(compiler.stepdump(count, all), 0)
#endif // ARDUINO
#else
#define DEBUG_DUMP(all)
#define DEBUG_DUMP_MSG(msg, all)
#define DEBUG_STEP_DUMP(count, all)
#endif

#ifndef ARDUINO
#define PROGMEM
#endif

#endif // H_logo
