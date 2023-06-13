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
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
using namespace std;
using namespace boost::algorithm;
namespace fs = boost::filesystem;
#endif

#include <stdlib.h>
#include <ctype.h>
#include <math.h>

LogoCompiler::LogoCompiler(Logo * logo) :
  _logo(logo),
  _inword(false), _inwordargs(false), _defining(-1), _defininglen(-1), _wordarity(-1),
  _jump(NO_JUMP),
  _wordcount(0) {

#ifdef LOGO_SENTENCES
  _sentencecount = 0;
#endif
}

void LogoCompiler::reset() {

  DEBUG_IN(LogoCompiler, "reset");
  
#ifdef LOGO_SENTENCES
  _sentencecount = 0;
#endif
  _wordcount = 0;
  
}

void LogoCompiler::compileword(tJump *next, LogoString *stri, short wordstart, short wordlen, short op) {

  DEBUG_IN_ARGS(LogoCompiler, "compileword", "%i%i%i", wordstart, wordlen, op);

#ifdef LOGO_DEBUG  
  stri->dump("word", wordstart, wordlen);
#endif

  if (*next >= MAX_CODE) {
    _logo->outofcode();
    DEBUG_RETURN(" err ", 0);
    return;
  }
  
  short index = -1, category = 0;
  _logo->findbuiltin(stri, wordstart, wordlen, &index, &category);
  
  if (index >= 0) {
    _logo->addop(next, OPTYPE_BUILTIN, index, category);
    DEBUG_RETURN(" builtin ", 0);
    return;
  }

 index = findword(stri, wordstart, wordlen);
  if (index >= 0) {
   _logo->addop(next, OPTYPE_JUMP, _words[index]._jump, _words[index]._arity);
    DEBUG_RETURN(" word ", 0);
    return;
  }

  if (wordlen == 0) {
    return;
  }
  
  if ((*stri)[wordstart] == '!') {
    _logo->addop(next, OPTYPE_ERR, op);
    DEBUG_RETURN(" err ", 0);
    return;
  }
  
  if ((*stri)[wordstart] == ':') {
    short len = wordlen - 1;
    short str = _logo->addstring(stri, wordstart+1, len);
    if (str < 0) {
      _logo->addop(next, OPTYPE_ERR, LG_OUT_OF_STRINGS);
      DEBUG_RETURN(" mo nore strings %i", 0);
      return;
    }
    _logo->addop(next, OPTYPE_REF, str, len);
    return;
  }

  if (isnum(stri, wordstart, wordlen)) {
    _logo->addop(next, OPTYPE_INT, stri->toi(wordstart, wordlen));
    return;
  }

  if ((*stri)[wordstart] == '\"') {
    wordstart++;
    wordlen--;
  }
  
  op = _logo->addstring(stri, wordstart, wordlen);
  if (op < 0) {
    _logo->addop(next, OPTYPE_ERR, LG_OUT_OF_STRINGS);
    DEBUG_RETURN(" mo nore strings %i", 0);
    return;
  }

  _logo->addop(next, OPTYPE_STRING, op, wordlen);
  

}

bool LogoCompiler::isnum(LogoString *stri, short wordstart, short wordlen) {

  if (wordlen == 0) {
    return false;
  }
  for (short i=0; i<wordlen; i++) {
    if (!isdigit((*stri)[i + wordstart])) {
      return false;
    }
  }
  
  return true;
  
}

short LogoCompiler::findword(LogoString *str, short wordstart, short wordlen) const {

  for (short i=0; i<_wordcount; i++) {
    if (_logo->stringcmp(str, wordstart, wordlen, _words[i]._name, _words[i]._namelen)) {
      return i;
    }
  }
  
  return -1;
}

short LogoCompiler::findword(LogoStringResult *str) const {

  for (short i=0; i<_wordcount; i++) {
    if (_logo->stringcmp(str, _words[i]._name, _words[i]._namelen)) {
      return i;
    }
  }
  
  return -1;
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
  _words[_wordcount]._arity = arity;
  _wordcount++;

}

#ifdef LOGO_SENTENCES
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
  
  if (!start) {
    start = buf;
  }
  while (1) {
  
    const char *end = strstr(start + 1, "]");
    if (!end) {
      _logo->outofcode();
      return;
    }
  
    snprintf(_sentencebuf, sizeof(_sentencebuf), "&%d", _sentencecount);
    _sentencecount++;

    LogoSimpleString str1(_sentencebuf);
    short wlen = str1.length();
    short word = _logo->addstring(&str1, 0, wlen);
    if (word < 0) {
      _logo->error(LG_OUT_OF_STRINGS);
      return;
    }
    
    // how much to go?
    short endlen = strlen(end);
    
    // remember where we are before compiling.
    short jump = _logo->_nextjcode;
    
    // add all the words etc. They are all placed as jumps.
    LogoSimpleString str(start + 1, end - start - 1);
    compilewords(&str, 0, str.length(), false);
    
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
#endif // LOGO_SENTENCES

void LogoCompiler::compile(LogoString *str) {

  DEBUG_IN(LogoCompiler, "compile");
  
  short len = str->length();
  short nextline = 0;
  short linestart, linelen;
  while (nextline >= 0) {
    nextline = scanfor(&linestart, &linelen, str, nextline, (len-nextline), true);
    
    if (linelen == 0) {
      break;
    }
#ifdef LOGO_DEBUG  
    str->dump("line", linestart, linelen);
#endif

    // skip comments.
    if ((*str)[linestart] == '#') {
      continue;
    }
    
#ifdef LOGO_SENTENCES
    // build and replace sentences in the line.
    if ((*str)[linestart] == '[') {
      // replacing sentences COULD modify the line.
      str->ncpy(_linebuf, linestart, linelen);
//      cout << "compile line before " << _linebuf << endl;
      dosentences(_linebuf, strlen(_linebuf), 0);
//      cout << "compile line after " << _linebuf << endl;
      short len = strlen(_linebuf);
      LogoSimpleString str2(_linebuf, len);
      compilewords(&str2, 0, len, true);
    }
    else {
#endif
      compilewords(str, linestart, linelen, true);
#ifdef LOGO_SENTENCES
    }
#endif
  }

}

void LogoCompiler::compilewords(LogoString *str, short start, short len, bool define) {

  DEBUG_IN_ARGS(LogoCompiler, "compilewords", "%i%i%b", start, len, define);
  
#ifdef LOGO_DEBUG  
  str->dump("compiling", start, len);
#endif
  
  short nextword = start;
  short wordstart, wordlen;
  while (nextword >= 0) {
  
#ifdef LOGO_SENTENCES
    if ((*str)[nextword] == '[') {
      str->ncpy(_linebuf, nextword, (start+len)-nextword);
//      cout << "compilewords line before " << _linebuf << endl;
      dosentences(_linebuf, strlen(_linebuf), 0);
//      cout << "compilewords line after " << _linebuf << endl;
      short len = strlen(_linebuf);
      LogoSimpleString str2(_linebuf, len);
      compilewords(&str2, 0, len, true);
      return;
    }
#endif    
    nextword = scanfor(&wordstart, &wordlen, str, nextword, (start+len)-nextword, false);
    if (define) {
      if (!dodefine(str, wordstart, wordlen, nextword == -1)) {
        if (_logo->_nextcode >= START_JCODE) {
          _logo->outofcode();
          return;
        }
        compileword(&_logo->_nextcode, str, wordstart, wordlen, 0);
      }
    }
    else {
      if (_logo->_nextjcode >= MAX_CODE) {
        _logo->outofcode();
        return;
      }
      compileword(&_logo->_nextjcode, str, wordstart, wordlen, 0);
    }
  }
  
}

bool LogoCompiler::dodefine(LogoString *str, short wordstart, short wordlen, bool eol) {

  DEBUG_IN_ARGS(LogoCompiler, "dodefine", "%i%i%b%b%b", wordstart, wordlen, _inword, _inwordargs, eol);
  
#ifdef LOGO_DEBUG  
  str->dump("defining", wordstart, wordlen);
#endif
  
  if (wordlen == 0) {
    DEBUG_RETURN(" empty word %b", false);
    return false;
  }
  
  if (str->ncmp("BUILTIN", wordstart, wordstart+7) == 0) {
    _logo->setprimitives(str, wordstart+8, wordlen-9);
    DEBUG_RETURN(" builtin def %b", true);
    return true;
  }

  if (str->ncmp("TO", wordstart, wordlen) == 0) {
    _inword = true;
    _inwordargs = false;
    _wordarity = 0;
    DEBUG_RETURN(" in define %b", true);
    return true;
  }

  if ((*str)[wordstart] == '[') {
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
  
      _defininglen = wordlen;
      _defining = _logo->addstring(str, wordstart, wordlen);
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
    else if (str->ncmp("END", wordstart, wordlen) == 0) {
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
    if ((*str)[wordstart] == ':') {
      short var = _logo->findvariable(str, wordstart+1, wordlen-1);
      if (var < 0) {
        var = _logo->newintvar(_logo->addstring(str, wordstart+1, wordlen-1), wordlen-1, 0);
      }
      else {
        _logo->setintvar(var, 0);
      }
      _logo->addop(&_logo->_nextjcode, OPTYPE_POPREF, var);
      _wordarity++;
      if (eol) {
        _inwordargs = false;
      }
      DEBUG_RETURN(" %b", true);
      return true;
    }
    _logo->error(LG_EXTRA_IN_DEFINE);
    DEBUG_RETURN(" too many in define %b", true);
    return true;
  }
  
  compileword(&_logo->_nextjcode, str, wordstart, wordlen, 0);
  
  DEBUG_RETURN(" %b", true);
  return true;

}

bool LogoCompiler::istoken(char c, bool newline) {

//  DEBUG_IN_ARGS(Logo, "istoken", "%c%b", c, newline);
  
  return newline ? c == ';' || c == '\n' : c == ' ';
  
}

short LogoCompiler::scanfor(short *strstart, short *strsize, LogoString *str, short start, short len, bool newline) {

  DEBUG_IN_ARGS(LogoCompiler, "scanfor", "%i%i%b", start, len, newline);
  
#ifdef LOGO_DEBUG  
  str->dump("scanning", start, len);
#endif  
  // skip ws
  while (start < (start+len) && ((*str)[start] == ' ' || (*str)[start] == '\t')) {
    start++;
  }
  
  short end = start;
  
  // find end of line
  while (end < (start+len) && (*str)[end] && !istoken((*str)[end], newline)) {
    end++;
  }
  
  if (end < (start+len-1)) {
    if (istoken((*str)[end], newline)) {
      end++;
      *strstart = start;
      *strsize = end-start-1;
    }
    else {
      DEBUG_RETURN(" token not found %i", -1);
      return -1;
    }
  }
  else {
    *strstart = start;
    *strsize = end-start;
    DEBUG_RETURN(" end of string %i%i%i", start, end, -1);
    return -1;
  }

  DEBUG_RETURN(" %i", end);
  return end;

}

#ifndef ARDUINO

void LogoStaticPrimitives::exec(short index, Logo *logo) {

  char s[LINE_LEN];
  name(index, s, sizeof(s));
  int ar = arity(index);
  cout << s;
  while (ar) {
    cout << " " << logo->popint();
    ar--;
  }
  cout << endl;
  
}

short LogoStaticPrimitives::find(LogoString *stri, short wordstart, short wordlen) {

#ifdef LOGO_DEBUG
  stri->dump("find", wordstart, wordlen);
  cout << _builtindef << endl;
#endif
  
  short i = 0;
  const char *end = _builtindef;
  while (*end) {
    const char *start = end;
    while (*end && !(*end == ',' || *end == ':')) {
      end++;
    }
    int len = end - start;
    if (len == wordlen && stri->ncmp(start, wordstart, wordlen) == 0) {
      return i;
    }
    if (*end) {
      if (*end == ':') {
        end += 2;
      }
      if (*end) {
        end++;
      }
    }
    i++;
  }
  
  return -1;
}

void LogoStaticPrimitives::str(short index, char *s, int len) {

  short i = 0;
  const char *end = _builtindef;
  while (*end) {
    const char *start = end;
    while (*end && *end != ',') {
      end++;
    }
    if (i == index) {
      int l = end-start;
      strncpy(s, start, l);
      s[l] = 0;
      return;
    }
    if (*end) {
      end++;
    }
    i++;
  }
  *s = 0;

}

void LogoStaticPrimitives::name(short index, char *s, int len) {

  str(index, s, len);
  char *colon = strchr(s, ':');
  if (colon) {
    *colon = 0;
  }
  
}

short LogoStaticPrimitives::arity(short index) {
  
  char s[LINE_LEN];
  str(index, s, sizeof(s));
  char *colon = strchr(s, ':');
  if (colon) {
    return atoi(colon+1);
  }
  return 0;
  
}

void LogoStaticPrimitives::set(LogoString *str, short start, short slen) {
  str->ncpy(_builtindef, start, slen);
}

void LogoCompiler::dumpwordscode(short offset, ostream &str) const {

  if (!_wordcount) {
    return;
  }
  
  char name[LINE_LEN];
  for (short i=0; i<_wordcount; i++) {
    LogoStringResult result;
    _logo->getstring(&result, _words[i]._name, _words[i]._namelen);
    result.ncpy(name, sizeof(name));
    str << "\t{ SCOPTYPE_WORD, " << (short)_words[i]._jump + offset << ", " << (short)_words[i]._arity << " }, " << endl;
  }
  
  _logo->dumpvarscode(this, str);
  
}

void LogoCompiler::entab(short indent) const {
  for (short i=0; i<indent; i++) {
    cout << "\t";
  }
}

void LogoCompiler::printwordstring(const LogoWord &word, ostream &str) const {
  char name[LINE_LEN];
  LogoStringResult result;
  _logo->getstring(&result, word._name, word._namelen);
  result.ncpy(name, sizeof(name));
  str << "\t\"" << name << "\\n\"";
}

void LogoCompiler::dumpwordstrings(ostream &str) const {

  if (!_wordcount) {
    return;
  }
  
  for (short i=0; i<_wordcount; i++) {
    printwordstring(_words[i], str);
    str << endl;
  }
  
}

int LogoCompiler::wordstringslist(char *buf, int len) const {

  if (!_wordcount) {
    return 0;
  }
  
  for (short i=0; i<_wordcount; i++) {
    if (!_logo->safestringcat(_words[i]._name, _words[i]._namelen, buf, len)) {
      return -1;
    }
  }
  
  return _wordcount;
  
}

int LogoCompiler::compile(fstream &file) {
  file.clear();
  file.seekg(0, ios::beg);
  string line;
  while (getline(file, line)) {
    line += "\n";
    compile(line.c_str());
    int err = _logo->geterr();
    if (err) {
      return err;
    }
  }
  return 0;
}

int LogoCompiler::generatecode(fstream &file, const string &name, ostream &str) {

  // first compile the code so we can dump the strings from it in 
  // the correct order
  LogoStaticPrimitives primitives;
  Logo logo(&primitives, 0, Logo::core);
  LogoCompiler compiler(&logo);
  compiler.compile(file);
  int err = logo.geterr();
  if (err) {
    return err;
  }
  
  // can dump the strings code.
  logo.dumpstringscode(&compiler, ("strings_" + name).c_str(), str);

  // now gather the strings and specify them for the new compile
  char list[STRING_POOL_SIZE];
  list[0] = 0;
  int count = logo.stringslist(&compiler, list, sizeof(list));
  if (count < 0) {
    cout << "buffer overflow generating strings list" << endl;
    return 1;
  }
  LogoSimpleString strings(list);
  {
    // build a new engine with these strings and compile again.
    LogoStaticPrimitives primitives2;
    Logo logo2(&primitives2, 0, Logo::core, &strings);
    LogoCompiler compiler2(&logo2);
    compiler2.compile(file);
    int err = logo2.geterr();
    if (err) {
      return err;
    }
    
    // ready to dump the code now.
    logo2.dumpinst(&compiler2, ("code_" + name).c_str(), str);
  }
  
  return 0;
  
}

int LogoCompiler::includelgo(const string &infn, const string &name, fstream &outfile) {

  fstream file;
  file.open(infn, ios::in);
  if (!file) {
    cout << "File not found " << infn << endl;
    return 1;
  }
  
  stringstream str;
  int err = LogoCompiler::generatecode(file, name, str);
  file.close();
  if (err) {
    return err;
  }
  
  outfile << str.str();
  
  return 0;
  
}

int LogoCompiler::updateino(const std::string &infn, std::fstream &infile, std::fstream &outfile) {

  const string logo_directive = "//#LOGO";
  const string file_directive = "FILE=";
  const string endfile_directive = "ENDFILE";
  const string name_directive = "NAME=";

  string line;
  bool including = false;
  while (getline(infile, line)) {
    size_t pos = line.find(logo_directive);
    if (pos != string::npos) {
      string rest = line.substr(pos + logo_directive.length());
      trim(rest);
      pos = rest.find(file_directive);
      if (pos != string::npos) {
        rest = rest.substr(pos + file_directive.length());
        trim(rest);
        pos = rest.find(name_directive);
        if (pos != string::npos) {
          string lfn = rest.substr(0, pos);
          trim(lfn);
          fs::path lgopath(lfn);
          fs::path inpath(infn);
          if (!lgopath.is_absolute()) {
            lgopath = inpath.parent_path() / lgopath;
          }
          rest = rest.substr(pos + name_directive.length());
          trim(rest);
          including = true;
          outfile << line << endl;
          int err = includelgo(lgopath.string(), rest, outfile);
          if (err) {
            return err;
          }
        }
      }
      else {
        pos = rest.find(endfile_directive);
        if (pos != string::npos) {
          outfile << line << endl;
          including = false;
        }
      }
    }
    else {
      if (!including) {
        outfile << line << endl;
      }
    }
  }
  
  return 0;

}

void LogoCompiler::printword(const LogoWord &word) const {

  char name[LINE_LEN];
  LogoStringResult result;
  _logo->getstring(&result, word._name, word._namelen);
  result.ncpy(name, sizeof(name));
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

  if (type == OPTYPE_JUMP) {
    entab(indent);
    searchword(op);
  }
  else {
    _logo->dump(indent, type, op, opand);
 }

}

void LogoCompiler::markword(tJump jump) const {

  LogoStringResult result;
  for (short i=0; i<_wordcount; i++) {
    _logo->getstring(&result, _words[i]._name, _words[i]._namelen);
    short word = findword(&result);
    if (word >= 0) {
      if (_words[word]._jump == jump) {
        char name[LINE_LEN];
        result.ncpy(name, sizeof(name));
        cout << name;
      }
    }
  }
  
}

void LogoCompiler::printvar(const LogoVar &var) const {

  char name[LINE_LEN];
  LogoStringResult result;
  _logo->getstring(&result, var._name, var._namelen);
  result.ncpy(name, sizeof(name));
  cout << name;
  dump(2, var._type, var._value, var._valuelen);
  
}

void LogoCompiler::dump(const char *msg, bool all) const {
  cout << msg << endl;
  dump(all);
}

void LogoCompiler::dump(bool all) const {

  cout << "------" << endl;
  _logo->dumpcode(this, all);
  _logo->dumpstack(this, all);
  _logo->dumpvars(this);
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

#endif // !defined(ARDUINO)
