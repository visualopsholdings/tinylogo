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
#include "arduinoflashcode.hpp"

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
#define DELAY_TIME  2 // must be non zero!
#endif

#ifdef ARDUINO
#include <HardwareSerial.h>
#else
#include <iostream>
#include <sstream>
using namespace std;
#endif

#include <stdlib.h>
#include <ctype.h>
#include <math.h>

// all the core words are here. To make it smaller you can comment them
// in and out here.
LogoBuiltinWord Logo::core[] = {
  { "ERR", LogoWords::err, LogoWords::errArity },
  { "MAKE", LogoWords::make, LogoWords::makeArity },
  { "FOREVER", LogoWords::forever, LogoWords::foreverArity },
  { "REPEAT", LogoWords::repeat, LogoWords::repeatArity },
  { "IFELSE", LogoWords::ifelse, LogoWords::ifelseArity },
  { "IF", LogoWords::ifWord, LogoWords::ifArity },
  { "WAIT", LogoWords::wait, LogoWords::waitArity },
  { "=", LogoWords::eq, LogoWords::eqArity },
  { "!=", LogoWords::neq, LogoWords::neqArity },
  { "-", LogoWords::subtract, LogoWords::subtractArity },
  { "+", LogoWords::add, LogoWords::addArity },
  { "/", LogoWords::divide, LogoWords::divideArity },
  { "*", LogoWords::multiply, LogoWords::multiplyArity },
  { ">", LogoWords::gt, LogoWords::gtArity },
  { ">=", LogoWords::gte, LogoWords::gteArity },
  { "<", LogoWords::lt, LogoWords::ltArity },
  { "<=", LogoWords::lte, LogoWords::lteArity },
  { "DUMPVARS", LogoWords::dumpvars, LogoWords::dumpvarsArity },
  { "PRINT", LogoWords::print, LogoWords::printArity },
};

LogoFunctionPrimitives::LogoFunctionPrimitives(LogoBuiltinWord *builtins, short size) : _builtins(builtins) {

  _count = size / sizeof(LogoBuiltinWord);

}

Logo::Logo(LogoPrimitives *primitives, LogoTimeProvider *time, LogoBuiltinWord *core, LogoString *strings, ArduinoFlashCode *code) : 
  _nextcode(0), 
  _primitives(primitives), 
  _core(core), 
  _nextstring(0), _fixedstrings(strings), _fixedcount(0), 
  _pc(0), _tos(0), _schedule(time),
  _staticcode(code) {
  
  _varcount = 0;
  _corecount = _core ? sizeof(Logo::core) / sizeof(LogoBuiltinWord) : 0;
  _nextjcode = START_JCODE;

  reset();
    
  // count the number of fixed strings.
  if (_fixedstrings) {
    short len = _fixedstrings->length();
    
    if (len > 0) {
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
  
  if (_staticcode) {
    short pc = 0;
    while (pc < MAX_CODE && (*_staticcode)[pc][0] != SCOPTYPE_WORD) {
      pc++;
    }
    for (int i=0; pc < MAX_CODE && ((*_staticcode)[pc])[0] != SCOPTYPE_END; i++, pc++) {
      if ((*_staticcode)[pc][0] == SCOPTYPE_VAR) {
        LogoStringResult result;
        getfixed(&result, i);
        newintvar(i, result.length(), (*_staticcode)[pc][1]);
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
    for (short j=0; j<INST_LENGTH; j++) {
      _stack[i][j] = 0;
    }
  }
  
}

void Logo::reset() {

  DEBUG_IN(Logo, "reset");
  
  // ALL the code
  short s = (sizeof(_code) / sizeof(tLogoInstruction));
  for (short i=0; i<s; i++) {
    for (short j=0; j<INST_LENGTH; j++) {
      _code[i][j] = 0;
    }
  }
  _code[START_JCODE-1][FIELD_OPTYPE] = OPTYPE_HALT;
  _nextstring = 0;
  _nextcode = 0;
  _nextjcode = START_JCODE;
  
  resetvars();
  restart();
  
}

void Logo::resetvars() {

  DEBUG_IN(Logo, "resetvars");
  
  _varcount = 0;

}

void Logo::resetcode() {

  DEBUG_IN(Logo, "resetcode");
  
  // Just the code up to the words.
  short s = min(START_JCODE-1, (sizeof(_code) / sizeof(tLogoInstruction)));
  for (short i=0; i<s; i++) {
    for (short j=0; j<INST_LENGTH; j++) {
      _code[i][j] = 0;
    }
  }
  _nextcode = 0;
 
  restart();
  
}

short Logo::parseint(short type, short op, short opand) {

  switch (type) {
  
  case OPTYPE_STRING:
    {
      LogoStringResult result;
      getstring(&result, op, opand);
      return result.toi();
    }
    break;
    
  case OPTYPE_INT:
  case OPTYPE_DOUBLE:
    return op;
    
  default:
    break;
    
  }
  
  return -1;
  
}

double Logo::parsedouble(short type, short op, short opand) {

  DEBUG_IN_ARGS(Logo, "parsedouble", "%i%i%i", type, op, opand);
  
  switch (type) {
  
  case OPTYPE_STRING:
    {
      LogoStringResult result;
      getstring(&result, op, opand);
      return result.tof();
    }
    break;
    
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

short Logo::getvarfromref(short op, short opand) {

  DEBUG_IN_ARGS(Logo, "getvarfromref", "%i%i", op, opand);
  
  LogoStringResult result;
  getstring(&result, op, opand);
  return findvariable(&result);

}

short Logo::popint() {

  DEBUG_IN(Logo, "popint");
  
  if (!pop()) {
    error(LG_STACK_OVERFLOW);
    return 0;
  }
  short i = parseint(_stack[_tos][FIELD_OPTYPE], _stack[_tos][FIELD_OP], _stack[_tos][FIELD_OPAND]);
  if (i >= 0) {
    return i;
  }
  if (_stack[_tos][FIELD_OPTYPE] == OPTYPE_REF) {
    short var = getvarfromref(_stack[_tos][FIELD_OP], _stack[_tos][FIELD_OPAND]);
    if (var >= 0) {
      i = parseint(_variables[var]._type, _variables[var]._value, _variables[var]._valuelen);
      if (i >= 0) {
        return i;
      }
    }
  }
  return 0;
}

double Logo::popdouble() {

  DEBUG_IN(Logo, "popdouble");
  
  if (!pop()) {
    error(LG_STACK_OVERFLOW);
    return 0;
  }
  double i = parsedouble(_stack[_tos][FIELD_OPTYPE], _stack[_tos][FIELD_OP], _stack[_tos][FIELD_OPAND]);
  if (i >= 0) {
    return i;
  }
  if (_stack[_tos][FIELD_OPTYPE] == OPTYPE_REF) {
    short var = getvarfromref(_stack[_tos][FIELD_OP], _stack[_tos][FIELD_OPAND]);
    if (var >= 0) {
      i = parsedouble(_variables[var]._type, _variables[var]._value, _variables[var]._valuelen);
      if (i >= 0) {
        return i;
      }
    }
  }
  return 0;
}

void Logo::pushint(short n) {

  DEBUG_IN(Logo, "pushint");
  
  if (!push(OPTYPE_INT, n)) {
    error(LG_STACK_OVERFLOW);
  }

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
  
  short op, opand;
  splitdouble(n, &op, &opand);

  if (!push(OPTYPE_DOUBLE, op, opand)) {
    error(LG_STACK_OVERFLOW);
  }
  
}

void Logo::pushstring(tStrPool n, tStrPool len) {

  DEBUG_IN(Logo, "pushstring");
  
  if (!push(OPTYPE_STRING, n, len)) {
    error(LG_STACK_OVERFLOW);
  }
  
}

void Logo::fail(short err) {
  
  DEBUG_IN_ARGS(Logo, "fail", "%i", err);
  
  if (_staticcode) {
#ifndef ARDUINO
    cout << "Can't fail in static code for now " << endl;
#endif
  }
  else {
    _code[_pc+1][FIELD_OPTYPE] = OPTYPE_ERR;
    _code[_pc+1][FIELD_OP] = err;
    _code[_pc+1][FIELD_OPAND] = 0;
  }
  
}

void Logo::modifyreturn(short rel, short count) {

  DEBUG_IN_ARGS(Logo, "modifyreturn", "%i%i", rel, _tos);
  
  if (!push(SOPTYPE_MRETADDR, rel, count)) {
    fail(LG_STACK_OVERFLOW);
  }

}

bool Logo::parsestring(short type, short op, short opand, LogoStringResult *str) {

  DEBUG_IN_ARGS(Logo, "parsestring", "%i%i%i", type, op, opand);
  
  switch (type) {
  
  case OPTYPE_STRING:
    getstring(str, op, opand);
    return true;
    
  case OPTYPE_INT:
    snprintf(_numbuf, sizeof(_numbuf), "%d", op);
    str->_simple.set(_numbuf, strlen(_numbuf));
    return true;
    
  case OPTYPE_DOUBLE:
    snprintf(_numbuf, sizeof(_numbuf), "%f", joindouble(op, opand));
    str->_simple.set(_numbuf, strlen(_numbuf));
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

void Logo::popstring(LogoStringResult *result) {

  DEBUG_IN(Logo, "popstring");
  
  if (!pop()) {
    error(LG_STACK_OVERFLOW);
    return;
  }
  if (!parsestring(_stack[_tos][FIELD_OPTYPE], _stack[_tos][FIELD_OP], _stack[_tos][FIELD_OPAND], result)) {
    short var = getvarfromref(_stack[_tos][FIELD_OP], _stack[_tos][FIELD_OPAND]);
    if (var >= 0) {
      if (!parsestring(_variables[var]._type, _variables[var]._value, _variables[var]._valuelen, result))
        result->init();
    }
  }
}

bool Logo::push(short type, short op, short opand) {

  DEBUG_IN_ARGS(Logo, "push", "%i%i", type, op);
  
  if (_tos >= MAX_STACK) {
    return false;
  }

  _stack[_tos][FIELD_OPTYPE] = type;
  _stack[_tos][FIELD_OP] = op;
  _stack[_tos][FIELD_OPAND] = opand;
  _tos++;
  
  return true;
  
}

short Logo::instField(short pc, short field) const {
  if (_staticcode) {
    return (*_staticcode)[pc][field];
  }
  return _code[pc][field];
}

bool Logo::codeisint(short rel) {

  DEBUG_IN_ARGS(Logo, "codeisint", "%i", rel);
  
  bool val;
  if (instField(_pc+rel, FIELD_OPTYPE) == OPTYPE_REF) {
    short var = getvarfromref(instField(_pc+rel, FIELD_OP), instField(_pc+rel, FIELD_OPAND));
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
 
  val = instField(_pc+rel, FIELD_OPTYPE) == OPTYPE_INT;
  DEBUG_RETURN(" num %b", val);
  return val;
  
}

short Logo::codetoint(short rel) {

  DEBUG_IN_ARGS(Logo, "codetoint", "%i", rel);
  
  short val;
  if (instField(_pc+rel, FIELD_OPTYPE) == OPTYPE_REF) {
    short var = getvarfromref(instField(_pc+rel, FIELD_OP), instField(_pc+rel, FIELD_OPAND));
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
  
  if (instField(_pc+rel, FIELD_OPTYPE) != OPTYPE_INT) {
    error(LG_NOT_INT);
    return 0;
  }

  val = instField(_pc+rel, FIELD_OP);
  DEBUG_RETURN(" num %i", val);
  return val;

}

bool Logo::codeisstring(short rel) {

  return instField(_pc+rel, FIELD_OPTYPE) == OPTYPE_STRING;

}

void Logo::codetostring(short rel, tStrPool *s, tStrPool *len) {

  if (!codeisstring(rel)) {
    error(LG_NOT_STRING);
    return;
  }
  *s = instField(_pc+rel, FIELD_OP);
  *len = instField(_pc+rel, FIELD_OPAND);
   
}

void Logo::jumpskip(short rel) {

  DEBUG_IN_ARGS(Logo, "jumpskip", "%i", rel);
  
  jump(rel);
  
  if (!push(SOPTYPE_SKIP)) {
    error(LG_STACK_OVERFLOW);
  }
}

void Logo::jump(short rel) {

  _pc += rel - 1;
  
}

bool Logo::call(short jump, tByte arity) {

  DEBUG_IN_ARGS(Logo, "call", "%i%i", jump, (short)arity);
  
  if (jump == NO_JUMP) {
    DEBUG_RETURN(" word ignored", 0);
    return true;
  }
  
  if (_tos >= MAX_STACK) {
    return false;
  }
  
  if (arity) {
    push(SOPTYPE_ARITY, _pc, arity);
    
    DEBUG_RETURN(" word has arity", 0);
    return true;
  }

  // push the return adddres
  push(SOPTYPE_RETADDR, _pc + 1);
  
  // and go.
  _pc = jump - 1;

  return true;
  
}

int Logo::callword(const char *word) {

  LogoSimpleString s(word);
  
  // true for a builtin.
  short index = -1, category = 0;
  findbuiltin(&s, 0, s.length(), &index, &category);
  if (index >= 0) {
  
    if (category == 0 && _primitives) {
      _primitives->exec(index, this);
      return 0;
    }
    
    const LogoBuiltinWord *builtin = getbuiltin(index, category);
    if (!builtin) {
      // problem with the code, no actual builtin?
      return LG_OUT_OF_CODE;
    }
    builtin->_code(*this);
    return 0;
  }

  // try for a word with this string.
  short n = findfixed(&s, 0, s.length());
  if (n < 0) {
    return LG_WORD_NOT_FOUND;
  }
  int w = 0;
  for (short i=0; i<MAX_CODE; i++) {
    short type = instField(i, FIELD_OPTYPE);
    if (type == SCOPTYPE_WORD) {
      if (w == n) {
        short op = instField(i, FIELD_OP);
        short opand = instField(i, FIELD_OPAND);
        if (opand > 0) {
          // could probably just check the stack and see if it has the arity
          // and then go for it.
          return LG_ARITY_NOT_IMPL;
        }
        call(op+1, 0);
        return 0;
      }
      w++;
    }
  }
  
  // problem with the code, couldn't find the word.
  return LG_OUT_OF_CODE;
}

void Logo::schedulenext(short delay) {
  _schedule.schedule(delay); 
}

void Logo::getbuiltinname(short op, short opand, char *s, int len) const {
  if (opand == 0 && _primitives) {
    _primitives->name(op, s, len);
  }
  else {
    int len = min(len, strlen(_core[opand == 1 ? op : 0]._name));
    strncpy(s, _core[opand == 1 ? op : 0]._name, len);
    s[len] = 0;
  }
}

const LogoBuiltinWord *Logo::getbuiltin(short op, short opand) const {

  if (opand == 1) {
    return &_core[op];
  }
  else {
    return &Logo::core[0];
  }
  
}

short Logo::step() {

//  DEBUG_IN(Logo, "step");
  
  if (!_schedule.next()) {
    return 0;
  }
  
  if (doarity()) {
    return 0;
  }
  
  // just in case arity ACTUALLY called a builtin.
  if (!_schedule.next()) {
    return 0;
  }
  
  short type = instField(_pc, FIELD_OPTYPE);
  
  // make sure stack ops don't make it onto here.
  if (type >= SOP_START) {
//    cout << (short)_pc << " " << type << endl;
    return LG_UNHANDLED_OP_TYPE;
  }
  
  short err = 0;
  
  switch (type) {
  
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
    {
      // push anything we find onto the stack.
      if (!push(type, instField(_pc, FIELD_OP), instField(_pc, FIELD_OPAND))) {
        err = LG_STACK_OVERFLOW;
      }
    }
    break;
    
  case OPTYPE_REF:
    {
      short var = getvarfromref(instField(_pc, FIELD_OP), instField(_pc, FIELD_OPAND));
      if (var >= 0) {
        if (!push(_variables[var]._type, _variables[var]._value, _variables[var]._valuelen)) {
          err = LG_STACK_OVERFLOW;
        }
      }
      else {
        // just push qa zero
        if (!push(OPTYPE_INT, 0, 0)) {
          err = LG_STACK_OVERFLOW;
        }
      }
    }
    break;
    
  case OPTYPE_POPREF:
  
    // pop the value from the stack
    if (!pop()) {
      err = LG_STACK_OVERFLOW;
      break;
    }
    if (_stack[_tos][FIELD_OPTYPE] != OPTYPE_INT) {
      err = LG_NOT_INT;
      break;
    }
  
    // and set the variable to that value.
    _variables[instField(_pc, FIELD_OP)]._value = _stack[_tos][FIELD_OP];

    break;
    
  case OPTYPE_JUMP:
  
    if (!call(instField(_pc, FIELD_OP), instField(_pc, FIELD_OPAND))) {
      err = LG_STACK_OVERFLOW;
    }
    break;
    
  case OPTYPE_ERR:
    err = instField(_pc, FIELD_OP);
    break;
    
  default:
    err = LG_UNHANDLED_OP_TYPE;
  }

  _pc++;
  
  return err;
  
}

// this is not quite sufficient for nested infixes. Still work to do.
bool Logo::doinfix() {

  DEBUG_IN(Logo, "doinfix");

  if (instField(_pc, FIELD_OPTYPE) == OPTYPE_BUILTIN) {
  
    short op = instField(_pc, FIELD_OP);
    short opand = instField(_pc, FIELD_OPAND);
    
    // find the arity of the builtin.
    short arity = 0;
    if (_primitives && opand == 0) {
      arity = _primitives->arity(op);
    }
    else {
      const LogoBuiltinWord *builtin = getbuiltin(op, opand);
      if (builtin) {
        arity = builtin->_arity;
      }
    }
    
    if (arity == 255) {
    
      // open up a spot in the stack with the current whatever it is.
      memmove(_stack + _tos, _stack + _tos - 1, (1) * sizeof(tLogoInstruction));
      
      // put a final arity in there.
      _stack[_tos-1][FIELD_OPTYPE] = SOPTYPE_ARITY;
      _stack[_tos-1][FIELD_OP] = _pc;          
      _stack[_tos-1][FIELD_OPAND] = 0;          
      _tos++;
      _pc++;
      DEBUG_RETURN(" true", 0);
      return true;
    }

  }
  
  DEBUG_RETURN(" false", 0);
  return false;
}

bool Logo::doarity() {

  DEBUG_IN(Logo, "doarity");

  short ar = _tos-1;
  while (ar >= 0 && _stack[ar][FIELD_OPTYPE] != SOPTYPE_RETADDR && _stack[ar][FIELD_OPTYPE] != SOPTYPE_ARITY) {
    ar--;
  }
  if (ar < 0) {
    DEBUG_RETURN(" no arity", 0);
    return false;
  }
  
  if (_stack[ar][FIELD_OPTYPE] == SOPTYPE_RETADDR) {
    DEBUG_RETURN(" found return address before arity", 0);
    return false;
  }
  
  if (_stack[ar][FIELD_OPAND] == 255) {
    _stack[ar][FIELD_OPAND] = 0;
    DEBUG_RETURN(" going once more for infix", 0);
    return false;
  }

  if (_stack[ar][FIELD_OPAND] > 0) {
    _stack[ar][FIELD_OPAND]--;
    DEBUG_RETURN(" going again", 0);
    return false;
  }

  DEBUG_OUT("finished", 0);

  short pc = _stack[ar][FIELD_OP];
  if (instField(pc, FIELD_OPTYPE) == OPTYPE_BUILTIN) {
  
    if (doinfix()) {
      DEBUG_RETURN(" found infix", 0);
      return false;
    }
    
    memmove(_stack + ar, _stack + ar + 1, (_tos - ar + 1) * sizeof(tLogoInstruction));
    _tos--;

    short op = instField(pc, FIELD_OP);
    short opand = instField(pc, FIELD_OPAND);
    
    DEBUG_OUT("calling builtin %i%i", op, opand);
    
//    dumpstack(0, false);
    if (_primitives && opand == 0) {
      // call the primitive.
      _primitives->exec(op, this);
    }
    else {
      getbuiltin(op, opand)->_code(*this);
    }
//    dumpstack(0, false);
    
    DEBUG_RETURN(" called builtin", 0);
    return true;
  }
  
  if (instField(pc, FIELD_OPTYPE) == OPTYPE_JUMP) {
  
    DEBUG_OUT("jumping to %i", instField(pc, FIELD_OP));
    
    // replace the arity entry with the return address
    _stack[ar][FIELD_OPTYPE] = SOPTYPE_RETADDR;
    _stack[ar][FIELD_OP] = pc + instField(pc, FIELD_OPAND) + 1;
    _stack[ar][FIELD_OPAND] = 0;
    
    // and go.
    _pc = instField(pc, FIELD_OP);
    
    DEBUG_RETURN(" jumping", 0);
    return false;
  }

  return false;
}

short LogoFunctionPrimitives::find(LogoString *str, short start, short slen) {

  for (short i=0; i<_count; i++) {
    if (strlen(_builtins[i]._name) == slen && str->ncmp(_builtins[i]._name, start, slen) == 0) {
      return i;
    }
  }
  return -1;
}

short LogoFunctionPrimitives::arity(short index) {
  return _builtins[index]._arity;
}

void LogoFunctionPrimitives::exec(short index, Logo *logo) {
  if (_builtins) {
    _builtins[index]._code(*logo);
  }
}

void LogoFunctionPrimitives::name(short index, char *s, int len) {
  if (_builtins) {
    len = min(len, strlen(_builtins[index]._name));
    strncpy(s, _builtins[index]._name, len);
    s[len] = 0;
  }
}

short Logo::dobuiltin() {

  DEBUG_IN(Logo, "dobuiltin");

  // save away where we will call to.
  short call = _pc;
  
  short opand = instField(call, FIELD_OPAND);
  short op = instField(call, FIELD_OP);
  
  if (_primitives && opand == 0) {
    short arity = _primitives->arity(op);
    
    if (arity == 0) {
      // call the primitive.
      _primitives->exec(op, this);
      return 0;
    }
    
    // push arity. We need to wait for this many calls.
    if (!push(SOPTYPE_ARITY, call, arity)) {
      return LG_STACK_OVERFLOW;
    }
  
    return 0;
    
  }
  
  // get the builtin.
  const LogoBuiltinWord *builtin = getbuiltin(op, opand);
  if (!builtin) {
      // problem with the code, no actual builtin?
      return LG_OUT_OF_CODE;
  }
  short arity = builtin->_arity;
  
  if (arity == 0) {
    // call the builtin.
    builtin->_code(*this);
    return 0;
  }
  
  // push arity. We need to wait for this many calls.
  if (!push(SOPTYPE_ARITY, call, arity)) {
    return LG_STACK_OVERFLOW;
  }
  
  return 0;
  
}

short Logo::doreturn() {

  DEBUG_IN(Logo, "doreturn");

  // handle returning
  
  if (!pop()) {
    return LG_STACK_OVERFLOW;
  }

  
  // find the return address on the stack
  short ret = _tos;
  while (ret > 0 && _stack[ret][FIELD_OPTYPE] != SOPTYPE_RETADDR) {
//    dump(3, _stack[ret]); cout << endl;
    ret--;
  }

//   dumpstack(false);
//   dump(3, _stack[_tos]); cout << endl;
//   dump(3, _stack[ret]); cout << endl;
  
  if (ret > 0) {
  
    if (_stack[ret-1][FIELD_OPTYPE] == SOPTYPE_SKIP) {
  
      DEBUG_OUT("skipping", 0);
      
       _pc = _stack[ret][FIELD_OP] + 1;

      // shuffle the stack down to remove out skip but leave whatever
      // is there after the return
      memmove(_stack + ret - 1, _stack + ret + 1, (_tos - ret + 1) * sizeof(tLogoInstruction));
      _tos--;

      return 0;
    }
    
    if (_stack[ret-1][FIELD_OPTYPE] == SOPTYPE_MRETADDR) {

      if (_stack[ret-1][FIELD_OPAND] == -1) {
        DEBUG_OUT("forever modify return by %i", _stack[ret-1][FIELD_OP]);
        _pc = _stack[ret][FIELD_OP] + _stack[ret-1][FIELD_OP];
      }
      else if (_stack[ret-1][FIELD_OPAND] > 1) {
        _stack[ret-1][FIELD_OPAND]--;
        DEBUG_OUT("modify return by %i", _stack[ret-1][FIELD_OPAND]);
        _pc = _stack[ret][FIELD_OP] + _stack[ret-1][FIELD_OP];
      }
      else {
        DEBUG_OUT("finished mod return", 0);
        _pc = _stack[ret][FIELD_OP];
        _tos--;
      }
      
      return 0;
    }
  }
  
  // here next
  _pc = _stack[ret][FIELD_OP];

  // shuffle the stack down so that words can push data
  memmove(_stack + ret, _stack + ret + 1, (_tos - ret) * sizeof(tLogoInstruction));
 
  return 0;
    
}

void Logo::setprimitives(LogoString *str, short start, short slen) {
  if (_primitives) {
    _primitives->set(str, start, slen);
  }
}

void Logo::addop(tJump *next, short type, short op, short opand) {

  DEBUG_IN_ARGS(Logo, "addop", "%i%i%i", type, op, opand);

  if (_staticcode) {
#ifndef ARDUINO
    cout << "can't add op with static code for now" << endl;
#endif
  }
  else {
    _code[*next][FIELD_OPTYPE] = type;
    _code[*next][FIELD_OP] = op;
    _code[*next][FIELD_OPAND] = opand;
    (*next)++;
  }
  
}

void Logo::findbuiltin(LogoString *str, short start, short slen, short *index, short *category) {

  DEBUG_IN_ARGS(Logo, "findbuiltin", "%i%i", start, slen);
  
  *index = -1;
  *category = 0;
  if (_primitives) {
    *index = _primitives->find(str, start, slen);
  }
  
  if (*index < 0) {
    for (short i=0; i<_corecount; i++) {
      if (strlen(_core[i]._name) == slen && str->ncmp(_core[i]._name, start, slen) == 0) {
        *index = i;
        *category = 1;
        return;
      }
    }
  }

  DEBUG_RETURN(" %i%i", *index, *category);

}

short Logo::findfixed(LogoString *stri, short strstart, short slen) const {

  DEBUG_IN_ARGS(Logo, "findfixed", "%i%i", strstart, slen);
  
//  stri->dump("findfixed", strstart, slen);
  
  if (_fixedstrings) {
//    _fixedstrings->dump("findfixed", 0, _fixedstrings->length());
    short len = _fixedstrings->length();
    short start = 0;
    short index = 0;
    for (int i=0; i<len; i++) {
      if ((*_fixedstrings)[i] == '\n') {
        if (_fixedstrings->ncmp2(stri, strstart, start, i-start) == 0) {
          DEBUG_RETURN(" found %i", index);
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

bool Logo::fixedcmp(LogoString *stri, short strstart, short slen, tStrPool str, tStrPool len) const {

//  DEBUG_IN_ARGS(Logo, "fixedcmp", "%i%i%i%i", strstart, slen, str, len);
  
  if (_fixedstrings) {
    short tlen = _fixedstrings->length();
    short start = 0;
    short index = 0;
    for (int i=0; i<tlen; i++) {
      if ((*_fixedstrings)[i] == '\n') {
        if (index == str) {
          if (_fixedstrings->ncmp2(stri, strstart, start, min(slen, len)) == 0) {
            return true;
          }
        }
        start = i + 1;
        index++;
      }
    }
  }

  return false;
  
}

bool Logo::getfixed(LogoStringResult *result, tStrPool str) const {

//  DEBUG_IN_ARGS(Logo, "getfixed", "%i", str);
  
  if (_fixedstrings && str < _fixedcount) {
    short start = 0;
    short index = 0;
    short len = _fixedstrings->length();
    for (int i=0; i<len; i++) {
      if ((*_fixedstrings)[i] == '\n') {
        if (index == str) {
          result->_fixed = _fixedstrings;
          result->_fixedstart = start;
          result->_fixedlen = i-start;
//          DEBUG_RETURN(" %b", true);
          return true;
        }
        start = i + 1;
        index++;
      }
    }
  }
  
//  DEBUG_RETURN(" %b", false);
  return false;
}

short Logo::addstring(LogoString *str, short start, short slen) {

  DEBUG_IN_ARGS(Logo, "addstring", "%i%i", start, slen);
  
  short existing = findfixed(str, start, slen);
  if (existing >= 0) {
    DEBUG_RETURN(" fixed %i", existing);
    return existing;
  }

  if ((_nextstring + slen) >= STRING_POOL_SIZE) {
    DEBUG_RETURN(" %i", -1);
    return -1;
  }
  
  short cur = _nextstring;
  str->ncpy(_strings + cur, start, slen);
  _nextstring += slen;
  
  short stri = cur + _fixedcount;
  DEBUG_RETURN(" %i", stri);
  return stri;
}

short Logo::addstring(LogoStringResult *stri) {

  if (stri->_fixed) {
    return addstring(stri->_fixed, stri->_fixedstart, stri->_fixedlen);
  }
  return addstring(&stri->_simple, 0, stri->_simple.length());

}

bool Logo::stringcmp(LogoString *stri, short start, short slen, tStrPool str, tStrPool len) const {

  if (slen != len) {
    return false;
  }
  
  if (fixedcmp(stri, start, slen, str, len)) {
    return true;
  }
  
  return stri->ncmp(_strings + (str - _fixedcount), start, len) == 0;
  
}

bool Logo::stringcmp(LogoStringResult *stri, tStrPool str, tStrPool len) const {

//  DEBUG_IN(Logo, "stringcmp");
  
  if (stri->length() != len) {
//    DEBUG_RETURN(" no len", 0);
    return false;
  }
  
  if (stri->_fixed) {
    return stringcmp(stri->_fixed, stri->_fixedstart, stri->_fixedlen, str, len);
  }
  else {
    return stringcmp(&stri->_simple, 0, stri->_simple.length(), str, len);
  }
  
}

void Logo::getstring(LogoStringResult *stri, tStrPool str, tStrPool len) const {

//  DEBUG_IN_ARGS(Logo, "getstring", "%i%i", str, len);
  
  if (getfixed(stri, str)) {
// #ifdef ARDUINO
//     Serial.println("got fixed");
// #endif
    return;
  }
  stri->_simple.set(_strings + (str - _fixedcount), len);
  
}

short Logo::findvariable(LogoString *stri, short start, short slen) const {

  DEBUG_IN_ARGS(Logo, "findvariable", "%i%i", start, _varcount);
  
  for (short i=0; i<_varcount; i++) {
    if (stringcmp(stri, start, slen, _variables[i]._name, _variables[i]._namelen)) {
      return i;
    }
  }
  
  return -1;
}
short Logo::findvariable(LogoStringResult *stri) const {

  DEBUG_IN_ARGS(Logo, "findvariable", "%i", _varcount);
  
  for (short i=0; i<_varcount; i++) {
    if (stringcmp(stri, _variables[i]._name, _variables[i]._namelen)) {
      DEBUG_RETURN(" %i", i);
      return i;
    }
  }
  
  DEBUG_RETURN(" %i", -1);
  return -1;
}

short Logo::geterr() {

  DEBUG_IN(Logo, "geterr");
  
  // walk through the code looking for errors.
  for (short i=0; i<CODE_SIZE; i++) {
    if (_code[i][FIELD_OPTYPE] == OPTYPE_ERR) {
      DEBUG_RETURN(" code %i", i);
      return _code[i][FIELD_OP];
    }
  }

  // couldn't find any
  DEBUG_RETURN(" %i", 0);
  return 0;
  
}

bool Logo::haserr(short err) {

  DEBUG_IN_ARGS(Logo, "haserr", "%i", err);
  
  // walk through the code looking for a particular error.
  for (short i=0; i<CODE_SIZE; i++) {
    if (_code[i][FIELD_OPTYPE] == OPTYPE_ERR && _code[i][FIELD_OP] == err) {
      return true;
    }
  }

  return false;
}

void Logo::error(short error) {
  addop(&_nextcode, OPTYPE_ERR, error);
}

void Logo::outofcode() {

  // if we run out code, put the error as the FIRST instruction
  // and reset the pc so that we simply fill it up again this
  // will allow people to see what code overflowed.
  _nextcode = 0;
  addop(&_nextcode, OPTYPE_ERR, LG_OUT_OF_CODE);

}

short Logo::newintvar(short str, short slen, short n) {

  if (str < 0) {
    error(LG_OUT_OF_STRINGS);
    return -1;
  }
  if (_varcount >= MAX_VARS) {
    error(LG_TOO_MANY_VARS);
    return -1;
  }
  short var = _varcount;
  _varcount++;
  _variables[var]._name = str;
  _variables[var]._namelen = slen;
  _variables[var]._type = OPTYPE_INT;
  _variables[var]._value = n;
  _variables[var]._valuelen = 0;
  return var;
  
}
void Logo::setintvar(short var, short n) {

  _variables[var]._type = OPTYPE_INT;
  _variables[var]._value = n;
  _variables[var]._valuelen = 0;
  
}

void LogoScheduler::schedule(short ms) {
  if (!_provider) {
    return;
  }
  if (_provider->testing(ms)) {
    return;
  }
  if (_time == 0) {
    _time = ms;
  }
  else {
    _time += ms;
  }
  _lasttime = _provider->currentms();
// #ifdef ARDUINO
//   Serial.print("time ");
//   Serial.println(_time);
// #endif
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
// #ifdef ARDUINO
//   Serial.print("diff ");
//   Serial.println(diff);
// #endif
  if (diff > _time) {
    _lasttime = now;
    _time = 0;
// #ifdef ARDUINO
//     Serial.print("lasttime ");
//     Serial.println(_lasttime);
// #endif
    return true;
  }
  _provider->delayms(DELAY_TIME);
  return false;

}

#ifndef ARDUINO

void Logo::dumpinstline(short type, short op, short opand, int offset, int line, ostream &str) const {

  char name[STRING_LEN];
  str << "\t{ ";
  optypename(type, str);
  str << ", ";
  if (type == OPTYPE_JUMP) {
    str << op + offset << ", " << opand;
  }
  else if (type == OPTYPE_STRING) {
    LogoStringResult result;
    if (getfixed(&result, op)) {
      short fixed = findfixed(result._fixed, result._fixedstart, result._fixedlen);
      str << fixed << ", " << opand;
    }
    else {
      str << "(missing_fixed, " << op << "), ";
    }
  }
  else {
    str << op << ", " << opand;
  }
  str << " },\t\t// " << line << endl;
  
}

void Logo::dumpinst(LogoCompiler *compiler, const char *varname, ostream &str) const {

  // find the start of the jump code.
  short jstart = 0;
  short codeend = 0;
  while (jstart<CODE_SIZE) {
    if (!codeend && _code[jstart][0] == OPTYPE_NOOP) {
      codeend = jstart;
    }
    if (_code[jstart++][0] == OPTYPE_HALT) {
      break;
    }
  }
    
  short offset = (codeend + 1) - jstart;
  bool haswords = compiler->haswords();
  str << "static const short " << varname << "[][INST_LENGTH] PROGMEM = {" << endl;
  for (int i=0; i<codeend; i++) {
    dumpinstline(_code[i][FIELD_OPTYPE], _code[i][FIELD_OP], _code[i][FIELD_OPAND], offset, i, str);
  }
  str << "\t{ OPTYPE_HALT, 0, 0 },\t\t// " << codeend << endl;
  for (int i=jstart; _code[i][0] != OPTYPE_NOOP && i<CODE_SIZE; i++) {
    dumpinstline(_code[i][FIELD_OPTYPE], _code[i][FIELD_OP], _code[i][FIELD_OPAND], offset, i + offset, str);
  }
  if (haswords) {
    compiler->dumpwordscode(offset, str);
  }
  str << "\t{ SCOPTYPE_END, 0, 0 } " << endl;
  str << "};" << endl;

}

bool Logo::safestringcat(short op, short opand, char *buf, int len) const {

  LogoStringResult result;
  getstring(&result, op, opand);
  int nlen = result.length();
  int buflen = strlen(buf);
  if ((buflen + (nlen + 1)) >= len) {
    return false;
  }
  result.ncpy(buf + buflen, nlen);
  strncat(buf + buflen + 1, "\n", 1);
  return true;
  
}

int Logo::stringslist(LogoCompiler *compiler, char *buf, int len) const {

  int count = 0;
  int wcount = compiler->wordstringslist(buf, len);
  if (wcount < 0) {
    return -1;
  }
  count += wcount;
  int vcount = varstringslist(compiler, buf, len);
  if (vcount < 0) {
    return -1;
  }
  count += vcount;
  // and add in the ACTUAL strings.
  for (short i=0; i<CODE_SIZE; i++) {
    if (_code[i][FIELD_OPTYPE] == OPTYPE_STRING) {
      if (!safestringcat(_code[i][FIELD_OP], _code[i][FIELD_OPAND], buf, len)) {
        return -1;
      }
      count++;
    }
  }
  
  return count;
  
}

void Logo::dumpstringscode(LogoCompiler *compiler, const char *varname, ostream &str) const {

  str << "static const char "<< varname << "[] PROGMEM = {" << endl;
  // these are in a particuar order
  compiler->dumpwordstrings(str);
  dumpvarsstrings(compiler, str);
  char name[STRING_LEN];
  for (short i=0; i<CODE_SIZE; i++) {
    if (_code[i][FIELD_OPTYPE] == OPTYPE_STRING) {
      LogoStringResult result;
      getstring(&result, _code[i][FIELD_OP], _code[i][FIELD_OPAND]);
      result.ncpy(name, sizeof(name));
      str << "\t\"" << name << "\\n\"" << endl;
    }
  }
  str << "};" << endl;
}

void Logo::optypename(short optype, ostream &str) const {

  switch (optype) {
  case OPTYPE_NOOP:
    str << "OPTYPE_NOOP";
    break;
  case OPTYPE_RETURN:
    str << "OPTYPE_RETURN";
    break;
  case OPTYPE_HALT:
    str << "OPTYPE_HALT";
    break;
  case OPTYPE_BUILTIN:
    str << "OPTYPE_BUILTIN";
    break;
  case OPTYPE_ERR:
    str << "OPTYPE_ERR";
    break;
  case OPTYPE_JUMP:
    str << "OPTYPE_JUMP";
    break;
  case OPTYPE_STRING:
    str << "OPTYPE_STRING";
    break;
  case OPTYPE_INT:
    str << "OPTYPE_INT";
    break;
  case OPTYPE_DOUBLE:
    str << "OPTYPE_DOUBLE";
    break;
  case OPTYPE_REF:
    str << "OPTYPE_REF";
    break;
  case OPTYPE_POPREF:
    str << "OPTYPE_POPREF";
    break;
  case SOPTYPE_ARITY:
    str << "SOPTYPE_ARITY";
    break;
  case SOPTYPE_RETADDR:
    str << "SOPTYPE_RETADDR";
    break;
  case SOPTYPE_MRETADDR:
    str << "SOPTYPE_MRETADDR";
    break;
  case SOPTYPE_SKIP:
    str << "SOPTYPE_SKIP";
    break;
  default:
    str << "BAD_OP_" << optype;
  }
  
}

void Logo::printvarcode(const LogoVar &var, ostream &str) const {

  char name[LINE_LEN];
  LogoStringResult result;
  getstring(&result, var._name, var._namelen);
  result.ncpy(name, sizeof(name));
  str << "\t{ SCOPTYPE_VAR, " << var._value << ", " << (short)var._valuelen << " }, " << endl;
  
}

void Logo::dumpvarscode(const LogoCompiler *compiler, ostream &str) const {

  if (!_varcount) {
    return;
  }
  
  for (short i=0; i<_varcount; i++) {
    printvarcode(_variables[i], str);
  }
}

void Logo::printvarstring(const LogoVar &var, ostream &str) const {

  char name[LINE_LEN];
  LogoStringResult result;
  getstring(&result, var._name, var._namelen);
  result.ncpy(name, sizeof(name));
  str << "\t\"" << name << "\\n\"";

}

void Logo::dumpvarsstrings(const LogoCompiler *compiler, ostream &str) const {

  if (!_varcount) {
    return;
  }
  
  for (short i=0; i<_varcount; i++) {
    printvarstring(_variables[i], str);
    str << endl;
  }
  
}

int Logo::varstringslist(LogoCompiler *compiler, char *buf, int len) const {

  if (!_varcount) {
    return 0;
  }
  
  for (short i=0; i<_varcount; i++) {
    if (!safestringcat(_variables[i]._name, _variables[i]._namelen, buf, len)) {
      return -1;
    }
  }

  return _varcount;
  
}

void Logo::dumpvars(const LogoCompiler *compiler) const {

  cout << "vars: " << endl;
  
  if (!_varcount) {
    entab(1);
    cout << "empty" << endl;
    return;
  }
  
  for (short i=0; i<_varcount; i++) {
    entab(1);
    compiler->printvar(_variables[i]);
    cout << endl;
  }
  
}

void Logo::entab(short indent) const {
  for (short i=0; i<indent; i++) {
    cout << "\t";
  }
}

void Logo::dump(short indent, short type, short op, short opand) const {

  entab(indent);
  char str[128];
  LogoStringResult result;
  switch (type) {
    case OPTYPE_NOOP:
      cout << "noop";
      break;
    case OPTYPE_RETURN:
      cout << "ret";
      break;
    case OPTYPE_HALT:
      cout << "halt";
      break;
    case OPTYPE_BUILTIN:
      getbuiltinname(op, opand, str, sizeof(str));
      cout << "builtin " << str;
      break;
    case OPTYPE_JUMP:
      cout << "jump " << op;
      break;
    case OPTYPE_STRING:
      getstring(&result, op, opand);
      result.ncpy(str, sizeof(str));
      cout << "string " << str;
      break;
    case OPTYPE_INT:
      cout << "integer " << op;
      break;
    case OPTYPE_DOUBLE:
      cout << "double " << joindouble(op, opand);
      break;
    case OPTYPE_REF:
      getstring(&result, op, opand);
      result.ncpy(str, sizeof(str));
      cout << "ref " << str;
      break;
    case OPTYPE_POPREF:
       cout << "pop ref " << op;
      break;
    case OPTYPE_ERR:
      cout << "err " << op;
      break;
    case SOPTYPE_ARITY:
      cout << "(stack) arity pc " << op << " to go " << opand;
      break;
    case SOPTYPE_MRETADDR:
      cout << "(stack) mod ret by " << op << " " << opand << " times";
      break;
    case SOPTYPE_RETADDR:
      cout << "(stack) ret to " << op;
      break;
    case SOPTYPE_SKIP:
      cout << "(stack) skip ";
      break;
    default:
      cout << "unknown optype " << type;
  }

}

void Logo::printvar(const LogoVar &var) const {

  char name[LINE_LEN];
  LogoStringResult result;
  getstring(&result, var._name, var._namelen);
  result.ncpy(name, sizeof(name));
  cout << name;
  dump(2, var._type, var._value, var._valuelen);
  
}

void Logo::dumpvars() const {

  cout << "vars: " << endl;
  
  if (!_varcount) {
    entab(1);
    cout << "empty" << endl;
    return;
  }
  
  for (short i=0; i<_varcount; i++) {
    entab(1);
    printvar(_variables[i]);
    cout << endl;
  }
  
}

void Logo::dumpstaticwords(const LogoCompiler *compiler) const {

  if (_staticcode) {
    cout << "static words:" << endl;

    char name[STRING_LEN];
    int word = 0;
    for (short i=0; i<MAX_CODE; i++) {
      short type = instField(i, FIELD_OPTYPE);
      if (type == SCOPTYPE_WORD) {
        short op = instField(i, FIELD_OP);
        short opand = instField(i, FIELD_OPAND);
        LogoStringResult result;
        getfixed(&result, word);
        result.ncpy(name, sizeof(name));
        entab(1);
        cout << name << " (" << op << ") "<< opand << endl;
        word++;
      }
    }
  }
  
}

void Logo::dumpcode(const LogoCompiler *compiler, bool all) const {

  if (_staticcode) {
    cout << "static code: pc (" << (short)_pc << ")" << endl;
    
    for (short i=0; i<MAX_CODE; i++) {
      short type = instField(i, FIELD_OPTYPE);
      if (type == SCOPTYPE_WORD) {
        break;
      }
      cout << i;
      short op = instField(i, FIELD_OP);
      short opand = instField(i, FIELD_OPAND);
      if (type == OPTYPE_JUMP) {
        entab(1);
        cout << "jump (" << op << ")";
      }
      if (compiler) {
        compiler->dump(1, type, op, opand);
      }
      else {
        dump(1, type, op, opand);
      }
      mark(i,  _pc, "pc");
      if (all) {
        mark(i, START_JCODE, "startjcode");
        mark(i, _nextjcode, "nextjcode");
      }
      cout << endl;
    }
  
  }
  cout << "code: pc (" << (short)_pc << ")" << endl;

  if (!_nextcode) {
    entab(1);
    cout << "empty" << endl;
  }

  for (short i=0; i<(all ? MAX_CODE : _nextcode); i++) {
    if (compiler) {
      compiler->markword(i);
      compiler->dump(1, instField(i, FIELD_OPTYPE), instField(i, FIELD_OP), instField(i, FIELD_OPAND));
    }
    else {
      cout << i;
      dump(1, instField(i, FIELD_OPTYPE), instField(i, FIELD_OP), instField(i, FIELD_OPAND));
    }
    mark(i, _pc, "pc");
    if (all) {
      mark(i, START_JCODE, "startjcode");
      mark(i, _nextjcode, "nextjcode");
    }
    cout << endl;
  }

  if (!all) {
    if (_pc >  _nextcode && _pc < START_JCODE) {
      for (short i= _nextcode; i<=_pc; i++) {
       if (compiler) {
          compiler->dump(1, instField(i, FIELD_OPTYPE), instField(i, FIELD_OP), instField(i, FIELD_OPAND));
        }
        else {
          cout << i;
          dump(1, instField(i, FIELD_OPTYPE), instField(i, FIELD_OP), instField(i, FIELD_OPAND));
        }
        mark(i, _pc, "pc");
        cout << endl;
      }
    }
    cout << "\t..." << endl;
    for (short i=START_JCODE; i<_nextjcode; i++) {
      if (compiler) {
        compiler->markword(i);
        compiler->dump(1, instField(i, FIELD_OPTYPE), instField(i, FIELD_OP), instField(i, FIELD_OPAND));
      }
      else {
        cout << i;
        dump(1, instField(i, FIELD_OPTYPE), instField(i, FIELD_OP), instField(i, FIELD_OPAND));
      }
      mark(i, _pc, "pc");
      mark(i, START_JCODE, "startjcode");
      cout << endl;
    }
  }

}

void Logo::mark(short i, short mark, const char *name) const {
  if (i == mark) {
    entab(2);
    cout << "(" << name << ")";
  }
}

void Logo::dumpstack(const LogoCompiler *compiler, bool all) const {

  cout << "stack: (" << _tos << ")" << endl;
  
  for (short i=0; i<(all ? MAX_STACK : _tos); i++) {
    if (compiler) {
      compiler->dump(1 ,_stack[i][FIELD_OPTYPE] ,_stack[i][FIELD_OP] ,_stack[i][FIELD_OPAND]);
    }
    else {
      dump(1 ,_stack[i][FIELD_OPTYPE] ,_stack[i][FIELD_OP] ,_stack[i][FIELD_OPAND]);
    }
    mark(i, _tos, "tos");
    cout << endl;
  }
  
}

#endif // !defined(ARDUINO)

short LogoWords::errArity = 0;

void LogoWords::err(Logo &logo) {

  logo.fail(LG_STOP);
  
}

short LogoWords::makeArity = 2;

void LogoWords::make(Logo &logo) {

  short n = logo.popint();
  LogoStringResult result;
  logo.popstring(&result);
  
// #ifdef ARDUINO
//   char s[256];
//   result.ncpy(s, sizeof(s));
//   Serial.println(s);
//   Serial.println(n);
// #endif
  
  short var = logo.findvariable(&result);
  if (var < 0) {
    var = logo.newintvar(logo.addstring(&result), result.length(), n);
  }
  else {
    logo.setintvar(var, n);    
  }

}

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

short LogoWords::ifelseArity = 1;

void LogoWords::ifelse(Logo &logo) {

//  logo.dump("ifelse", false);
  
  // this is a very special trick to allow us to manipulate the PC
  // based on the value of the stack.
  //
  // our code looks like:
	//  builtin IFELSE		(pc)
	//  TEST
	//  THEN
	//  ELSE
	//
  int cond = logo.popint();
  if (cond) {
    if (pushliterals(logo, 0)) {
      logo.jump(3);
    }
    else {
      logo.jumpskip(1);
    }
  }
  else {
    if (pushliterals(logo, 1)) {
      logo.jump(3);
    }
    else {
      logo.jump(2);
    }
  }
    
}

short LogoWords::ifArity = 1;

void LogoWords::ifWord(Logo &logo) {

  // this is a very special trick to allow us to manipulate the PC
  // based on the value of the stack.
  //
  // our code looks like:
	//  builtin IF		(pc)
	//  TEST
	//  THEN
	//
  int cond = logo.popint();
  if (cond) {
    if (pushliterals(logo, 0)) {
      logo.jump(2);
    }
  }
  else {
    logo.jump(2);
  }

}

short LogoWords::waitArity = 1;

void LogoWords::wait(Logo &logo) {

  // schedule the next word to execute after a certain time.
  logo.schedulenext(logo.popint());
  
}

short LogoWords::eqArity = 255;

void LogoWords::eq(Logo &logo) {

  logo.pushint(logo.popint() == logo.popint());
  
}

short LogoWords::neqArity = 255;

void LogoWords::neq(Logo &logo) {

  logo.pushint(logo.popint() != logo.popint());
  
}

short LogoWords::gtArity = 255;

void LogoWords::gt(Logo &logo) {

  // args are backwards.
  double i1 = logo.popdouble();
  double i2 = logo.popdouble();
  logo.pushint(i2 > i1);
  
}

short LogoWords::ltArity = 255;

void LogoWords::lt(Logo &logo) {

  // args are backwards.
  double i1 = logo.popdouble();
  double i2 = logo.popdouble();
  logo.pushint(i2 < i1);
  
}

short LogoWords::gteArity = 255;

void LogoWords::gte(Logo &logo) {

  // args are backwards.
  double i1 = logo.popdouble();
  double i2 = logo.popdouble();
  logo.pushint(i2 >= i1);
  
}

short LogoWords::lteArity = 255;

void LogoWords::lte(Logo &logo) {

  // args are backwards.
  double i1 = logo.popdouble();
  double i2 = logo.popdouble();
  logo.pushint(i2 <= i1);
  
}

short LogoWords::subtractArity = 255;

void LogoWords::subtract(Logo &logo) {

  // args are backwards.
  double a1 = logo.popdouble();
  double a2 = logo.popdouble();
  logo.pushdouble(a2 - a1);
  
}

short LogoWords::addArity = 255;

void LogoWords::add(Logo &logo) {

  double a1 = logo.popdouble();
  double a2 = logo.popdouble();
  logo.pushdouble(a2 + a1);
  
}

short LogoWords::divideArity = 255;

void LogoWords::divide(Logo &logo) {

  double a1 = logo.popdouble();
  double a2 = logo.popdouble();
  logo.pushdouble(a2 / a1);
  
}

short LogoWords::multiplyArity = 255;

void LogoWords::multiply(Logo &logo) {

  double a1 = logo.popdouble();
  double a2 = logo.popdouble();
  logo.pushdouble(a2 * a1);
  
}

short LogoWords::dumpvarsArity = 0;

void LogoWords::dumpvars(Logo &logo) {

#ifndef ARDUINO
  logo.dumpvars();
#endif
    
}

short LogoWords::printArity = 1;

void LogoWords::print(Logo &logo) {

  LogoStringResult result;
  logo.popstring(&result);
  char s[256];
  result.ncpy(s, sizeof(s));
#ifdef ARDUINO
  Serial.println(s);
#else
  cout << "=== " << s << endl;
#endif
  
}


