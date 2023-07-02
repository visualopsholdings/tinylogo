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
}

void LogoCompiler::reset() {

  DEBUG_IN(LogoCompiler, "reset");
  
  _wordcount = 0;
  
}

int LogoCompiler::callword(const char *word) {

  DEBUG_IN_ARGS(LogoCompiler, "callword", "%s", word);
  
  LogoSimpleString s(word);
  for (short i=0; i<_wordcount; i++) {
    if (_logo->stringcmp(&s, 0, s.length(), _words[i]._name, _words[i]._namelen)) {
      _logo->backup();
      _logo->call(_words[i]._jump+1, _words[i]._arity);
      return 0;
    }
  }
  
  return LG_WORD_NOT_FOUND;

}

void LogoCompiler::compileword(tJump *next, LogoString *stri, short wordstart, short wordlen, short op) {

  DEBUG_IN_ARGS(LogoCompiler, "compileword", "%i%i%i", wordstart, wordlen, op);

#ifdef LOGO_DEBUG  
  stri->dump("word", wordstart, wordlen);
#endif

  if (wordlen == 0) {
    return;
  }
    
  if (*next >= MAX_CODE) {
    _logo->outofcode();
    DEBUG_RETURN(" err ", 0);
    return;
  }
  
  if ((*stri)[wordstart] == '(') {
    _logo->addop(next, OPTYPE_GSTART);
    DEBUG_RETURN(" gstart ", 0);
    return;
  }
  
  if ((*stri)[wordstart] == ')') {
    _logo->addop(next, OPTYPE_GEND);
    DEBUG_RETURN(" gend ", 0);
    return;
  }
  
  if ((*stri)[wordstart] == '[') {
    _logo->addop(next, OPTYPE_LSTART);
    DEBUG_RETURN(" lstart ", 0);
    return;
  }
  
  if ((*stri)[wordstart] == ']') {
    _logo->addop(next, OPTYPE_LEND);
    DEBUG_RETURN(" lend ", 0);
    return;
  }
  
  short index =_logo->findbuiltin(stri, wordstart, wordlen);
  
  if (index >= 0) {
    _logo->addop(next, OPTYPE_BUILTIN, index);
    DEBUG_RETURN(" builtin ", 0);
    return;
  }

  index = findword(stri, wordstart, wordlen);
  if (index >= 0) {
   _logo->addop(next, OPTYPE_JUMP, _words[index]._jump, _words[index]._arity);
    DEBUG_RETURN(" word ", 0);
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

  if (_logo->isnum(stri, wordstart, wordlen)) {
    if (stri->find('.', wordstart, wordlen) > 0) {
      double n = stri->tof(wordstart, wordlen);
      short op, opand;
      _logo->splitdouble(n, &op, &opand);
      _logo->addop(next, OPTYPE_DOUBLE, op, opand);
    }
    else {
      _logo->addop(next, OPTYPE_INT, stri->toi(wordstart, wordlen));
    }
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


void LogoCompiler::compile(LogoString *str) {

  DEBUG_IN(LogoCompiler, "compile");
  
  short len = str->length();
  short nextline = 0;
  short linestart, linelen;
  while (nextline >= 0) {
    nextline = scan(&linestart, &linelen, str, len, nextline, true);
    
    if (linelen == 0) {
      break;
    }
#ifdef LOGO_DEBUG  
    str->dump("line", linestart, linelen);
#endif

    // skip comments.
    if ((*str)[linestart] == '#') {
      DEBUG_OUT(" comment", 0);
      continue;
    }
    
    compilewords(str, linestart, linelen, true);
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
  
    nextword = scan(&wordstart, &wordlen, str, start+len, nextword, false);
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
  
  if (str->ncasecmp("TO", wordstart, wordlen) == 0) {
    _inword = true;
    _inwordargs = false;
    _wordarity = 0;
    DEBUG_RETURN(" in define %b", true);
    return true;
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
    else if (str->ncasecmp("END", wordstart, wordlen) == 0) {
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
      wordstart++;
      wordlen--;
    }
    short var = _logo->findvariable(str, wordstart, wordlen);
    if (var < 0) {
      var = _logo->newintvar(_logo->addstring(str, wordstart, wordlen), wordlen, 0);
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
  
  compileword(&_logo->_nextjcode, str, wordstart, wordlen, 0);
  
  DEBUG_RETURN(" %b", true);
  return true;

}

bool LogoCompiler::switchtoken(char prevc, char c, bool newline) {

//  DEBUG_IN_ARGS(Logo, "switchtoken", "%c%b%b", c, newline, wasident);
  
  if (newline) {
    return strchr(";\n", c);
  }

  // identifiers can have . in them like MY.VAR
  if (isalnum(c) || c == '.') {
    // above is transitive and also :A and "A 
    return !(isalnum(prevc) || prevc == '.') && !strchr(":\"", prevc);
  }
  
  // != >= <=
  return !strchr("!><", prevc) || c != '=';
  
}

short LogoCompiler::scan(short *strstart, short *strsize, LogoString *str, short len, short start, bool newline) {

  DEBUG_IN_ARGS(LogoCompiler, "scan", "%i%i%b", start, len, newline);
  
#ifdef LOGO_DEBUG  
  str->dump("scanning", start, len-start);
#endif  

  if ((len - start) == 0) {
    *strsize = 0;
    DEBUG_RETURN(" nothing to scan %i", -1);
    return -1;
  }

  // skip ws
  while (start < len && isspace((*str)[start])) {
    start++;
  }
  
  if (start >= len) {
    *strstart = start;
    *strsize = 0;
    DEBUG_RETURN(" all whitespace %i", -1);
    return -1;
  }
  
  short end = start;
  
  // save away the token we are at
  char prevc = (*str)[end];
  
  // go 1 more.
  end++;
  
  if (end >= (start+len)) {
    *strstart = start;
    *strsize = 0;
    DEBUG_RETURN(" found end at start %i", -1);
    return -1;
  }
  
  // span quotes.
  if ((*str)[start] == '\"') {
    DEBUG_OUT("a quote", 0);
    // we leave the quote in to allow us to know that it can't be a number
    // if it's a quoted number.
    *strstart = start;
    end = start + 1;
    while (end < len && (*str)[end] != '\"' && !isspace((*str)[end])) {
      // allow chars to be escaped.
      if ((*str)[end] == '\\') {
        end++;
      }
      end++;
    }
    *strsize = end-start;
    if (end < len) {
      end++;
    }
    DEBUG_RETURN(" in quotes %i%i", end, *strsize);
    return end;
  }
  
  // find a place where we switch to a new type of token.
  while (end < (start+len) && (*str)[end] && !switchtoken(prevc, (*str)[end], newline)) {
    end++;
  }
  
  *strstart = start;
  
  if (newline || isspace((*str)[end])) {
    DEBUG_OUT("newline or space", 0);
    end++;
    *strsize = end-start-1;
    if (end >= len) {
      end = -1;
    }
    DEBUG_RETURN(" newline or space %i%i", end, *strsize);
    return end;
  }

  *strsize = end-start;
  
  if (end >= len) {
    DEBUG_RETURN(" found end %i", -1);
    return -1;
  }
  
  DEBUG_RETURN(" %i%i", end, *strsize);
  return end;
    
}

#ifndef ARDUINO

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

int LogoCompiler::inlinelgo(std::fstream &file, const std::map<std::string, std::string> &directives, std::ostream &str) {

  // check for name directive
  map<string, string>::const_iterator name = directives.find("NAME");
  if (name == directives.end()) {
    return LG_OUT_OF_STRINGS;
  }
  
  str << "static const char program_" << name->second << "[] PROGMEM = " << endl;

  file.clear();
  file.seekg(0, ios::beg);
  string line;
  
  while (getline(file, line)) {
    replacedirectives(&line, directives, false, 0);
    replace_all(line, "\"", "\\\"");
    str << "\"" << line << "\\n\"" << endl;
  }
  str << ";" << endl;
  
  return 0;
  
}


int LogoCompiler::compile(fstream &file, const map<string, string> &directives, bool autoassign) {
  file.clear();
  file.seekg(0, ios::beg);
  string line;
  
  int n = 0;
  while (getline(file, line)) {
    line += "\n";
    replacedirectives(&line, directives, autoassign, &n);
    compile(line.c_str());
    int err = _logo->geterr();
    if (err) {
      return err;
    }
  }
  return 0;
}

int LogoCompiler::generatecode(fstream &file, const map<string, string> &directives, ostream &str) {

  // check for name directive
  map<string, string>::const_iterator name = directives.find("NAME");
  if (name == directives.end()) {
    return LG_OUT_OF_STRINGS;
  }
  
  // first compile the code so we can dump the strings from it in 
  // the correct order
  Logo logo;
  LogoCompiler compiler(&logo);
  compiler.compile(file, directives);
//  compiler.dump(false);
  int err = logo.geterr();
  if (err) {
    return err;
  }
  
  // can dump the strings code.
  logo.dumpstringscode(&compiler, ("strings_" + name->second).c_str(), str);

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
    Logo logo2(0, &strings);
    LogoCompiler compiler2(&logo2);
    compiler2.compile(file, directives);
    int err = logo2.geterr();
    if (err) {
      return err;
    }
    
    // ready to dump the code now.
    logo2.dumpinst(&compiler2, ("code_" + name->second).c_str(), str);
  }
  
  return 0;
  
}

int LogoCompiler::includelgo(const string &infn, const map<string, string> &directives, fstream &outfile) {

  fstream file;
  file.open(infn, ios::in);
  if (!file) {
    cout << "File not found " << infn << endl;
    return 1;
  }
  
  stringstream str;
  int err;
  
  map<string, string>::const_iterator inl = directives.find("INLINE");
  if (inl != directives.end() && inl->second == "true") {
    err = LogoCompiler::inlinelgo(file, directives, str);
  }
  else {
    err = LogoCompiler::generatecode(file, directives, str);
  }
  
  file.close();
  if (err) {
    return err;
  }
  
  outfile << str.str();
  
  return 0;
  
}

void LogoCompiler::getdirectives(const string line, map<string, string> *directives) {
  
  directives->clear();
  
  string rest = line;
  trim(rest);
  vector<string> directive;
  split(directive, rest, is_any_of(" "));
  for (vector<string>::iterator i=directive.begin(); i != directive.end(); i++) {
    trim(*i);
    vector<string> nvp;
    split(nvp, *i, is_any_of("="));
    if (nvp.size() > 0) {
      (*directives)[nvp[0]] = nvp.size() == 2 ? nvp[1] : "";
    }
  }
  
}

void LogoCompiler::replacedirectives(string *line, const map<string, string> &directives, bool autoassign, int *count) {

  while (1) {
    size_t dollar = line->find('$');
    if (dollar == string::npos) {
      // loop until we find no new tokens to replace.
      break;
    }
    string rest = line->substr(dollar);
    size_t space = rest.find(' ');
    string token = space == string::npos ? rest.substr(1) : rest.substr(1, space-1);
    trim(token);
    map<string, string>::const_iterator d = directives.find(token);
    stringstream str;
    str << line->substr(0, dollar);
    if (d == directives.end()) {
      if (autoassign) {
        int n = ++(*count);
        cout << token << "=" << n << endl;
        str << n;
      }
      else {
        cout << "missing " << token << endl;
        str << "?" << token;
      }
    }
    else {
      str << d->second;
    }
    str << (space == string::npos ? "" : rest.substr(space));
    *line = str.str();
  }

}

int LogoCompiler::updateino(const std::string &infn, std::fstream &infile, std::fstream &outfile) {

  const string logo_directive = "//#LOGO";

  string line;
  bool including = false;
  while (getline(infile, line)) {
    size_t pos = line.find(logo_directive);
    if (pos != string::npos) {
      string rest = line.substr(pos + logo_directive.length());
      map<string, string> directives;
      getdirectives(rest, &directives);
      if (directives.find("FILE") != directives.end()) {
        string lfn = directives["FILE"];
        fs::path lgopath(lfn);
        fs::path inpath(infn);
        if (!lgopath.is_absolute()) {
          lgopath = inpath.parent_path() / lgopath;
        }
        including = true;
        outfile << line << endl;
        cout << "including .LGO " << lgopath.string() << endl;
        int err = includelgo(lgopath.string(), directives, outfile);
        if (err) {
          return err;
        }
      }
      else if (directives.find("ENDFILE") != directives.end()) {
        outfile << line << endl;
        including = false;
      }
      else {
        cout << "Unknown directives:" << endl;
        for (map<string, string>::iterator i=directives.begin(); i != directives.end(); i++) {
          cout << i->first << " = "<< i->second << endl;
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
        cout << "(" << (short)jump << ") " << name << endl;
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
  dump(2, var._type, var._value, var._valueopand);
  
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
