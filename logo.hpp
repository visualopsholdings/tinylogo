/*
  logo.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 5-May-2023
  
  Tiniest Logo Intepreter
  
  Keep feeding it commands and it will execute them.
  
  Use it like this:
  
  ArduinoTimeProvider time;
  Logo logo(&time);
  LogoCompiler compiler(&logo);
  
  compiler.compiler("to ON; dhigh 13; end;");
  compiler.compiler("to OFF; dlow 13; end;");
  compiler.compiler("to FLASH; ON wait 100 OFF wait 100; end;");
  compiler.compiler("to GO; forever FLASH; end;");

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
    
  This work is licensed under the Creative Commons Attribution 4.0 International License. 
  To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/ or 
  send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  https://github.com/visualopsholdings/tinylogo
*/

#ifndef H_logo
#define H_logo

#include "arduinoflashstring.hpp"
#include "list.hpp"

#define MACHINE_VERSION   1

//#define LOGO_DEBUG

// compiling with the WIFI code increases the size dramatically. IF you don't use it
// comment this out.
#define USE_WIFI

#ifdef ARDUINO

#ifdef ESP32
#define USE_LARGE_CODE
#else
// un comment this if you want to use the logo runtime directly with flash code. This allows
// for MUCH greater stack and vars.
// this isn't used when USE_LARGE_CODE is on
#define USE_FLASH_CODE
#endif

#endif

// it's really important with these numbers that after you compile your code it 
// leaves about 200 bytes for local variables. Otherwise your sketch won't work.
// 

#ifdef USE_LARGE_CODE
// Huge settings for something that can handle it.
#define STRING_POOL_SIZE    1024 * 4
#define MAX_CODE            1024 * 4        
#define CODE_SIZE           MAX_CODE
#define START_JCODE         1024 * 3
#define MAX_STACK           1024 * 2
#define MAX_VARS            1024 * 2

#else

#ifdef USE_FLASH_CODE
#if defined(ARDUINO) && defined(__AVR__)
// when we are allocating strings on the ARDUINO, this will be too small but
// for now they area all static.
#define STRING_POOL_SIZE    64
#else
#define STRING_POOL_SIZE    512       // these number of bytes
#endif
#else
#define STRING_POOL_SIZE    128       // these number of bytes
#endif

#ifdef USE_FLASH_CODE
#define MAX_CODE            512       // 6 bytes each
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
#if defined(ARDUINO) && defined(__AVR__)
#define CODE_SIZE           2
#else
#define CODE_SIZE           MAX_CODE
#endif
#else
#define CODE_SIZE           MAX_CODE  
#endif

#endif // USE_LARGE_CODE

#define NUM_LEN             12        // these number of bytes
#define SENTENCE_LEN        4         // & and 3 more digits
#define STRING_LEN          32        // the length of a single string.

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
#define LG_FIXED_NO_NEWLINE   13
#define LG_ARITY_NOT_IMPL     14

#define OPTYPE_NOOP           0 //
#define OPTYPE_RETURN         1 //
#define OPTYPE_HALT           2 //
#define OPTYPE_BUILTIN        3 // FIELD_OP = index of builtin
#define OPTYPE_ERR            4 // FIELD_OP = error
#define OPTYPE_JUMP           5 // FIELD_OP = where to jump, FIELD_OPAND = possible arity
#define OPTYPE_STRING         6 // FIELD_OP = index of string (-fixedcount), FIELD_OPAND = length of string
#define OPTYPE_INT            7 // FIELD_OP = literal integer
#define OPTYPE_DOUBLE         8 // FIELD_OP = literal integer part of double, FIELD_OPAND is fractional
#define OPTYPE_REF            9 // FIELD_OP = index of string (-fixedcount) with a var in it, FIELD_OPAND = length of string
#define OPTYPE_POPREF         10 // FIELD_OP = index of var to pop into
#define OPTYPE_GSTART         11 // (
#define OPTYPE_GEND           12 // )
#define OPTYPE_LSTART         13 // [
#define OPTYPE_LEND           14 // ]
#define OPTYPE_LIST           15 // FIELD_OP = the head of the list, FIELD_OPAND = tail

// only on the stack
#define SOP_START             100
#define SOPTYPE_ARITY         SOP_START + 1 // FIELD_OP = the arity of the builtin function
#define SOPTYPE_RETADDR       SOP_START + 2 // FIELD_OP = the return address. These are just on the stack
#define SOPTYPE_MRETADDR      SOP_START + 3 // FIELD_OP = the offset to modify by
#define SOPTYPE_SKIP          SOP_START + 4 // skip the next instruction if on the stack under a return
#define SOPTYPE_GSTART        SOP_START + 5 // we are grouping on the stack.
#define SOPTYPE_OPENLIST      SOP_START + 6 // FIELD_OP = the head of the list, FIELD_OPAND = tail

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

#if defined(ARDUINO) && !defined(__AVR__)
// this makes it work on the ESP32
#define INFIX_ARITY   255
#else
#define INFIX_ARITY   -1
#endif

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
  short              _value; // needs to hold a (possible) literal number
  short              _valueopand;
} LogoVar;

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

class Logo {

public:
  Logo(LogoTimeProvider *time=0, LogoString *strings=0, ArduinoFlashCode *code=0);
  ~Logo();
  
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
  void halt();
  short pc() { return _pc; }
  void backup() { if (_pc > 0) _pc--; }
  bool call(short jump, tByte opand2);
  
  // interface to compiler
  void error(short error);
  void outofcode();
  void addop(tJump *next, short type, short op=0, short opand=0);
  short findbuiltin(LogoString *str, short start, short slen);
  short addstring(const LogoString *str, short start, short slen);
  short addstring(LogoStringResult *stri);
  void getstring(LogoStringResult *stri, tStrPool str, tStrPool len) const;
  bool stringcmp(const LogoString *str, short start, short slen, tStrPool stri, tStrPool len) const;
  bool stringcmp(LogoStringResult *str, tStrPool stri, tStrPool len) const;
  void getbuiltinname(short op, char *s, int len) const;
  void splitdouble(double n, short *op, short *opand);
  
  // compiler needs direct access to these?
  
  // the code
  tJump _nextcode;
  tJump _nextjcode; // for allocating new jumps
  
  // dealing with the stack
  bool stackempty();
  short popint();
  void pushint(short n);
  double popdouble();
  void pushdouble(double n);
  void popstring(LogoStringResult *result);  
  void pushstring(tStrPool n, tStrPool len);  
  void pushstring(LogoString *str);
  void pushlist(const List &list);
  bool isstacklist();
  bool isstackstring();
  bool isstackint();
  bool isstackdouble();
  List newlist();
  List poplist();
  bool pop();
  short findvariable(LogoString *str, short start, short slen) const;
  short findvariable(LogoStringResult *str) const;
  short newintvar(short str, short slen, short n);
  void setintvar(short var, short n);
  bool varisint(short var);
  short varintvalue(short var);
  bool isnum(LogoString *str, short wordstart, short wordlen);
  bool getlistval(const ListNodeVal &val, LogoStringResult *str);
  
  // logo words can be self modifying code but be careful!
  
  // used in FOREVER and REPEAT
  void modifyreturn(short rel, short n); 
  
  // used in IF and IFELSE
  bool codeisint(short rel);
  bool codeisstring(short rel);
  short codetoint(short rel);
  void codetostring(short rel, tStrPool *s, tStrPool *len);
  void jumpskip(short rel);
  void jump(short rel);

  // so we can test.
  short findfixed(const LogoString *str, short start, short slen) const;
  
  // allow to be abstracted.
  void callbuiltin(short index);

#ifdef LOGO_DEBUG
  void outstate() const;
#endif
 
#ifndef ARDUINO
  void entab(short indent) const;
  void dumpcode(const LogoCompiler *compiler, bool all=true) const;
  void dumpvars(const LogoCompiler *compiler) const;
  void dumpvars() const;
  void dumpstaticwords(const LogoCompiler *compiler) const;
  bool safestringcat(short op, short opand, char *buf, int len) const;
  int stringslist(LogoCompiler *compiler, char *buf, int len) const;
  int varstringslist(LogoCompiler *compiler, char *buf, int len) const;
  void dumpstringscode(LogoCompiler *compiler, const char *varname, std::ostream &str) const;
  void dumpinst(LogoCompiler *compiler, const char *varname, std::ostream &str) const;
  void dumpinstline(short type, short op, short opand, int offset, int line, std::ostream &str) const;
  void optypename(short optype, std::ostream &str) const;
  void printvarstring(const LogoVar &var, std::ostream &str) const;
  void dumpvarsstrings(const LogoCompiler *compiler, std::ostream &str) const;
  void dumpvarscode(const LogoCompiler *compiler, std::ostream &str) const;
  void printvar(const LogoVar &var) const;
  void printvarcode(const LogoVar &var, std::ostream &str) const;
  void dump(short indent, short type, short op, short opand) const;
  void mark(short i, short mark, const char *name) const;
  std::ostream &out();
  void setout(std::ostream *s);
  std::ostream *_ostream;
#endif

  void dumpstack(const LogoCompiler *compiler, bool all=true) const;

  static const char coreNames[];
  static const char coreArity[];
  
  static short findcrstring(const LogoString *strings, const LogoString *stri, short strstart, short slen);
  static bool getfixedcr(const LogoString *strings, LogoStringResult *result, short index);

  // manage the channel map.
  void setpin(tByte channel, tByte pin) {
    _channels[channel] = pin;
  }
  tByte getpin(tByte channel) {
    return _channels[channel];
  }
  
private:
  
  // the pool of all strings
  LogoString *_fixedstrings;
  short _fixedcount;
  char _strings[STRING_POOL_SIZE];
  tStrPool _nextstring;
  
  // the core names iand arity.
  ArduinoFlashString _corenames;
  ArduinoFlashString _corearity;
  
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

  // Lists
  ListPool _lists;
  
  short getvarfromref(short op, short opand);
  
  // the schdeuler for WAIT
  LogoScheduler _schedule;
  
  // A channel map for leds
  tByte _channels[8];
  
  // parser
  bool parsestring(short type, short op, short oplen, LogoStringResult *str);

  // fixed strings
  bool fixedcmp(const LogoString *stri, short strstart, short slen, tStrPool str, tStrPool len) const;
  bool getfixed(LogoStringResult *reuslt, tStrPool str) const;

  // the machine
  bool push(short type, short op=0, short opand=0);
  short parseint(short type, short op, short opand);
  double parsedouble(short type, short op, short opand);
  short doreturn();
  short dobuiltin();
  bool doarity();
  bool doinfix();
  short startgroup();
  short endgroup();
  short startlist();
  short endlist();
  short instField(short pc, short field) const;
  double joindouble(short op, short opand) const;
  short pushvalue(short type, short op, short opand);
   
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

#if !(defined(ARDUINO) && defined(__AVR__))
#define PROGMEM
#endif

#endif // H_logo
