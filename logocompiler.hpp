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
  void dumpwords(bool code=false) const;
  short stepdump(short n, bool all=true);
  void dump(short indent, const LogoInstruction &entry) const;
  void dump(short indent, short type, short op, short opand) const;
  void printword(const LogoWord &word, bool code=false) const;
  void entab(short indent) const;
  void mark(short i, short mark, const char *name) const;
  void searchword(short op) const;
  void markword(tJump jump) const;
#ifdef HAS_VARIABLES
  void printvar(const LogoVar &var, bool code=false) const;
#endif
#endif

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
  char _wordbuf[WORD_LEN];
  char _sentencebuf[WORD_LEN];

  // words
  short _wordcount;
  LogoWord _words[MAX_WORDS];
  
  // sentences
  short _sentencecount;

  // parser
  bool dodefine(const char *word, bool eol);
  void compilewords(LogoString *str, short len, bool define);
  short scanfor(char *s, short size, LogoString *str, short len, short start, bool newline);
  bool istoken(char c, bool newline);
  bool isnum(const char *word, short len);
  void compileword(tJump *next, const char *word, short op);
  void finishword(short word, short wordlen, short jump, short arity);
  void dosentences(char *buf, short len, const char *start);
  short findword(const char *word) const;
  
};

#endif // H_logocompiler
