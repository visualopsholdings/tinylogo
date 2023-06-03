/*
  logocompiler.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 5-May-2023
  
  Tiniest Logo Intepreter
    
  This work is licensed under the Creative Commons Attribution 4.0 International License. 
  To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/ or 
  send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  https://github.com/visualopsholdings/tinylogo
*/

#ifndef H_logocompiler
#define H_logocompiler

#include "logo.hpp"

#ifndef ARDUINO
#include <fstream>
#endif

#define LINE_LEN            64        // these number of bytes

typedef struct {
  tStrPool  _name;
  tStrPool  _namelen;
  tJump     _jump;
 tByte      _arity; // smaller than 256?
} LogoWord;

class LogoCompiler {

public:
  LogoCompiler(Logo * logo);
  
  // the compiler.
  void compile(const char *code) {
    LogoSimpleString str(code, strlen(code));
    compile(&str);
  }
  void compile(LogoString *str);
  
  // main execution
  void reset(); // reset the sentences

#ifdef LOGO_DEBUG
  void outstate() const;
  void dump(const char *msg, bool all=true) const;
  void dump(bool all=true) const;
  short stepdump(short n, bool all=true);
  void dump(short indent, short type, short op, short opand) const;
  void mark(short i, short mark, const char *name) const;
  void searchword(short op) const;
  void markword(tJump jump) const;
  void printword(const LogoWord &word) const;
  void dumpwordnames() const;
  void printvar(const LogoVar &var) const;
#endif

#ifndef ARDUINO
  void dumpwordscode(short offset) const;
  void dumpwordstrings() const;
  int wordstringslist(char *buf, int len) const;
  bool haswords() { return _wordcount > 0; }
  void entab(short indent) const;
  void printwordstring(const LogoWord &word) const;
  void printvarstring(const LogoVar &var) const;
  void printvarcode(const LogoVar &var) const;
  static void compile(std::fstream &file, const std::string &name);
#endif

  // public for testing.
  short scanfor(short *strstart, short *strsize, LogoString *str, short start, short len, bool newline);
  void dosentences(char *buf, short len, const char *start);

private:
  
  Logo *_logo;
  
  // the state variables for defining new words
  bool _inword;
  bool _inwordargs;
  short _defining;
  short _defininglen;
  short _jump;
  short _wordarity;
  
  // various buffers to hold data
  char _linebuf[LINE_LEN];
  char _sentencebuf[SENTENCE_LEN];

  // words
  short _wordcount;
  LogoWord _words[MAX_WORDS];
  
  // sentences
  short _sentencecount;

  // builtin definition
  char _builtindef[LINE_LEN];
  
  // parser
  bool dodefine(LogoString *str, short wordstart, short wordlen, bool eol);
  void compilewords(LogoString *str, short start, short len, bool define);
  bool istoken(char c, bool newline);
  bool isnum(LogoString *str, short wordstart, short wordlen);
  void compileword(tJump *next, LogoString *str, short wordstart, short wordlen, short op);
  void finishword(short word, short wordlen, short jump, short arity);
  short findword(LogoString *str, short wordstart, short wordlen) const;
  short findword(LogoStringResult *str) const;
  void findbuiltindef(LogoString *stri, short wordstart, short wordlen, short *index, short *category);
  
#ifndef ARDUINO
  int compile(std::fstream &file, Logo *logo);
#endif

};

#endif // H_logocompiler
