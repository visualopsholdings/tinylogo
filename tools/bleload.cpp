/*
  bleload.cpp
    
  Author: Paul Hamilton (paul@visualops.com)
  Date: 2-Jun-2023
  
  Load a logo program into a Clicker using BLE.
  
  This work is licensed under the Creative Commons Attribution 4.0 International License. 
  To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/ or 
  send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  https://github.com/visualopsholdings/tinylogo
*/

#include "../logobleclnt.hpp"

#include <iostream>
#include <boost/program_options.hpp> 
#include <optional>
#include <fstream>

namespace po = boost::program_options;
using namespace std;

int main(int argc, char *argv[]) {

  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "produce help message")
    ("input-file", po::value<string>(), "input file")
    ("name", po::value<string>(), "peripheral name")
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
 
  if (vm.count("name") && vm.count("input-file")) {
  
    string infn = vm["input-file"].as< string >();
    
    fstream file;
    file.open(vm["input-file"].as< string >(), ios::in);
    if (!file) {
		  cout << "File not found" << endl;
		  return 1;
    }
    string contents = "";
    string line;
    while (getline(file, line)) {
      contents += line + "\n";
    }

    auto name = vm["name"].as< string >();

    LogoBLEClient client;
    
    if (!client.send(name, contents)) {
      cout << "Couldn't send" << endl;
      return 1;
    }
    return 0;
  }
    
  cout << desc << endl;
}

