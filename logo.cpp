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
#include "logostring.hpp"
#include "arduinoflashcode.hpp"
#include "logowords.hpp"

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
#define DELAY_TIME  1 // must be non zero!
#endif

#ifdef ARDUINO
#include <Arduino.h>
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#else
#include "logocompiler.hpp"
#include <iostream>
#include <sstream>
using namespace std;
#endif

#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#include "logowordnames.hpp"

Logo::Logo(LogoTimeProvider *time, LogoString *strings, ArduinoFlashCode *code) : 
  _nextcode(0), 
  _corenames(coreNames), 
  _corearity(coreArity), 
  _nextstring(0), _fixedstrings(strings), _fixedcount(0), 
  _pc(0), _tos(0), _schedule(time),
  _staticcode(code),
  _sketch(0)
  {
  
  _varcount = 0;
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
#ifndef ARDUINO
  _ostream = 0;
#endif

  for (int i=0; i<(sizeof(_channels) / sizeof(tByte)); i++) {
    _channels[i] = 255; // invalid value;
  }
}

Logo::~Logo() {
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
  
  return 0;
  
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
  
  return 0;
  
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
  
  return parseint(_stack[_tos][FIELD_OPTYPE], _stack[_tos][FIELD_OP], _stack[_tos][FIELD_OPAND]);
}

double Logo::popdouble() {

  DEBUG_IN(Logo, "popdouble");
  
  if (!pop()) {
    error(LG_STACK_OVERFLOW);
    return 0;
  }
  
  return parsedouble(_stack[_tos][FIELD_OPTYPE], _stack[_tos][FIELD_OP], _stack[_tos][FIELD_OPAND]);

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

void Logo::pushstring(LogoString *stri) {

  short len = stri->length();
  short str = addstring(stri, 0, len);
  pushstring(str, len);
  
}

void Logo::pushlist(const List &list) {
  
  if (!push(OPTYPE_LIST, list.head(), list.tail())) {
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
      if (!parsestring(_variables[var]._type, _variables[var]._value, _variables[var]._valueopand, result))
        result->init();
    }
  }
}

bool Logo::isstacklist() {
  return _tos > 0 ? _stack[_tos-1][FIELD_OPTYPE] == OPTYPE_LIST : false;
}

bool Logo::isstackstring() {
  return _tos > 0 ? _stack[_tos-1][FIELD_OPTYPE] == OPTYPE_STRING : false;
}

bool Logo::isstackint() {
  return _tos > 0 ? _stack[_tos-1][FIELD_OPTYPE] == OPTYPE_INT : false;
}

bool Logo::isstackdouble() {
  return _tos > 0 ? _stack[_tos-1][FIELD_OPTYPE] == OPTYPE_DOUBLE : false;
}

List Logo::poplist() {

  if (isstacklist()) {
    if (!pop()) {
      error(LG_STACK_OVERFLOW);
      return List(&_lists);
    }
    List list(&_lists, _stack[_tos][FIELD_OP], _stack[_tos][FIELD_OPAND]);
    return list;
  }
  return List(&_lists);
  
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
  
//  dumpstack(0, false);
  
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
  
  bool val = instField(_pc+rel, FIELD_OPTYPE) == OPTYPE_INT;
  DEBUG_RETURN(" int %b", val);
  return val;
  
}

short Logo::codetoint(short rel) {

  DEBUG_IN_ARGS(Logo, "codetoint", "%i", rel);
  
  if (instField(_pc+rel, FIELD_OPTYPE) != OPTYPE_INT) {
    error(LG_NOT_INT);
    return 0;
  }

  short val = instField(_pc+rel, FIELD_OP);
  DEBUG_RETURN(" int %i", val);
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

void Logo::halt() {
  while (_pc < (CODE_SIZE - 1)) {
    if (_code[_pc+1][FIELD_OPTYPE] == OPTYPE_HALT) {
      return;
    }
   _pc++;
  }
}

bool Logo::isnum(LogoString *stri, short wordstart, short wordlen) {

  if (wordlen == 0) {
    return false;
  }
  for (short i=0; i<wordlen; i++) {
    if (!isdigit((*stri)[i + wordstart]) && (*stri)[i + wordstart] != '.') {
      return false;
    }
  }
  
  return true;
  
}

int Logo::callword(const char *word) {

  DEBUG_IN_ARGS(Logo, "callword", "%s", word);
  
  LogoSimpleString s(word);
  
  // allow vars to be set.
  int equals = s.find('=', 0, s.length());
  if (equals > 0) {
    short start = equals+1;
    short len = s.length()-equals-1;
    if (isnum(&s, start, len)) {
      short var = findvariable(&s, 0, equals);
      short val = s.toi(start, len);
      if (var < 0) {
        newintvar(addstring(&s, 0, equals), equals, val);
      }
      else {
        setintvar(var, val);
      }
      return 0;
    }
    else {
      return LG_NOT_INT;
    }
  }
  
  // handle a builtin.
  short index = findbuiltin(&s, 0, s.length());
  if (index >= 0) {
    callbuiltin(index);
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
        // now find the halt.
        _pc = 0;
        while (instField(_pc, FIELD_OPTYPE) != OPTYPE_HALT) {
          _pc++;
        }
        backup();
        call(op+1, 0);
        return 0;
      }
      w++;
    }
  }
  
  return LG_OUT_OF_CODE;
}

void Logo::schedulenext(short delay) {
  _schedule.schedule(delay); 
}

void Logo::getbuiltinname(short op, char *s, int len) const {
  LogoStringResult result;
  if (getfixedcr(&_corenames, &result, op)) {
    result.ncpy(s, len);
    s[len-1] = 0;
  }
  else {
    s[0] = 0;
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
    
  case OPTYPE_NOOP:
    break;
    
  case OPTYPE_GSTART:
    err = startgroup();
    break;
    
  case OPTYPE_GEND:
    err = endgroup();
    break;
    
  case OPTYPE_LSTART:
    err = startlist();
    break;
    
  case OPTYPE_LEND:
    err = endlist();
    break;
    
  case OPTYPE_RETURN:
    return doreturn();
    
  case OPTYPE_BUILTIN:
    err = dobuiltin();
    break;
    
  case OPTYPE_STRING:
  case OPTYPE_INT:
  case OPTYPE_DOUBLE:
    err = pushvalue(type, instField(_pc, FIELD_OP), instField(_pc, FIELD_OPAND));
    break;
    
  case OPTYPE_REF:
    {
      short var = getvarfromref(instField(_pc, FIELD_OP), instField(_pc, FIELD_OPAND));
      if (var >= 0) {
        err = pushvalue(_variables[var]._type, _variables[var]._value, _variables[var]._valueopand);
      }
      else {
        err = pushvalue(OPTYPE_INT, 0, 0);
      }
    }
    break;
    
  case OPTYPE_POPREF:
    {
      // pop the value from the stack
      if (!pop()) {
        err = LG_STACK_OVERFLOW;
        break;
      }
      // and set the variable to that value.
      short var = instField(_pc, FIELD_OP);
      _variables[var]._type = _stack[_tos][FIELD_OPTYPE];
      _variables[var]._value = _stack[_tos][FIELD_OP];
      _variables[var]._valueopand = _stack[_tos][FIELD_OPAND];
    }
    break;
    
  case OPTYPE_JUMP:
    {
      if (!call(instField(_pc, FIELD_OP), instField(_pc, FIELD_OPAND))) {
        err = LG_STACK_OVERFLOW;
      }
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

short Logo::pushvalue(short type, short op, short opand) {

  if (_stack[_tos-1][FIELD_OPTYPE] == SOPTYPE_OPENLIST) {
    // append to the open list.
    List l(&_lists, _stack[_tos-1][FIELD_OP], _stack[_tos-1][FIELD_OPAND]);
    ListNodeVal val(type, op, opand);
    l.push(val);
    _stack[_tos-1][FIELD_OPAND] = l.tail();
    return 0;
  }

  // push anything we find onto the stack.
  if (!push(type, op, opand)) {
    return LG_STACK_OVERFLOW;
  }
  return 0;
  
}

// this is not quite sufficient for nested infixes. Still work to do.
bool Logo::doinfix() {

  DEBUG_IN(Logo, "doinfix");

  if (instField(_pc, FIELD_OPTYPE) == OPTYPE_BUILTIN) {
  
    short op = instField(_pc, FIELD_OP);
    
    if (_corearity[op] == INFIX_ARITY) {
    
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
  while (ar >= 0 && 
    _stack[ar][FIELD_OPTYPE] != SOPTYPE_RETADDR && 
    _stack[ar][FIELD_OPTYPE] != SOPTYPE_ARITY && 
    _stack[ar][FIELD_OPTYPE] != SOPTYPE_GSTART && 
    _stack[ar][FIELD_OPTYPE] != SOPTYPE_OPENLIST) {
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
  
  if (_stack[ar][FIELD_OPTYPE] == SOPTYPE_GSTART) {
    DEBUG_RETURN(" found gstart before arity", 0);
    return false;
  }
  
  if (_stack[ar][FIELD_OPTYPE] == SOPTYPE_OPENLIST) {
    DEBUG_RETURN(" found list before arity", 0);
    return false;
  }
  
  if (_stack[ar][FIELD_OPAND] == INFIX_ARITY) {
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
    
    DEBUG_OUT("calling builtin %i", op);
    
//    dumpstack(0, false);
    callbuiltin(op);
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

short Logo::startgroup() {

  DEBUG_IN(Logo, "startgroup");

  if (!push(SOPTYPE_GSTART)) {
    return LG_STACK_OVERFLOW;
  }
  
  return 0;
}

short Logo::endgroup() {

  DEBUG_IN(Logo, "endgroup");

  // go back and find the last gstart
  short start = _tos-1;
  while (start >= 0 && _stack[start][FIELD_OPTYPE] != SOPTYPE_GSTART) {
    start--;
  }
  
  if (_stack[start][FIELD_OPTYPE] != SOPTYPE_GSTART) {
    return LG_STACK_OVERFLOW;
  }
  
  // shuffle the gstart out. The dtack is correct :-)
  memmove(_stack + start, _stack + start + 1, sizeof(tLogoInstruction));
  _tos--;

  return 0;
}

List Logo::newlist() {

  List l(&_lists);
  return l;
  
}

short Logo::startlist() {

  DEBUG_IN(Logo, "startlist");

  List list(&_lists);
  
  // push the fact that we have an open list.
  if (!push(SOPTYPE_OPENLIST, list.head(), list.tail())) {
    return LG_STACK_OVERFLOW;
  }
  
  return 0;
}

short Logo::endlist() {

  DEBUG_IN(Logo, "endlist");

  if (_stack[_tos-1][FIELD_OPTYPE] == SOPTYPE_OPENLIST) {
    // when a list ends, turn the stack top into an actual list.
    _stack[_tos-1][FIELD_OPTYPE] = OPTYPE_LIST;
  }

  return 0;
  
}

short Logo::dobuiltin() {

  DEBUG_IN(Logo, "dobuiltin");

  // save away where we will call to.
  short call = _pc;
  
  short op = instField(call, FIELD_OP);
  short arity = _corearity[op];
  
  if (arity == 0) {
    // call the builtin.
    callbuiltin(op);
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

short Logo::findbuiltin(LogoString *str, short start, short slen) {

  return findcrstring(&_corenames, str, start, slen);
  
}

short Logo::findcrstring(const LogoString *strings, const LogoString *stri, short strstart, short slen) {

  if (strings) {
//    strings->dump("findfixed", 0, strings->length());
    short len = strings->length();
    short start = 0;
    short index = 0;
    for (int i=0; i<len; i++) {
      if ((*strings)[i] == '\n') {
        if (slen == (i - start) && strings->ncmp2(stri, strstart, start, i-start) == 0) {
          return index;
        }
        start = i + 1;
        index++;
      }
    }
  }
  
  return -1;
  
}

short Logo::findfixed(const LogoString *stri, short strstart, short slen) const {

  DEBUG_IN_ARGS(Logo, "findfixed", "%i%i", strstart, slen);
  
  return findcrstring(_fixedstrings, stri, strstart, slen);

}

bool Logo::fixedcmp(const LogoString *stri, short strstart, short slen, tStrPool str, tStrPool len) const {

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

bool Logo::getfixedcr(const LogoString *strings, LogoStringResult *result, short ind) {

  if (strings) {
    short start = 0;
    short index = 0;
    short len = strings->length();
    for (int i=0; i<len; i++) {
      if ((*strings)[i] == '\n') {
        if (index == ind) {
          result->_fixed = strings;
          result->_fixedstart = start;
          result->_fixedlen = i-start;
          return true;
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
  
  if (str < _fixedcount) {
    return getfixedcr(_fixedstrings, result, str);
  }
  
//  DEBUG_RETURN(" %b", false);
  return false;
}

short Logo::addstring(const LogoString *str, short start, short slen) {

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

bool Logo::stringcmp(const LogoString *stri, short start, short slen, tStrPool str, tStrPool len) const {

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
  _variables[var]._valueopand = 0;
  return var;
  
}
void Logo::setintvar(short var, short n) {

  _variables[var]._type = OPTYPE_INT;
  _variables[var]._value = n;
  _variables[var]._valueopand = 0;
  
}

short Logo::newstringvar(short str, short slen, short vstr, short vlen) {

  if (str < 0 || vstr < 0) {
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
  _variables[var]._type = OPTYPE_STRING;
  _variables[var]._value = vstr;
  _variables[var]._valueopand = vlen;
  return var;
}

void Logo::setstringvar(short var, short vstr, short vlen) {

  _variables[var]._type = OPTYPE_STRING;
  _variables[var]._value = vstr;
  _variables[var]._valueopand = vlen;

}

short Logo::newdoublevar(short str, short slen, double n) {

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
  _variables[var]._type = OPTYPE_DOUBLE;
  splitdouble(n, & _variables[var]._value, &_variables[var]._valueopand);
  return var;

}

void Logo::setdoublevar(short var, double n) {

  _variables[var]._type = OPTYPE_INT;
  splitdouble(n, & _variables[var]._value, &_variables[var]._valueopand);

}

short Logo::newlistvar(short str, short slen, const List &l) {

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
  _variables[var]._type = OPTYPE_LIST;
  _variables[var]._value = l.head();
  _variables[var]._valueopand = l.tail();
  return var;

}

void Logo::setlistvar(short var, const List &l) {

  _variables[var]._type = OPTYPE_LIST;
  _variables[var]._value = l.head();
  _variables[var]._valueopand = l.tail();

}

bool Logo::varisint(short var) {
  return _variables[var]._type == OPTYPE_INT;
}

short Logo::varintvalue(short var) {
  if (varisint(var)) {
    return _variables[var]._value;
  }
  return 0;
}

bool Logo::getlistval(const ListNodeVal &val, LogoStringResult *str) {

  return parsestring(val.type(), val.data1(), val.data2(), str);

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
    if (!codeend && _code[jstart][FIELD_OPTYPE] == OPTYPE_NOOP) {
      codeend = jstart;
    }
    if (_code[jstart++][0] == OPTYPE_HALT) {
      break;
    }
  }
    
  short offset = (codeend + 1) - jstart;
  str << "static const short " << varname << "[][INST_LENGTH] PROGMEM = {" << endl;
  for (int i=0; i<codeend; i++) {
    dumpinstline(_code[i][FIELD_OPTYPE], _code[i][FIELD_OP], _code[i][FIELD_OPAND], offset, i, str);
  }
  str << "\t{ OPTYPE_HALT, 0, 0 },\t\t// " << codeend << endl;
  for (int i=jstart; _code[i][0] != OPTYPE_NOOP && i<CODE_SIZE; i++) {
    dumpinstline(_code[i][FIELD_OPTYPE], _code[i][FIELD_OP], _code[i][FIELD_OPAND], offset, i + offset, str);
  }
  if (compiler->haswords()) {
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
  str << "// words" << endl;
  compiler->dumpwordstrings(str);
  str << "// variables" << endl;
  dumpvarsstrings(compiler, str);
  str << "// strings" << endl;
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
  case OPTYPE_GSTART:
    str << "OPTYPE_GSTART";
    break;
  case OPTYPE_GEND:
    str << "OPTYPE_GEND";
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
  str << "\t{ SCOPTYPE_VAR, " << var._value << ", " << (short)var._valueopand << " }, " << endl;
  
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
      getbuiltinname(op, str, sizeof(str));
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
    case OPTYPE_GSTART:
      cout << "gstart";
      break;
    case OPTYPE_GEND:
      cout << "gend";
      break;
    case OPTYPE_LSTART:
      cout << "lstart";
      break;
    case OPTYPE_LEND:
      cout << "lend";
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
    case SOPTYPE_GSTART:
      cout << "(stack) gstart ";
      break;
    case SOPTYPE_OPENLIST:
    case OPTYPE_LIST:
      {
        cout << "(stack) list ";
        List l((ListPool *)&_lists, (tNodeType)op, (tNodeType)opand);
        cout << l.length();
      }
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
  dump(2, var._type, var._value, var._valueopand);
  
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
      compiler->dump(2, instField(i, FIELD_OPTYPE), instField(i, FIELD_OP), instField(i, FIELD_OPAND));
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
        compiler->dump(2, instField(i, FIELD_OPTYPE), instField(i, FIELD_OP), instField(i, FIELD_OPAND));
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

ostream &Logo::out() {
  if (_ostream) {
    return *_ostream;
  }
  return cout;
}

void Logo::setout(ostream *s) {
  _ostream = s;
}


#endif // !defined(ARDUINO)

void Logo::dumpstack(const LogoCompiler *compiler, bool all) const {

#ifdef ARDUINO
  Serial.print("stack: (");
  Serial.print(_tos);
  Serial.println(")");
  for (short i=0; i<(all ? MAX_STACK : _tos); i++) {
    Serial.print("\t");
    Serial.print(_stack[i][FIELD_OPTYPE]);
    Serial.print(",");
    Serial.print(_stack[i][FIELD_OP]);
    Serial.print(",");
    Serial.println(_stack[i][FIELD_OPAND]);
  }
#else
  cout << "stack: (" << _tos << ")" << endl;
  
  for (short i=0; i<(all ? MAX_STACK : _tos); i++) {
    if (compiler) {
      compiler->dump(1, _stack[i][FIELD_OPTYPE] ,_stack[i][FIELD_OP] ,_stack[i][FIELD_OPAND]);
    }
    else {
      dump(1, _stack[i][FIELD_OPTYPE], _stack[i][FIELD_OP] ,_stack[i][FIELD_OPAND]);
    }
    mark(i, _tos, "tos");
    cout << endl;
  }
#endif  
}

bool Logo::extractEvent(LogoString *s, LogoStringResult *name, LogoStringResult *data) {

  int len = s->length();
  if (len == 0) {
    return false;
  }
  
  int start = 0;
  if ((*s)[start++] != '[') {
    return false;
  }
  if ((*s)[start++] != '\"') {
    return false;
  }
  
  int end = start;
  while (end < len && (*s)[end] != '\"') {
    end++;
  }
  if (end >= len) {
    return false;
  }
  
  name->_fixed = s;
  name->_fixedstart = start;
  name->_fixedlen = end-start;
  
  end++;
  if ((*s)[end++] != ',') {
    return false;
  }
  if ((*s)[end] == '\"') {
  
    end++;
    start = end;
    while (end < len && (*s)[end] != '\"') {
      end++;
    }
  
    if (end >= len) {
      return false;
    }
  
    data->_fixed = s;
    data->_fixedstart = start;
    data->_fixedlen = end-start;
  }
  else if ((*s)[end] == '{') {
  
    start = end;
    while (end < len && (*s)[end] != '}') {
      end++;
    }
  
    if (end >= len) {
      return false;
    }
  
    data->_fixed = s;
    data->_fixedstart = start;
    data->_fixedlen = end-start+1;
  }
  
  return true;
}

