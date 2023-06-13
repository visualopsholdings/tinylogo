/*
  flashcode.cpp
    
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

#include <iostream>
#include <boost/program_options.hpp> 
#include <boost/filesystem.hpp>
#include <fstream>
#include <strstream>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

using namespace std;

int process_ino(const string &infn) {

  fs::path inpath(infn);
  fstream infile;
  infile.open(infn, ios::in);
  if (!infile) {
    cout << "Input file not found " << infn << endl;
    return 1;
  }
  string outfn = infn + ".txt";
  fstream outfile;
  outfile.open(outfn, ios::out);
  int err = LogoCompiler::updateino(infn, infile, outfile);
  infile.close();
  outfile.close();
  
  if (err) {
    cout << "got error " << err << endl;
    return err;
  }
  
  fs::rename(infn, infn + ".old");
  fs::rename(outfn, infn);
  
  return 0;
  
}

int process_lgo(const string &infn, const string &name) {
  fstream file;
  file.open(infn, ios::in);
  if (!file) {
    cout << "File not found " << infn << endl;
    return 1;
  }
  LogoCompiler::generatecode(file, name, cout);
  file.close();
  return 0;
  
}

int main(int argc, char *argv[]) {

  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "produce help message")
    ("input-file", po::value<string>(), "input file")
    ("name", po::value<string>()->default_value("prog"), "variable name")
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
  if (vm.count("input-file") && vm.count("name")) {
    string infn = vm["input-file"].as< string >();
    fs::path inpath(infn);
    if (inpath.extension() == ".ino") {
      return process_ino(infn);
    }
    else if (inpath.extension() == ".lgo") {
      return process_lgo(infn, vm["name"].as< string >());
    }
  }
  
  cout << desc << endl;
  return 1;
}
