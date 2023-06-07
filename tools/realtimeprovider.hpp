/*
  realtimeprovider.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 5-May-2023
  
  Tiniest Logo Intepreter
    
  This work is licensed under the Creative Commons Attribution 4.0 International License. 
  To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/ or 
  send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  https://github.com/visualopsholdings/tinylogo
*/

#ifndef H_realtimeprovider
#define H_realtimeprovider

#include "../logo.hpp"

class RealTimeProvider: public LogoTimeProvider {

public:

  unsigned long currentms();
  void delayms(unsigned long ms);
  bool testing(short ms);
  
};

#endif // H_realtimeprovider
