/*
  realtimeprovider.cpp
    
  Author: Paul Hamilton (paul@visualops.com)
  Date: 5-May-2023
  
  This work is licensed under the Creative Commons Attribution 4.0 International License. 
  To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/ or 
  send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  https://github.com/visualopsholdings/tinylogo
*/

#include "realtimeprovider.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <chrono>
#include <thread>

using namespace std;
using namespace boost::posix_time;

unsigned long RealTimeProvider::currentms() {
  std::time_t t = 1118158776;
  ptime first = from_time_t(t);
  ptime nowt = microsec_clock::local_time();
  time_duration start = nowt - first;
  return start.total_milliseconds();
}

void RealTimeProvider::delayms(unsigned long ms) {
  this_thread::sleep_for(chrono::milliseconds(ms));
}

bool RealTimeProvider::testing(short ms) { 
  cout << "WAIT " << ms << endl;
  return false; 
};
