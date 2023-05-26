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
    { "OFF", &ledOff ,
  };
  ArduinoTimeProvider time;
  Logo logo(builtins, sizeof(builtins), &time, &Logo::core);
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

// if you don't need variables, save about 1.6k bytes
#define HAS_VARIABLES

// if you don't need IFELSE and all it needs you can save around 1.2k bytes
#define HAS_IFELSE

// about 14.5k with all code, 9.6k bare bones with everything off.
#define STRING_POOL_SIZE    128       // these number of bytes
#define LINE_LEN            40        // these number of bytes
#define WORD_LEN            24        // these number of bytes

// on some Arduinos this could be MUCH larger.
#define MAX_WORDS           20        // 4 bytes each
#define MAX_CODE            100       // 6 bytes each
#define START_JCODE         30        // the start of where the JCODE lies (the words)
#define MAX_STACK           16        // 6 bytes each

// might have to use something like this while you are debugging things on the ardiuno
// itself
// #define STRING_POOL_SIZE  64       // these number of bytes
// #define LINE_LEN          32        // these number of bytes
// #define WORD_LEN          16        // these number of bytes
// #define MAX_WORDS         10        // 4 bytes each
// #define MAX_CODE          80        // 6 bytes each
// #define MAX_STACK         10        // 6 bytes each

#ifdef HAS_VARIABLES
#define MAX_VARS             8         // 6 bytes each
#endif

#include <string.h>
#include <stdio.h>

// success is 0
#define LG_STOP               1
#define LG_UNHANDLED_OP_TYPE  2
#define LG_TOO_MANY_WORDS     3
#define LG_OUT_OF_STRINGS     4
#define LG_LINE_TOO_LONG      5
#define LG_WORD_TOO_LONG      6
#define LG_OUT_OF_CODE        7
#define LG_STACK_OVERFLOW     8
#define LG_WORD_NOT_FOUND     9
#define LG_NO_RETURN_ADDRESS  10
#define LG_TOO_MANY_VARS      11
#define LG_NOT_INT            12
#define LG_NOT_STRING         13
#define LG_NOT_CALLABLE       14
#define LG_EXTRA_IN_DEFINE    15
#define LG_FIXED_NO_NEWLINE   16

#define OPTYPE_NOOP           0 //
#define OPTYPE_RETURN         1 //
#define OPTYPE_HALT           2 //
#define OPTYPE_BUILTIN        3 // _op = index of builtin, _opand = category 0 = builtin, 1 = core 
#define OPTYPE_ERR            4 // _op = error
#define OPTYPE_WORD           5 // _op = where to jump, _opand = possible arity
#define OPTYPE_STRING         6 // _op = index of string (-fixedcount), _opand = length of string
#define OPTYPE_INT            7 // _op = literal integer
#define OPTYPE_DOUBLE         8 // _op = literal integer part of double, _opand is fractional
#ifdef HAS_VARIABLES
#define OPTYPE_REF            9 // _op = index of string (-fixedcount) with a var in it, _opand = length of string
#define OPTYPE_POPREF         10 // _op = index of var to pop into
#endif

// only on the stack
#define SOP_START             100
#define SOPTYPE_ARITY         SOP_START + 1 // _op = the arity of the builtin function
#define SOPTYPE_RETADDR       SOP_START + 2 // _op = the return address. These are just on the stack
#define SOPTYPE_MRETADDR      SOP_START + 3 // _op = the offset to modify by
#ifdef HAS_IFELSE
#define SOPTYPE_CONDRET       SOP_START + 4 // _op = the return address of if true, otherwise _op + 1
#define SOPTYPE_SKIP          SOP_START + 5 // skip the next instruction if on the stack under a return
#endif

class Logo;

typedef void (*tLogoFp)(Logo &logo);

// allow indexes to strings to be chars if the stringpool is small.
#if STRING_POOL_SIZE <= 256
typedef unsigned char tStrPool;
#else
typedef short tStrPool;
#endif

// types are predefinced and small.
typedef unsigned char tType;

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

typedef struct {
  tStrPool  _name;
  tStrPool  _namelen;
  tJump     _jump;
#ifdef HAS_VARIABLES
 tByte      _arity; // smaller than 256?
#endif
} LogoWord;

typedef struct {
  tType     _optype;
  tByte     _unused;
  short     _op;    // needs to hold a (possible) literal number
  short     _opand; // could be holding a repeat count
} LogoInstruction;

#ifdef HAS_VARIABLES

typedef struct {
  tType              _type; // just OPTYPE_STRING, OPTYPE_INT or OPTYPE_DOUBLE
  tStrPool           _name;
  tStrPool           _namelen;
  tStrPool           _valuelen;
  short              _value; // needs to hold a (possible) literal number
} LogoVar;

#endif // HAS_VARIABLES

class LogoWords {

public:

  static void err(Logo &logo);
  static short errArity;
  
#ifdef HAS_IFELSE
  static void ifelse(Logo &logo);
  static short ifelseArity;
#endif
  
  static void repeat(Logo &logo);
  static short repeatArity;
  
  static void forever(Logo &logo);
  static short foreverArity;

#ifdef HAS_VARIABLES
  static void make(Logo &logo);
  static short makeArity;
#endif

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

private:

#ifdef HAS_IFELSE
  static bool pushliterals(Logo &logo, short rel);
#endif

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

// strings are in an abstract class to allow them
// to be placed in FLASH memory if we need to.

class LogoString {

public:
  
  virtual size_t length() const = 0;
  virtual char operator[](int index) const = 0;
  virtual void ncpy(char *to, size_t offset, size_t size) const = 0;
 
};

// just a simple string as a sequence of characters, null terminated.

class LogoSimpleString: public LogoString {

public:
  LogoSimpleString(const char *code): _code(code) {}
  
  size_t length() const { 
    return strlen(_code); 
  }
  
  char operator[](int index) const { 
    return _code[index]; 
  }
  
  void ncpy(char *to, size_t offset, size_t len) const { 
    strncpy(to, _code + offset, len);
  }

private:
  const char *_code;

};

class LogoCompiler;

class Logo {

public:
  Logo(LogoBuiltinWord *builtins, short size, LogoTimeProvider *time, LogoBuiltinWord *core=0, LogoString *strings=0);
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
  
  // interface to compiler
  bool hascore() { return _core; }
  void error(short error);
  void addop(tJump *next, short type, short op=0, short opand=0);
  void findbuiltin(const char *name, short *index, short *category);
  short addstring(const char *s, tStrPool len);
  void getstring(char *buf, short buflen, tStrPool str, tStrPool len) const;
  const LogoBuiltinWord *getbuiltin(short op, short opand) const;

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
  void popstring(char *s, tStrPool len);  
  void pushstring(tStrPool n, tStrPool len);  
  bool pop();
#ifdef HAS_VARIABLES
  short defineintvar(const char *s, short i);
#endif
  
  // logo words can be self modifying code but be careful!
  void modifyreturn(short rel, short n);
#ifdef HAS_IFELSE
  bool codeisint(short rel);
  bool codeisstring(short rel);
  short codetoint(short rel);
  void codetostring(short rel, tStrPool *s, tStrPool *len);
  void jumpskip(short rel);
  void jump(short rel);
  void condreturn(short rel);
#endif

#ifdef LOGO_DEBUG
  void outstate() const;
  void dumpvars(const LogoCompiler *compiler, bool code=false) const;
  void dumpcode(const LogoCompiler *compiler, bool all=true) const;
  void dumpstack(const LogoCompiler *compiler, bool all=true) const;
  void dumpstringscode(LogoCompiler *compiler) const;
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
  LogoBuiltinWord *_builtins;
  short _builtincount;
  
  // the core words if needed.
  LogoBuiltinWord *_core;
  short _corecount;
  
  // various buffers to hold data
  char _findbuf[LINE_LEN];
  char _tmpbuf[LINE_LEN];
   
  // the code
  LogoInstruction _code[MAX_CODE];
  tJump _pc;
  
  // the stack
  LogoInstruction _stack[MAX_STACK];
  short _tos;
  
#ifdef HAS_VARIABLES
  // the variables
  LogoVar _variables[MAX_VARS];
  short _varcount;

  short findvariable(const char *word) const;
  short getvarfromref(const LogoInstruction &entry);
#endif
  
  // the schdeuler for WAIT
  LogoScheduler _schedule;
  
  // parser
  bool parsestring(short type, short op, short oplen, char *s, short len);

  // strings
   short findfixed(const char *s);
   bool getfixed(char *buf, short buflen, tStrPool str) const;

  // builtins
  const LogoBuiltinWord *getbuiltin(const LogoInstruction &entry) const;
  
  // the machine
  bool push(const LogoInstruction &entry);
  short parseint(short type, short op, short opand);
  double parsedouble(short type, short op, short opand);
  short doreturn();
  short dobuiltin();
  short doarity();
  bool call(short jump, tByte opand2);
  
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
