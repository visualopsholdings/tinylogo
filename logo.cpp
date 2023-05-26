/*
  logo.cpp
    
  Author: Paul Hamilton (paul@visualops.com)
  Date: 5-May-2023
  
  This work is licensed under the Creative Commons Attribution 4.0 International License. 
  To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/ or 
  send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  https://github.com/visualopsholdings/tinylogo
*/

#include "logo.hpp"
#include "logocompiler.hpp"

#ifdef LOGO_DEBUG
#ifdef ARDUINO
#include "arduinodebug.hpp"
#else
#include "test/debug.hpp"
void Logo::outstate() const {
//  cout << " _inword " << _inword << endl;
//  cout << " _pc " << _pc << endl;
  cout << endl;
}
#endif // ARDUINO
#define DELAY_TIME  100
#else
#include "nodebug.hpp"
#define DELAY_TIME  5 // must be non zero!
#endif

#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

// all the core words are here. To make it smaller you can comment them
// in and out here.
LogoBuiltinWord Logo::core[] = {
  { "ERR", LogoWords::err, LogoWords::errArity },
#ifdef HAS_VARIABLES
  { "MAKE", LogoWords::make, LogoWords::makeArity },
#endif
  { "FOREVER", LogoWords::forever, LogoWords::foreverArity },
  { "REPEAT", LogoWords::repeat, LogoWords::repeatArity },
#ifdef HAS_IFELSE
  { "IFELSE", LogoWords::ifelse, LogoWords::ifelseArity },
#endif
  { "WAIT", LogoWords::wait, LogoWords::waitArity },
  { "=", LogoWords::eq, LogoWords::eqArity },
  { "-", LogoWords::subtract, LogoWords::subtractArity },
  { "+", LogoWords::add, LogoWords::addArity },
  { "/", LogoWords::divide, LogoWords::divideArity },
  { "*", LogoWords::multiply, LogoWords::multiplyArity },
};

Logo::Logo(LogoBuiltinWord *builtins, short size, LogoTimeProvider *time, LogoBuiltinWord *core, LogoString *strings) : 
  _nextcode(0), 
  _builtins(builtins), 
  _core(core), 
  _nextstring(0), _fixedstrings(strings), _fixedcount(0), 
  _pc(0), _tos(0), _schedule(time) {
  
#ifdef HAS_VARIABLES
  _varcount = 0;
#endif

  _builtincount = size / sizeof(LogoBuiltinWord);
  _corecount = _core ? sizeof(Logo::core) / sizeof(LogoBuiltinWord) : 0;
  _nextjcode = START_JCODE;

  reset();
    
  // count the number of fixed strings.
  if (_fixedstrings) {
    short len = _fixedstrings->length();
    
    // make sure it ends with a newline. simplifies a lot of code :-)
    if ((*_fixedstrings)[len-1] != '\n') {
      error(LG_FIXED_NO_NEWLINE);
      return;
    }
    
    // count the newlines
    for (int i=0; i<len; i++) {
      if ((*_fixedstrings)[i] == '\n') {
        _fixedcount++;
      }
    }
  }
  
}

short Logo::run() {
  
  DEBUG_IN(Logo, "run");
  
  short err = 0;
  do {
    err = step();
  }
  while (!err);
  
  if (err == LG_STOP) {
    err = 0;
  }
  
  return err;
}

void Logo::restart() {

  DEBUG_IN(Logo, "restart");
  
  _pc = 0;
  _tos = 0;
  for (short i=0; i<MAX_STACK; i++) {
    _stack[i]._optype = OPTYPE_NOOP;
  }
  
}

void Logo::reset() {

  DEBUG_IN(Logo, "reset");
  
  // ALL the code
  for (short i=0; i<MAX_CODE; i++) {
    _code[i]._optype = OPTYPE_NOOP;
  }
  _code[START_JCODE-1]._optype = OPTYPE_HALT;
  _nextstring = 0;
  _nextcode = 0;
  _nextjcode = START_JCODE;
  
  resetvars();
  restart();
  
}

void Logo::resetvars() {

  DEBUG_IN(Logo, "resetvars");
  
#ifdef HAS_VARIABLES
  _varcount = 0;
#endif

}

void Logo::resetcode() {

  DEBUG_IN(Logo, "resetcode");
  
  // Just the code up to the words.
  for (short i=0; i<START_JCODE-1; i++) {
    _code[i]._optype = OPTYPE_NOOP;
  }
  _nextcode = 0;
 
  restart();
  
}

short Logo::parseint(short type, short op, short opand) {

  switch (type) {
  
  case OPTYPE_STRING:
    getstring(_tmpbuf, sizeof(_tmpbuf), op, opand);
    return atoi(_tmpbuf);
    
  case OPTYPE_INT:
  case OPTYPE_DOUBLE:
    return op;
    
  default:
    break;
    
  }
  
  return -1;
  
}

double Logo::parsedouble(short type, short op, short opand) {

  switch (type) {
  
  case OPTYPE_STRING:
    getstring(_tmpbuf, sizeof(_tmpbuf), op, opand);
    return atof(_tmpbuf);
    
  case OPTYPE_INT:
    return (double)op;
    
  case OPTYPE_DOUBLE:
    return joindouble(op, opand);
    
  default:
    break;
    
  }
  
  return -1;
  
}

bool Logo::stackempty() {
  return _tos == 0;
}

#ifdef HAS_VARIABLES
short Logo::getvarfromref(const LogoInstruction &entry) {

  DEBUG_IN(Logo, "getvarfromref");
  
  getstring(_tmpbuf, sizeof(_tmpbuf), entry._op, entry._opand);
  return findvariable(_tmpbuf);

}
#endif

short Logo::popint() {

  DEBUG_IN(Logo, "popint");
  
  if (!pop()) {
    error(LG_STACK_OVERFLOW);
    return 0;
  }
  short i = parseint(_stack[_tos]._optype, _stack[_tos]._op, _stack[_tos]._opand);
  if (i >= 0) {
    return i;
  }
#ifdef HAS_VARIABLES
  if (_stack[_tos]._optype == OPTYPE_REF) {
    short var = getvarfromref(_stack[_tos]);
    if (var >= 0) {
      i = parseint(_variables[var]._type, _variables[var]._value, _variables[var]._valuelen);
      if (i >= 0) {
        return i;
      }
    }
  }
#endif  
  return 0;
}

double Logo::popdouble() {

  DEBUG_IN(Logo, "popdouble");
  
  if (!pop()) {
    error(LG_STACK_OVERFLOW);
    return 0;
  }
  double i = parsedouble(_stack[_tos]._optype, _stack[_tos]._op, _stack[_tos]._opand);
  if (i >= 0) {
    return i;
  }
#ifdef HAS_VARIABLES
  if (_stack[_tos]._optype == OPTYPE_REF) {
    short var = getvarfromref(_stack[_tos]);
    if (var >= 0) {
      i = parsedouble(_variables[var]._type, _variables[var]._value, _variables[var]._valuelen);
      if (i >= 0) {
        return i;
      }
    }
  }
#endif  
  return 0;
}

void Logo::pushint(short n) {

  DEBUG_IN(Logo, "pushint");
  
  if (_tos >= MAX_STACK) {
    error(LG_STACK_OVERFLOW);
    return;
  }
  
  _stack[_tos]._optype = OPTYPE_INT;
  _stack[_tos]._op = n;
  _stack[_tos]._opand = 0;
  _tos++;

}

void Logo::splitdouble(double n, short *op, short *opand) {
  double i, f;
  f = modf(n, &i);
  *op = (short)i;
  *opand = (short)(f * 10000);
}

double Logo::joindouble(short op, short opand) const {
  return (double)op + ((double)opand)/((double)10000);
}

void Logo::pushdouble(double n) {

  DEBUG_IN_ARGS(Logo, "pushdouble", "%f", n);
  
  if (_tos >= MAX_STACK) {
    error(LG_STACK_OVERFLOW);
    return;
  }
  
  _stack[_tos]._optype = OPTYPE_DOUBLE;
  short op, opand;
  splitdouble(n, &(_stack[_tos]._op), &(_stack[_tos]._opand));
  _tos++;

}
void Logo::pushstring(tStrPool n, tStrPool len) {

  DEBUG_IN(Logo, "pushstring");
  
  if (_tos >= MAX_STACK) {
    error(LG_STACK_OVERFLOW);
    return;
  }
  
  _stack[_tos]._optype = OPTYPE_STRING;
  _stack[_tos]._op = n;
  _stack[_tos]._opand = len;
  _tos++;

}

void Logo::fail(short err) {
  
  DEBUG_IN_ARGS(Logo, "fail", "%i", err);
  
  _pc = 0;
  _code[0]._optype = OPTYPE_ERR;
  _code[0]._op = err;
  _code[0]._opand = 0;
  
}

void Logo::modifyreturn(short rel, short count) {

  DEBUG_IN_ARGS(Logo, "modifyreturn", "%i%i", rel, _tos);
  
  if (_tos >= MAX_STACK) {
    fail(LG_STACK_OVERFLOW);
    return;
  }

  // push the return adddres
  _stack[_tos]._optype = SOPTYPE_MRETADDR;
  _stack[_tos]._op = rel;
  _stack[_tos]._opand = count;
  _tos++;
  
}

bool Logo::parsestring(short type, short op, short opand, char *s, short len) {

  switch (type) {
  
  case OPTYPE_STRING:
    getstring(s, len, op, opand);
    return true;
    
  case OPTYPE_INT:
    snprintf(s, len, "%d", op);
    return true;
    
  case OPTYPE_DOUBLE:
    snprintf(s, len, "%f", joindouble(op, opand));
    return true;
    
  default:
    break;
    
  }
  
  return false;
  
}

bool Logo::pop() {

  DEBUG_IN(Logo, "pop");
  
  if (_tos <= 0) {
    return false;
  }
  _tos--;
  return true;
  
}

void Logo::popstring(char *s, tStrPool len) {

  DEBUG_IN(Logo, "popstring");
  
  if (!pop()) {
    error(LG_STACK_OVERFLOW);
    return;
  }
  if (!parsestring(_stack[_tos]._optype, _stack[_tos]._op, _stack[_tos]._opand, s, len)) {
#ifdef HAS_VARIABLES
    short var = getvarfromref(_stack[_tos]);
    if (var >= 0) {
      if (!parsestring(_variables[var]._type, _variables[var]._value, _variables[var]._valuelen, s, len))
#endif
        *s = 0;
#ifdef HAS_VARIABLES
    }
#endif  
  }
}

bool Logo::push(const LogoInstruction &entry) {

  DEBUG_IN_ARGS(Logo, "push", "%i%i", entry._optype, entry._op);
  
  if (_tos >= MAX_STACK) {
    return false;
  }
  
  _stack[_tos++] = entry;
  
  return true;
  
}

#ifdef HAS_IFELSE
bool Logo::codeisint(short rel) {

  DEBUG_IN_ARGS(Logo, "codeisint", "%i", rel);
  
  bool val;
#ifdef HAS_VARIABLES
  if (_code[_pc+rel]._optype == OPTYPE_REF) {
    short var = getvarfromref(_code[_pc+rel]);
    if (var >= 0) {
      val = _variables[var]._type == OPTYPE_INT;
    }
    else {
      // it's missing, say it's a num.
      val = true;
    }
    DEBUG_RETURN(" ref %b", val);
    return val;
  }
#endif 
 
  val = _code[_pc+rel]._optype == OPTYPE_INT;
  DEBUG_RETURN(" num %b", val);
  return val;
  
}

short Logo::codetoint(short rel) {

  DEBUG_IN_ARGS(Logo, "codetoint", "%i", rel);
  
  short val;
#ifdef HAS_VARIABLES
  if (_code[_pc+rel]._optype == OPTYPE_REF) {
    short var = getvarfromref(_code[_pc+rel]);
    if (var >= 0) {
      if (_variables[var]._type != OPTYPE_INT) {
        error(LG_NOT_INT);
        return 0;
      }
      val = _variables[var]._value;
    }
    else {
      // a mssing var just report as 0
      val = 0;
    }
    DEBUG_RETURN(" ref %i", val);
    return val;
  }
#endif
  
  if (_code[_pc+rel]._optype != OPTYPE_INT) {
    error(LG_NOT_INT);
    return 0;
  }

  val = _code[_pc+rel]._op;
  DEBUG_RETURN(" num %i", val);
  return val;

}

bool Logo::codeisstring(short rel) {

  return _code[_pc+rel]._optype == OPTYPE_STRING;

}

void Logo::codetostring(short rel, tStrPool *s, tStrPool *len) {

  if (!codeisstring(rel)) {
    error(LG_NOT_STRING);
    return;
  }
  *s = _code[_pc+rel]._op;
  *len = _code[_pc+rel]._opand;
   
}

void Logo::jumpskip(short rel) {

  DEBUG_IN_ARGS(Logo, "jumpskip", "%i", rel);
  
  if (_tos >= MAX_STACK) {
    error(LG_STACK_OVERFLOW);
    return;
  }
  
  jump(rel);
  
  _stack[_tos]._optype = SOPTYPE_SKIP;
  _stack[_tos]._op = 0;
  _tos++;
  
}

void Logo::jump(short rel) {

  _pc += rel - 1;
  
}

void Logo::condreturn(short rel) {

  DEBUG_IN_ARGS(Logo, "condreturn", "%i", rel);
  
  if (_tos >= MAX_STACK) {
    error(LG_STACK_OVERFLOW);
    return;
  }
  
  // push the return adddres
  _stack[_tos]._optype = SOPTYPE_CONDRET;
  _stack[_tos]._op = _pc + rel;
  _stack[_tos]._opand = 0;
  _tos++;
  
}
#endif // HAS_IFELSE

bool Logo::call(short jump, tByte arity) {

  DEBUG_IN(Logo, "call");
  
  if (jump == NO_JUMP) {
    DEBUG_RETURN(" word ignored", 0);
    return true;
  }
  
  if (_tos >= MAX_STACK) {
    return false;
  }
  
#ifdef HAS_VARIABLES
  if (arity) {
    _stack[_tos]._optype = SOPTYPE_ARITY;
    _stack[_tos]._op = _pc;
    _stack[_tos]._opand = arity;
    _tos++;
    
    DEBUG_RETURN(" word has arity", 0);
    return true;
  }
#endif

  // push the return adddres
  _stack[_tos]._optype = SOPTYPE_RETADDR;
  _stack[_tos]._op = _pc + 1;
  _stack[_tos]._opand = 0;
  _tos++;
  
  // and go.
  _pc = jump - 1;

  return true;
  
}

void Logo::schedulenext(short delay) {
  _schedule.schedule(delay); 
}

short Logo::step() {

//  DEBUG_IN(Logo, "step");
  
  if (!_schedule.next()) {
    return 0;
  }
  short err = doarity();
  if (err) {
    return err;
  }
  
  // just in case arity ACTUALLY called a builtin.
  if (!_schedule.next()) {
    return 0;
  }
  
  // make sure stack ops don't make it onto here.
  if (_code[_pc]._optype >= SOP_START) {
    return LG_UNHANDLED_OP_TYPE;
  }
  switch (_code[_pc]._optype) {
  
  case OPTYPE_HALT:
    return LG_STOP;
    
  case OPTYPE_RETURN:
    return doreturn();
    
  case OPTYPE_NOOP:
    break;
    
  case OPTYPE_BUILTIN:
    err = dobuiltin();
    break;
    
  case OPTYPE_STRING:
  case OPTYPE_INT:
  case OPTYPE_DOUBLE:
    // push anything we find onto the stack.
    if (!push(_code[_pc])) {
      err = LG_STACK_OVERFLOW;
    }
    break;
    
#ifdef HAS_VARIABLES
  case OPTYPE_REF:
    {
      short var = getvarfromref(_code[_pc]);
      LogoInstruction inst;
      if (var >= 0) {
        inst._optype = _variables[var]._type;
        inst._op = _variables[var]._value;
        inst._opand = _variables[var]._valuelen;
      }
      else {
        // just push qa zero
        inst._optype = OPTYPE_INT;
        inst._op = 0;
        inst._opand = 0;
      }
      if (!push(inst)) {
        err = LG_STACK_OVERFLOW;
      }
    }
    break;
    
  case OPTYPE_POPREF:
  
    // pop the value from the stack
    if (!pop()) {
      err = LG_STACK_OVERFLOW;
      break;
    }
    if (_stack[_tos]._optype != OPTYPE_INT) {
      err = LG_NOT_INT;
      break;
    }
    
    // and set the variable to that value.
    _variables[_code[_pc]._op]._value = _stack[_tos]._op;
    break;
#endif
    
  case OPTYPE_WORD:
    if (!call(_code[_pc]._op, _code[_pc]._opand)) {
      err = LG_STACK_OVERFLOW;
    }
    break;
    
  case OPTYPE_ERR:
    err = _code[_pc]._op;
    break;
    
  default:
    err = LG_UNHANDLED_OP_TYPE;
  }

  _pc++;
  
  return err;
  
}

short Logo::doarity() {

  DEBUG_IN(Logo, "doarity");

  short ar = _tos-1;
  while (ar >= 0 && _stack[ar]._optype != SOPTYPE_RETADDR && _stack[ar]._optype != SOPTYPE_ARITY) {
    ar--;
  }
  if (ar < 0) {
    DEBUG_RETURN(" no arity", 0);
    return 0;
  }
  
  if (_stack[ar]._optype == SOPTYPE_RETADDR) {
    DEBUG_RETURN(" found return address before arity", 0);
    return 0;
  }
  
  if (_stack[ar]._opand > 0) {
    _stack[ar]._opand--;
    DEBUG_RETURN(" going again", 0);
    return 0;
  }

  DEBUG_OUT("finished", 0);

  short pc = _stack[ar]._op;
  if (_code[pc]._optype == OPTYPE_BUILTIN) {
  
    memmove(_stack + ar, _stack + ar + 1, (_tos - ar + 1) * sizeof(LogoInstruction));
    _tos--;

    DEBUG_OUT("calling builtin %i", _code[pc]._op);
    getbuiltin(_code[pc])->_code(*this);

    return 0;
  }
  
#ifdef HAS_VARIABLES
  if (_code[pc]._optype == OPTYPE_WORD) {
  
    DEBUG_OUT("calling word %i", _code[pc]._op);
    
    // replace the arity entry with the return address
    _stack[ar]._optype = SOPTYPE_RETADDR;
    _stack[ar]._op = pc + _code[pc]._opand + 1;
    _stack[ar]._opand = 0;
    
    // and go.
    _pc = _code[pc]._op;
    
    return 0;
  }
#endif

  DEBUG_RETURN(" not a builtin or word", 0);
  return LG_NOT_CALLABLE;

}

short Logo::dobuiltin() {

  DEBUG_IN(Logo, "dobuiltin");

  short err = 0;
  
  // handle arity by pushing arguments onto the stack
  
  short call = _pc;
  const LogoBuiltinWord *builtin = getbuiltin(_code[call]);
  short arity = builtin->_arity;
  
  if (arity == 0) {
    builtin->_code(*this);
    return 0;
  }
  
  if (_tos >= MAX_STACK) {
    return LG_STACK_OVERFLOW;
  }
  
  // push the arity onto the stack
  _stack[_tos]._optype = SOPTYPE_ARITY;
  _stack[_tos]._op = call;
  _stack[_tos]._opand = arity;
  _tos++;

  return err;
  
}

short Logo::doreturn() {

  DEBUG_IN(Logo, "doreturn");

  // handle returning
  
  if (!pop()) {
    return LG_STACK_OVERFLOW;
  }

  
  // find the return address on the stack
  short ret = _tos;
  while (ret > 0 && _stack[ret]._optype != SOPTYPE_RETADDR) {
//    dump(3, _stack[ret]); cout << endl;
    ret--;
  }

//   dumpstack(false);
//   dump(3, _stack[_tos]); cout << endl;
//   dump(3, _stack[ret]); cout << endl;
  
  if (ret > 0) {
  
#ifdef HAS_IFELSE
    if (_stack[ret-1]._optype == SOPTYPE_CONDRET) {
  
      if (_stack[ret+1]._optype == OPTYPE_INT && _stack[ret+1]._op) {
        // condition was true
        DEBUG_OUT("condition true", 0);
        _pc = _stack[ret-1]._op;
        _tos = ret;
        _stack[_tos-1]._optype = SOPTYPE_SKIP;
        _stack[_tos-1]._op = 0;
      }
      else {
        // condition is false (or not a number)
        DEBUG_OUT("conditional false", 0);
        _pc = _stack[ret-1]._op + 1;
        _tos = ret-1;
      }
      return 0;
    }
  
    if (_stack[ret-1]._optype == SOPTYPE_SKIP) {
  
      DEBUG_OUT("skipping", 0);
      
       _pc = _stack[ret]._op + 1;

     // shuffle the stack down to remove out skip but leave whatever
      // is there after the return
      memmove(_stack + ret - 1, _stack + ret + 1, (_tos - ret + 1) * sizeof(LogoInstruction));
      _tos--;

      return 0;
    }
#endif // HAS_IFELSE
    
    if (_stack[ret-1]._optype == SOPTYPE_MRETADDR) {

      if (_stack[ret-1]._opand == -1) {
        DEBUG_OUT("forever modify return by %i", _stack[ret-1]._op);
        _pc = _stack[ret]._op + _stack[ret-1]._op;
      }
      else if (_stack[ret-1]._opand > 1) {
        _stack[ret-1]._opand--;
        DEBUG_OUT("modify return by %i", _stack[ret-1]._opand);
        _pc = _stack[ret]._op + _stack[ret-1]._op;
      }
      else {
        DEBUG_OUT("finished mod return", 0);
        _pc = _stack[ret]._op;
        _tos--;
      }
      
      return 0;
    }
  }
  
  // here next
  _pc = _stack[ret]._op;

  // shuffle the stack down so that words can push data
  memmove(_stack + ret, _stack + ret + 1, (_tos - ret) * sizeof(LogoInstruction));
 
  return 0;
    
}

void Logo::addop(tJump *next, short type, short op, short opand) {

  DEBUG_IN_ARGS(Logo, "addop", "%i%i%i", type, op, opand);

  _code[*next]._optype = type;
  _code[*next]._op = op;
  _code[*next]._opand = opand;
  (*next)++;
  
}

void Logo::findbuiltin(const char *name, short *index, short *category) {

  DEBUG_IN_ARGS(Logo, "findbuiltin", "%s", name);
  
  *index = -1;
  *category = 0;
  for (short i=0; i<_builtincount; i++) {
    if (strcmp(_builtins[i]._name, name) == 0) {
      *index = i;
      return;
    }
  }
  
  for (short i=0; i<_corecount; i++) {
    if (strcmp(_core[i]._name, name) == 0) {
      *index = i;
      *category = 1;
      return;
    }
  }
  
}

short Logo::findfixed(const char *s) {

  DEBUG_IN_ARGS(Logo, "findfixed", "%s", s);
  
  if (_fixedstrings) {
    short len = _fixedstrings->length();
    short start = 0;
    short index = 0;
    for (int i=0; i<len; i++) {
      if ((*_fixedstrings)[i] == '\n') {
        _fixedstrings->ncpy(_tmpbuf, start, i-start);
        _tmpbuf[i-start] = 0;
        if (strcmp(_tmpbuf, s) == 0) {
          DEBUG_RETURN(" %i", index);
          return index;
        }
        start = i + 1;
        index++;
      }
    }
  }
  
  DEBUG_RETURN(" %i", -1);
  return -1;
}

bool Logo::fixedcmp(const char *s, short slen, tStrPool str) const {

  if (_fixedstrings) {
    short len = _fixedstrings->length();
    short start = 0;
    short index = 0;
    for (int i=0; i<len; i++) {
      if ((*_fixedstrings)[i] == '\n') {
        if (index == str && _fixedstrings->ncmp(s, start, slen) == 0) {
          return true;
        }
        start = i + 1;
        index++;
      }
    }
  }
  
  return false;
  
}

bool Logo::getfixed(char *buf, short buflen, tStrPool str) const {

  if (_fixedstrings && str < _fixedcount) {
    short start = 0;
    short index = 0;
    short len = _fixedstrings->length();
    for (int i=0; i<len; i++) {
      if ((*_fixedstrings)[i] == '\n') {
        if (index == str) {
          short l = min(buflen, i-start);
          _fixedstrings->ncpy(buf, start, l);
          buf[l] = 0;
          return true;
        }
        start = i + 1;
        index++;
      }
    }
  }
  
  return false;
}

short Logo::addstring(const char *s, tStrPool len) {

  DEBUG_IN_ARGS(Logo, "addstring", "%s%i", s, len);
  
  short fixed = findfixed(s);
  if (fixed >= 0) {
    DEBUG_RETURN(" %i", fixed);
    return fixed;
  }

  if ((_nextstring + len) >= STRING_POOL_SIZE) {
    DEBUG_RETURN(" %i", -1);
    return -1;
  }
  
  short cur = _nextstring;
  memmove(_strings + cur, s, len);
  _nextstring += len;
  
  short str = cur + _fixedcount;
  DEBUG_RETURN(" %i", str);
  return str;
}

bool Logo::stringcmp(const char *s, short slen, tStrPool str, tStrPool len) const {

  if (slen != len) {
    return false;
  }
  
  if (fixedcmp(s, slen, str)) {
    return true;
  }
  
  return strncmp(s, _strings + (str - _fixedcount), len) == 0;
  
}

void Logo::getstring(char *buf, short buflen, tStrPool str, tStrPool len) const {

  if (getfixed(buf, buflen, str)) {
    return;
  }

  short l = min(buflen, len);
  memmove(buf, _strings + (str - _fixedcount), l);
  buf[l] = 0;
  
}

#ifdef HAS_VARIABLES
short Logo::findvariable(const char *word) const {

  DEBUG_IN_ARGS(Logo, "findvariable", "%s%i", word, _varcount);
  
  short len = strlen(word);
  for (short i=0; i<_varcount; i++) {
    if (stringcmp(word, len, _variables[i]._name, _variables[i]._namelen)) {
      return i;
    }
  }
  
  return -1;
}
#endif

short Logo::geterr() {

  DEBUG_IN(Logo, "geterr");
  
  // walk through the code looking for errors.
  for (short i=0; i<MAX_CODE; i++) {
    if (_code[i]._optype == OPTYPE_ERR) {
      DEBUG_RETURN(" code %i", i);
      return _code[i]._op;
    }
  }

  // couldn't find any
  DEBUG_RETURN(" %i", 0);
  return 0;
  
}

bool Logo::haserr(short err) {

  DEBUG_IN_ARGS(Logo, "haserr", "%i", err);
  
  // walk through the code looking for a particular error.
  for (short i=0; i<MAX_CODE; i++) {
    if (_code[i]._optype == OPTYPE_ERR && _code[i]._op == err) {
      return true;
    }
  }

  return false;
}

void LogoCompiler::finishword(short word, short wordlen, short jump, short arity) {

  DEBUG_IN_ARGS(LogoCompiler, "finishword", "%i%i%i%i", word, wordlen, jump, arity);
  
  _logo->addop(&_logo->_nextjcode, OPTYPE_RETURN);
      
  if (_wordcount >= MAX_WORDS) {
    _logo->error(LG_TOO_MANY_WORDS);
    return;
  }

  _words[_wordcount]._name = word;
  _words[_wordcount]._namelen = wordlen;
  _words[_wordcount]._jump = jump;
#ifdef HAS_VARIABLES
  _words[_wordcount]._arity = arity;
#endif
  _wordcount++;

}

void Logo::error(short error) {
  addop(&_nextcode, OPTYPE_ERR, error);
}

bool LogoCompiler::dodefine(const char *word, bool eol) {

  DEBUG_IN_ARGS(LogoCompiler, "dodefine", "%s%b%b%b", word, _inword, _inwordargs, eol);
  
  if (*word == 0) {
    DEBUG_RETURN(" empty word %b", false);
    return false;
  }
  
  if (strcmp(word, "TO") == 0) {
    _inword = true;
    _inwordargs = false;
    _wordarity = 0;
    DEBUG_RETURN(" in define %b", true);
    return true;
  }

  if (word[0] == '[') {
    // all sentences should have been replaced!
    _logo->error(LG_WORD_NOT_FOUND);
    return false;
  }

  if (!_inword) {
    DEBUG_RETURN(" %b", false);
    return false;
  }
  
  // the word we are defining
  if (_inword) {
  
    if (_defining < 0) {
  
      _defininglen = strlen(word);
      _defining = _logo->addstring(word, _defininglen);
      if (_defining < 0) {
        _logo->error(LG_OUT_OF_STRINGS);
        return true;
      }
      if (!eol) {
        _inwordargs = true;
      }
      DEBUG_RETURN(" word started %b", true);
      return true;
    
    }
    else if (strcmp(word, "END") == 0) {
      // the END token
      _inword = false;
      _inwordargs = false;
      finishword(_defining, _defininglen, _jump, _wordarity);
      _wordarity = -1;
      _defining = -1;
      _defininglen = -1;
      _jump = NO_JUMP;
      DEBUG_RETURN(" finished word %b", true);
      return true;
    }
  }
    
  // first word we define in our word, remember where we are.
  if (_jump == NO_JUMP) {
    _jump = _logo->_nextjcode;
  }
  
  if (_logo->_nextjcode >= MAX_CODE) {
    _logo->error(LG_OUT_OF_CODE);
    return true;
  }
  
  if (_inwordargs) {
#ifdef HAS_VARIABLES
    if (word[0] == ':') {
    
      short var = _logo->defineintvar(word+1, 0);
      _logo->addop(&_logo->_nextjcode, OPTYPE_POPREF, var);
      _wordarity++;
      if (eol) {
        _inwordargs = false;
      }
      DEBUG_RETURN(" %b", true);
      return true;
    }
#endif
    _logo->error(LG_EXTRA_IN_DEFINE);
    DEBUG_RETURN(" too many in define %b", true);
    return true;
  }
  
  compileword(&_logo->_nextjcode, word, 0);
  
  DEBUG_RETURN(" %b", true);
  return true;

}

#ifdef HAS_VARIABLES
short Logo::defineintvar(const char *s, short i) {

  DEBUG_IN_ARGS(Logo, "defineintvar", "%s%i", s, i);
  
  short var = findvariable(s);
  if (var < 0) {
    short len = strlen(s);
    short str = addstring(s, len);
    if (str < 0) {
      error(LG_OUT_OF_STRINGS);
      return -1;
    }
    if (_varcount >= MAX_VARS) {
      error(LG_TOO_MANY_VARS);
      return -1;
    }
    var = _varcount;
    _variables[_varcount]._name = str;
    _variables[_varcount]._namelen = len;
    _varcount++;
  }
  
  _variables[var]._type = OPTYPE_INT;
  _variables[var]._value = i;
  _variables[var]._valuelen = 0;
    
  return var;
}
#endif

void LogoScheduler::schedule(short ms) {
  if (!_provider) {
    return;
  }
  if (_provider->testing(ms)) {
    return;
  }
  if (_lasttime == 0) {
    _lasttime = _provider->currentms();
  }
  if (_time == 0) {
    _time = ms;
  }
  else {
    _time += ms;
  }
}

bool LogoScheduler::next() {
  if (!_provider) {
    return true;
  }
  if (_time == 0) {
    _provider->delayms(DELAY_TIME);
    return true;
  }
  unsigned long now = _provider->currentms();
  unsigned long diff = now - _lasttime;
  if (diff > _time) {
    _lasttime = now;
    _time = 0;
    return true;
  }
  _provider->delayms(DELAY_TIME);
  return false;

}

#if defined(LOGO_DEBUG) && !defined(ARDUINO)

void Logo::dumpstringscode(LogoCompiler *compiler) const {

  char name[32];
  for (short i=0; i<_nextcode; i++) {
    switch (_code[i]._optype) {
    
    case OPTYPE_STRING:
      getstring(name, sizeof(name), _code[i]._op, _code[i]._opand);
      cout << "\t\"" << name << "\\n\"" << endl;
      break;
    }
  }
  compiler->dumpwords(true);
  dumpvars(compiler, true);
}

#ifdef HAS_VARIABLES
void Logo::dumpvars(const LogoCompiler *compiler, bool code) const {

  if (!code) {
    cout << "vars: " << endl;
  }
  
  if (!_varcount) {
    if (!code) {
      compiler->entab(1);
      cout << "empty" << endl;
    }
    return;
  }
  
  for (short i=0; i<_varcount; i++) {
    if (!code) {
      compiler->entab(1);
    }
    compiler->printvar(_variables[i], code);
    cout << endl;
  }
  
}
#endif

void Logo::dumpcode(const LogoCompiler *compiler, bool all) const {

  cout << "code: pc (" << (short)_pc << ")" << endl;
  
  if (!_nextcode) {
    compiler->entab(1);
    cout << "empty" << endl;
  }
  
  for (short i=0; i<(all ? MAX_CODE : _nextcode); i++) {
    compiler->markword(i);
    compiler->dump(1, _code[i]);
    compiler->mark(i,  _pc, "pc");
    if (all) {
      compiler->mark(i, START_JCODE, "startjcode");
      compiler->mark(i, _nextjcode, "nextjcode");
    }
    cout << endl;
  }
  
  if (!all) {
    if (_pc >  _nextcode && _pc < START_JCODE) {
      for (short i= _nextcode; i<=_pc; i++) {
        compiler->dump(1, _code[i]);
        compiler->mark(i,  _pc, "pc");
        cout << endl;
      }
    }
    cout << "\t..." << endl;
    for (short i=START_JCODE; i<_nextjcode; i++) {
      compiler->markword(i);
      compiler->dump(1, _code[i]);
      compiler->mark(i,  _pc, "pc");
      compiler->mark(i, START_JCODE, "startjcode");
      cout << endl;
    }
  }
}

void Logo::dumpstack(const LogoCompiler *compiler, bool all) const {

  cout << "stack: (" << _tos << ")" << endl;
  
  for (short i=0; i<(all ? MAX_STACK : _tos); i++) {
    compiler->dump(1,_stack[i]);
    compiler->mark(i, _tos, "tos");
    cout << endl;
  }
  
}

#endif // defined(LOGO_DEBUG) && !defined(ARDUINO)

short LogoWords::errArity = 0;

void LogoWords::err(Logo &logo) {

  logo.fail(LG_STOP);
  
}

#ifdef HAS_VARIABLES
short LogoWords::makeArity = 2;

void LogoWords::make(Logo &logo) {

  short n = logo.popint();
  char s[WORD_LEN];
  logo.popstring(s, sizeof(s));
  logo.defineintvar(s, n);
  
}
#endif

short LogoWords::foreverArity = 0;

void LogoWords::forever(Logo &logo) {

  // change the next return address to be 1 minus what we have 
  logo.modifyreturn(-1, -1);

}

short LogoWords::repeatArity = 1;

void LogoWords::repeat(Logo &logo) {

  short n = logo.popint();

  // change the next return address to be 1 minus what we have 
  logo.modifyreturn(-1, n);

}

#ifdef HAS_IFELSE
bool LogoWords::pushliterals(Logo &logo, short rel) {

  if (logo.codeisint(rel)) {
    logo.pushint(logo.codetoint(rel));
    return true;
  }
  else if (logo.codeisstring(rel)) {
    tStrPool s, l;
    logo.codetostring(rel, &s, &l);
    if (logo.geterr()) {
      return false;
    }
    logo.pushstring(s, l);
    return true;
  }

  return false;
  
}

short LogoWords::ifelseArity = 0;

void LogoWords::ifelse(Logo &logo) {

//  logo.dump("ifelse", false);
  
  if (logo.codeisint(1)) {
  
    // handle numeric literal argument.
    if (logo.codetoint(1)) {  
    
      // true
      if (pushliterals(logo, 2)) {
        logo.jump(4);
      }
      else {
        logo.jumpskip(2);
      }
    }
    else {
    
      // false
      if (pushliterals(logo, 3)) {
        logo.jump(4);
      }
      else {
        logo.jump(3);
      }
      
    }
    
    return;
  }
  
  // this is a very special trick to allow us to manipulate the PC
  // based on the value of the stack.
  //
  // our code looks like:
	//  builtin IFELSE		(pc)
	//  TEST
	//  THEN
	//  ELSE
	//
  // we push a "condreturn" onto the stack telling
  // use to go to "2"
  //
  // then when we hit a return for the TEST
  //
  // our stack looks like:
  //
  //  cond return 2 (relative to the )
  //  ret 3 (or whatever the address of THEN is)
  //  num 1
  //
  //  when we return we look back and find the cond return and 
  //  then pop the top of the stack and test if it and
  // if true we go to THEN, otherwise we go to ELSE
  //
  //  Then we clean the stack up
  
//  logo.dump(false);
  logo.condreturn(2);
//  logo.dumpstack(false);
  
}

#endif

short LogoWords::waitArity = 1;

void LogoWords::wait(Logo &logo) {

  // schedule the next word to execute after a certain time.
  logo.schedulenext(logo.popint());
  
}

short LogoWords::eqArity = 1;

void LogoWords::eq(Logo &logo) {

  logo.pushint(logo.popint() == logo.popint());
  
}

short LogoWords::subtractArity = 1;

void LogoWords::subtract(Logo &logo) {

  double a1 = logo.popdouble();
  double a2 = logo.popdouble();
  logo.pushdouble(a2 - a1);
  
}

short LogoWords::addArity = 1;

void LogoWords::add(Logo &logo) {

  double a1 = logo.popdouble();
  double a2 = logo.popdouble();
  logo.pushdouble(a2 + a1);
  
}

short LogoWords::divideArity = 1;

void LogoWords::divide(Logo &logo) {

  double a1 = logo.popdouble();
  double a2 = logo.popdouble();
  logo.pushdouble(a2 / a1);
  
}

short LogoWords::multiplyArity = 1;

void LogoWords::multiply(Logo &logo) {

  double a1 = logo.popdouble();
  double a2 = logo.popdouble();
  logo.pushdouble(a2 * a1);
  
}

