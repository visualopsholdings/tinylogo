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
#include "logostring.hpp"

#ifndef ARDUINO
#include <fstream>
#include <map>
#endif

#ifdef USE_LARGE_CODE
#define MAX_WORDS           1024       // 6 bytes each
#else
#define MAX_WORDS           16        // 6 bytes each
#endif

#define LINE_LEN            128       // these number of bytes

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
  int callword(const char *word);

  // main execution
  void reset();

#ifdef LOGO_DEBUG
 void outstate() const;
#endif

#ifndef ARDUINO
  void dump(const char *msg, bool all=true) const;
  void dump(bool all=true) const;
  short stepdump(short n, bool all=true);
  void dump(short indent, short type, short op, short opand) const;
  void searchword(short op) const;
  void markword(tJump jump) const;
  void printword(const LogoWord &word) const;
  void printvar(const LogoVar &var) const;
  void dumpwordscode(short offset, std::ostream &str) const;
  void dumpwordstrings(std::ostream &str) const;
  int wordstringslist(char *buf, int len) const;
  bool haswords() { return _wordcount > 0; }
  void entab(short indent) const;
  void printwordstring(const LogoWord &word, std::ostream &str) const;
  static int generatecode(std::fstream &file, const std::map<std::string, std::string> &directives, std::ostream &str);
  static int inlinelgo(std::fstream &file, const std::map<std::string, std::string> &directives, std::ostream &str);
  static int updateino(const std::string &infn, std::fstream &infile, std::fstream &outfile);
  static int includelgo(const std::string &infn, const std::map<std::string, std::string> &directives, std::fstream &outfile);
  int compile(std::fstream &file, const std::map<std::string, std::string> &directives);
  static void getdirectives(const std::string line, std::map<std::string, std::string> *directives);
  static void replacedirectives(std::string *line, const std::map<std::string, std::string> &directives);
#endif

  // public for testing.
  short scan(short *strstart, short *strsize, LogoString *str, short len, short start, bool newline);
  bool switchtoken(char prevc, char c, bool newline);

private:
  
  Logo *_logo;
  
  // the state variables for defining new words
  bool _inword;
  bool _inwordargs;
  short _defining;
  short _defininglen;
  short _jump;
  short _wordarity;
  
  // words
  short _wordcount;
  LogoWord _words[MAX_WORDS];
  
  // parser
  bool dodefine(LogoString *str, short wordstart, short wordlen, bool eol);
  void compilewords(LogoString *str, short start, short len, bool define);
  void compileword(tJump *next, LogoString *str, short wordstart, short wordlen, short op);
  void finishword(short word, short wordlen, short jump, short arity);
  short findword(LogoString *str, short wordstart, short wordlen) const;
  short findword(LogoStringResult *str) const;
  
};

#endif // H_logocompiler
