/*
  logowords.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 5-May-2023
  
  Words for Tiniest Logo Intepreter
    
  This work is licensed under the Creative Commons Attribution 4.0 International License. 
  To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/ or 
  send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  https://github.com/visualopsholdings/tinylogo
*/

#ifndef H_logowords
#define H_logowords

class Logo;
class LogoSimpleString;
class LogoStringResult;

class LogoWords {

public:

  static void err(Logo &logo);
  static void ifelse(Logo &logo);
  static void ifWord(Logo &logo);
  static void repeat(Logo &logo);
  static void forever(Logo &logo);
  static void make(Logo &logo);
  static void wait(Logo &logo);
  static void subtract(Logo &logo);
  static void add(Logo &logo);
  static void divide(Logo &logo);
  static void multiply(Logo &logo);
  static void eq(Logo &logo);
  static void neq(Logo &logo);
  static void gt(Logo &logo);
  static void gte(Logo &logo);
  static void lt(Logo &logo);
  static void lte(Logo &logo);
  static void notWord(Logo &logo);
  static void dumpvars(Logo &logo);
  static void print(Logo &logo);
  static void dread(Logo &logo);
  static void dhigh(Logo &logo);
  static void dlow(Logo &logo);
  static void pinout(Logo &logo);
  static void pinin(Logo &logo);
  static void pininup(Logo &logo);
  static void aout(Logo &logo);
  static void thing(Logo &logo);
  static void output(Logo &logo);
  static void wordWord(Logo &logo);
  static void first(Logo &logo);
  static void type(Logo &logo);
  static void machineinfo(Logo &logo);
  static void pinrgb(Logo &logo);
  static void rgbout(Logo &logo);
  static void wifiscan(Logo &logo);
  static void wifiget(Logo &logo);
  static void wifisockets(Logo &logo);
  static void vopsopenmsg(Logo &logo);
  static void wifilogintest(Logo &logo);
  static void wifilogin(Logo &logo);
  static void wifistation(Logo &logo);
  static void wificonnect(Logo &logo);
  static void btstart(Logo &logo);
  static void throwWord(Logo &logo);
  static void intWord(Logo &logo);
  static void midinoteon(Logo &logo);
  static void midinoteoff(Logo &logo);

  static bool extractEventName(LogoSimpleString *s, char *name, int len);

private:

  static bool pushliterals(Logo &logo, short rel);

};

#endif // H_logowords
