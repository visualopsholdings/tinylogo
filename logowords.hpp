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

class LogoWords {

public:

  static void err(Logo &logo);
  static short errArity;
  
  static void ifelse(Logo &logo);
  static short ifelseArity;
  
  static void ifWord(Logo &logo);
  static short ifArity;
  
  static void repeat(Logo &logo);
  static short repeatArity;
  
  static void forever(Logo &logo);
  static short foreverArity;

  static void make(Logo &logo);
  static short makeArity;

  static void wait(Logo &logo);
  static short waitArity;

  static void subtract(Logo &logo);
  static short subtractArity;

  static void add(Logo &logo);
  static short addArity;

  static void divide(Logo &logo);
  static short divideArity;

  static void multiply(Logo &logo);
  static short multiplyArity;

  static void eq(Logo &logo);
  static short eqArity;

  static void neq(Logo &logo);
  static short neqArity;

  static void gt(Logo &logo);
  static short gtArity;

  static void gte(Logo &logo);
  static short gteArity;

  static void lt(Logo &logo);
  static short ltArity;

  static void lte(Logo &logo);
  static short lteArity;

  static void notWord(Logo &logo);
  static short notArity;

  static void dumpvars(Logo &logo);
  static short dumpvarsArity;

  static void print(Logo &logo);
  static short printArity;

  static void dread(Logo &logo);
  static short dreadArity;

  static void dhigh(Logo &logo);
  static short dhighArity;

  static void dlow(Logo &logo);
  static short dlowArity;

  static void pinout(Logo &logo);
  static short pinoutArity;

  static void pinin(Logo &logo);
  static short pininArity;

  static void aout(Logo &logo);
  static short aoutArity;

  static void thing(Logo &logo);
  static short thingArity;

private:

  static bool pushliterals(Logo &logo, short rel);

};

#endif // H_logowords
