/*
  testtimeprovider.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 19-May-2023
  
  https://github.com/visualopsholdings/tinylogo
*/

// just include any time after defining PRINT_RESULT and gCmds

class TestTimeProvider: public LogoTimeProvider {

public:

  unsigned long currentms() {
    return 0;
  }
  void delayms(unsigned long ms) {
  }
  bool testing(short ms) {
    return true;
  }
  void settime(unsigned long time) {
  }
  
};
