/*
  run.cpp
    
  Author: Paul Hamilton (paul@visualops.com)
  Date: 2-Jun-2023
  
  Write variables used to hold code and strings in flash memory.
  
  This work is licensed under the Creative Commons Attribution 4.0 International License. 
  To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/ or 
  send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  https://github.com/visualopsholdings/tinylogo
*/

#include "../logo.hpp"
#include "../logocompiler.hpp"
#include "../arduinoflashcode.hpp"
#include "realtimeprovider.hpp"

#include <iostream>
#include <boost/program_options.hpp> 
#include <fstream>
#include <strstream>

namespace po = boost::program_options;
using namespace std;

int main(int argc, char *argv[]) {

  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "produce help message")
    ("input-file", po::value<string>(), "input file")
    ("dump", "dump after compile")
    ("step-dump", po::value<int>(), "step and dump this number of times")
    ;
  po::positional_options_description p;
  p.add("input-file", -1);
  
  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).
          options(desc).positional(p).run(), vm);
  po::notify(vm);   
    
  if (vm.count("help")) {
    cout << desc << endl;
    return 1;
  }
  if (vm.count("input-file")) {
    fstream file;
    file.open(vm["input-file"].as< string >(), ios::in);
    if (!file) {
		  cout << "File not found" << endl;
    }
    else {
      RealTimeProvider time;
      LogoStaticPrimitives primitives;
      Logo logo(&primitives, &time, Logo::core);
      LogoCompiler compiler(&logo);
      compiler.compile(file);
      int err = logo.geterr();
      if (err) {
        cout << "got compile err " << err << endl;
        return err;
      }
      if (vm.count("dump")) {
        compiler.dump(false);
      }
      if (vm.count("step-dump")) {
        int n = vm["step-dump"].as< int >();
        err = 0;
        for (short i=0; i<n && err != LG_STOP; i++) {
          cout << "step " << i << " -----------" << endl;
          err = logo.step();
          compiler.dump(false);
        }
        if (err && err != LG_STOP) {
          cout << "got step err " << err << endl;
          return err;
        }
      }
      else {
        err = logo.run();
        if (err) {
          cout << "got runerr " << err << endl;
          return err;
        }
      }
      file.close();
    }
    return 0;
  }
  
  cout << desc << endl;
}
