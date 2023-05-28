/*
  logocompiler.cpp
    
  Author: Paul Hamilton (paul@visualops.com)
  Date: 5-May-2023
  
  This work is licensed under the Creative Commons Attribution 4.0 International License. 
  To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/ or 
  send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  https://github.com/visualopsholdings/tinylogo
*/

#include "logocompiler.hpp"

#ifdef LOGO_DEBUG
#ifdef ARDUINO
#include "arduinodebug.hpp"
#else
#include "test/debug.hpp"
void LogoCompiler::outstate() const {
  cout << endl;
}
#endif // ARDUINO
#else
#include "nodebug.hpp"
#endif

#ifndef ARDUINO
#include <iostream>
using namespace std;
#endif

#include <stdlib.h>
#include <ctype.h>

LogoCompiler::LogoCompiler(Logo * logo) :
  _logo(logo),
  _inword(false), _inwordargs(false), _defining(-1), _defininglen(-1), _wordarity(-1),
  _jump(NO_JUMP),
  _sentencecount(0), _wordcount(0) {

}

void LogoCompiler::reset() {

  DEBUG_IN(LogoCompiler, "reset");
  
  _sentencecount = 0;
  _wordcount = 0;
  
}

void LogoCompiler::compileword(tJump *next, const char *word, short op) {

  DEBUG_IN_ARGS(LogoCompiler, "compileword", "%s%i", word, op);
  
  if (*next >= MAX_CODE) {
    _logo->outofcode();
    DEBUG_RETURN(" err ", 0);
    return;
  }
  
  short index, category;
  _logo->findbuiltin(word, &index, &category);
  
  if (index >= 0) {
    _logo->addop(next, OPTYPE_BUILTIN, index, category);
    DEBUG_RETURN(" builtin ", 0);
    return;
  }

  index = findword(word);
  if (index >= 0) {
   _logo->addop(next, OPTYPE_JUMP, _words[index]._jump, _words[index]._arity);
    DEBUG_RETURN(" word ", 0);
    return;
  }

  short len = strlen(word);
  if (len == 0) {
    return;
  }
  
  if (word[0] == '!') {
    _logo->addop(next, OPTYPE_ERR, op);
    DEBUG_RETURN(" err ", 0);
    return;
  }
  
#ifdef HAS_VARIABLES
  if (word[0] == ':') {
    short len = strlen(word+1);
    short str = _logo->addstring(word+1, len);
    if (str < 0) {
      _logo->addop(next, OPTYPE_ERR, LG_OUT_OF_STRINGS);
      DEBUG_RETURN(" mo nore strings %i", 0);
      return;
    }
    _logo->addop(next, OPTYPE_REF, str, len);
    return;
  }
#endif

  if (isnum(word, len)) {
    _logo->addop(next, OPTYPE_INT, atoi(word));
    return;
  }

  const char *start = word;
  if (word[0] == '\"') {
    start++;
    len--;
  }
  
  op = _logo->addstring(start, len);
  if (op < 0) {
    _logo->addop(next, OPTYPE_ERR, LG_OUT_OF_STRINGS);
    DEBUG_RETURN(" mo nore strings %i", 0);
    return;
  }

  _logo->addop(next, OPTYPE_STRING, op, len);

}

bool LogoCompiler::isnum(const char *word, short len) {

  if (len == 0) {
    return false;
  }
  for (short i=0; i<len; i++) {
    if (!isdigit(word[i])) {
      return false;
    }
  }
  
  return true;
  
}

short LogoCompiler::findword(const char *word) const {

  short len = strlen(word);
  for (short i=0; i<_wordcount; i++) {
    if (_logo->stringcmp(word, len, _words[i]._name, _words[i]._namelen)) {
      return i;
    }
  }
  
  return -1;
}

void LogoCompiler::dosentences(char *buf, short len, const char *start) {

  DEBUG_IN(LogoCompiler, "dosentences");
  
  // we preprocess all sentences in a line first. We basically treat
  // sentences like lambdas (anonymous words). Each one has a unique name
  // like &N and we define them and then replace sentence in the line
  // with the word name of the sentence.
  //
  //  so
  //
  //    REPEAT 10 [SOMETHING TO DO]
  //
  //  becomes
  //
  //    REPEAT 10 &0
  //
  //  where
  //
  //    &0 is
  //      string SOMETHING
  //      string TO
  //      string DO
  
  while (1) {
  
    const char *end = strstr(start + 1, "]");
    if (!end) {
      _logo->outofcode();
      return;
    }
  
    snprintf(_sentencebuf, sizeof(_sentencebuf), "&%d", _sentencecount);
    _sentencecount++;

    short wlen = strlen(_sentencebuf);
    short word = _logo->addstring(_sentencebuf, wlen);
    if (word < 0) {
      _logo->error(LG_OUT_OF_STRINGS);
      return;
    }
    
    // how much to go?
    short endlen = strlen(end);
    
    // remember where we are before compiling.
    short jump = _logo->_nextjcode;
    
    // add all the words etc.
    LogoSimpleString str(start + 1, end - start - 1);
    compilewords(&str, str.length(), false);
    
    // and finish off the word
    finishword(word, wlen, jump, 0);
  
    // replace sentence in the original string.
    memmove((char *)start, _sentencebuf, wlen);
    memmove((char *)(start + wlen), end + 1, endlen);
    
    if (endlen > 1) {
      start = strstr(buf, "[");
      if (!start) {
        break;
      }
      end = strstr(start + 1, "]");
      if (!end) {
        _logo->outofcode();
        return;
      }
    }
    else {
      break;
   }
  }
}

void LogoCompiler::compile(LogoString *str) {

  DEBUG_IN(LogoCompiler, "compile");
  
  short len = str->length();
  short nextline = 0;
  while (nextline >= 0) {
    nextline = scanfor(_linebuf, sizeof(_linebuf), str, len, nextline, true);
    
    if (nextline == -2) {
      _logo->error(LG_LINE_TOO_LONG);
      break;
    }
    if (!_linebuf[0]) {
      break;
    }
    
    // build and replace sentences in the line.
    const char *s = strstr(_linebuf, "[");
    if (s) {
      dosentences(_linebuf, strlen(_linebuf), s);
    }
    
    short len = strlen(_linebuf);
    LogoSimpleString str(_linebuf, len);
    compilewords(&str, len, true);
  }

}

void LogoCompiler::compilewords(LogoString *str, short len, bool define) {

  DEBUG_IN_ARGS(LogoCompiler, "compilewords", "%i%b", len, define);
  
  short nextword = 0;
  while (nextword >= 0) {
    nextword = scanfor(_wordbuf, sizeof(_wordbuf), str, len, nextword, false);
    if (nextword == -2) {
      _logo->error(LG_WORD_TOO_LONG);
      return;
    }
    if (define) {
      if (!dodefine(_wordbuf, nextword == -1)) {
        if (_logo->_nextcode >= START_JCODE) {
          _logo->outofcode();
          return;
        }
        compileword(&_logo->_nextcode, _wordbuf, 0);
      }
    }
    else {
      if (_logo->_nextjcode >= MAX_CODE) {
        _logo->outofcode();
        return;
      }
      compileword(&_logo->_nextjcode, _wordbuf, 0);
    }
  }
  
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
    _logo->outofcode();
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

bool LogoCompiler::istoken(char c, bool newline) {

//  DEBUG_IN_ARGS(Logo, "istoken", "%c%b", c, newline);
  
  return newline ? c == ';' || c == '\n' : c == ' ';
  
}

short LogoCompiler::scanfor(char *s, short size, LogoString *str, short len, short start, bool newline) {

  DEBUG_IN_ARGS(LogoCompiler, "scanfor", "%i%i%i%b", size, len, start, newline);
  
  // skip ws
  while (start < len && ((*str)[start] == ' ' || (*str)[start] == '\t')) {
    start++;
  }
  
  short end = start;
  
  // find end of line
  for (short i=0; end < len && (*str)[end] && !istoken((*str)[end], newline); i++) {
    end++;
  }
  
  if ((end - start) > size) {
    DEBUG_RETURN(" too long ", -2);
    return -2;
  }
  
  if (end < len) {
    if (istoken((*str)[end], newline)) {
      end++;
      str->ncpy(s, start, end-start-1);
      s[end-start-1] = 0;
    }
    else {
      DEBUG_RETURN(" token not found %i", -1);
      return -1;
    }
  }
  else {
    str->ncpy(s, start, end-start);
    s[end-start] = 0;
    DEBUG_RETURN(" end of string %i", -1);
    return -1;
  }
  
  DEBUG_RETURN(" %i", end);
  return end;

}

#ifndef ARDUINO
void LogoCompiler::dumpwordscode(short offset) const {

  if (!_wordcount) {
    return;
  }
  
  char name[32];
  for (short i=0; i<_wordcount; i++) {
    _logo->getstring(name, sizeof(name), _words[i]._name, _words[i]._namelen);
    cout << "\t{ SCOPTYPE_WORD, " << (short)_words[i]._jump + offset << ", " << (short)_words[i]._arity << " }, " << endl;
  }
  
}

void LogoCompiler::entab(short indent) const {
  for (short i=0; i<indent; i++) {
    cout << "\t";
  }
}

void LogoCompiler::printwordcode(const LogoWord &word) const {
  char name[32];
  _logo->getstring(name, sizeof(name), word._name, word._namelen);
  cout << "\t\"" << name << "\\n\"";
}

#ifdef HAS_VARIABLES
void LogoCompiler::printvarcode(const LogoVar &var) const {

  char name[32];
  _logo->getstring(name, sizeof(name), var._name, var._namelen);
  cout << "\t\"" << name << "\\n\"";

}
#endif // HAS_VARIABLES

void LogoCompiler::dumpwordnamescode() const {

  if (!_wordcount) {
    return;
  }
  
  for (short i=0; i<_wordcount; i++) {
    printwordcode(_words[i]);
    cout << endl;
  }
  
}
#endif // ARDUINO

#if defined(LOGO_DEBUG) && !defined(ARDUINO)

void LogoCompiler::dumpwordnames() const {

  cout << "words: " << endl;
  
  if (!_wordcount) {
    entab(1);
    cout << "empty" << endl;
    return;
  }
  
  for (short i=0; i<_wordcount; i++) {
    entab(1);
    printword(_words[i]);
    cout << endl;
  }
  
}

void LogoCompiler::printword(const LogoWord &word) const {
  char name[32];
  _logo->getstring(name, sizeof(name), word._name, word._namelen);
  cout << name << " (" << (short)word._jump << ")";
  if (word._arity) {
    cout << " " << (short)word._arity;
  }
}

void LogoCompiler::searchword(short op) const {

  for (short i=0; i<_wordcount; i++) {
    if (_words[i]._jump == op) {
      printword(_words[i]);
      return;
    }
  }
  
}

void LogoCompiler::dump(short indent, short type, short op, short opand) const {

  entab(indent);
  char str[128];
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
      cout << "builtin " << _logo->getbuiltin(op, opand)->_name;
      break;
    case OPTYPE_JUMP:
      searchword(op);
      break;
    case OPTYPE_STRING:
      _logo->getstring(str, sizeof(str), op, opand);
      cout << "string " << str;
      break;
    case OPTYPE_INT:
      cout << "integer " << op;
      break;
    case OPTYPE_DOUBLE:
      cout << "double " << _logo->joindouble(op, opand);
      break;
#ifdef HAS_VARIABLES
    case OPTYPE_REF:
      _logo->getstring(str, sizeof(str), op, opand);
      cout << "ref " << str;
      break;
    case OPTYPE_POPREF:
       cout << "pop ref " << op;
      break;
#endif
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
    case SOPTYPE_CONDRET:
      cout << "(stack) cond ret to " << op;
      break;
    case SOPTYPE_SKIP:
      cout << "(stack) skip ";
      break;
    default:
      cout << "unknown optype " << type;
  }

}

void LogoCompiler::markword(tJump jump) const {

  for (short i=0; i<_wordcount; i++) {
    char name[WORD_LEN];
    _logo->getstring(name, sizeof(name), _words[i]._name, _words[i]._namelen);
    short word = findword(name);
    if (word >= 0) {
      if (_words[word]._jump == jump) {
         cout << name;
      }
    }
  }
  
}

#ifdef HAS_VARIABLES
void LogoCompiler::printvar(const LogoVar &var) const {

  char name[32];
  _logo->getstring(name, sizeof(name), var._name, var._namelen);
  cout << name;
  dump(2, var._type, var._value, var._valuelen);
  
}
#endif

void LogoCompiler::dump(const char *msg, bool all) const {
  cout << msg << endl;
  dump(all);
}

void LogoCompiler::dump(bool all) const {

  cout << "------" << endl;
  dumpwordnames();
  _logo->dumpcode(this, all);
  _logo->dumpstack(this, all);
#ifdef HAS_VARIABLES
  _logo->dumpvars(this);
#endif  
}

void LogoCompiler::mark(short i, short mark, const char *name) const {
  if (i == mark) {
    entab(2);
    cout << "(" << name << ")";
  }
}

short LogoCompiler::stepdump(short n, bool all) {
  dump(false);
  for (short i=0; i<n; i++) {
    cout << "step " << i << " -----------" << endl;
    short ret = _logo->step();
    if (ret) {
      return ret;
    }
    dump(all);
  }
  return 0;
}

#endif // defined(LOGO_DEBUG) && !defined(ARDUINO)