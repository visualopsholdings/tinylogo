#   CMakeLists.txt
#
#   Author: Paul Hamilton (paul@visualops.com)
#   Date: 5-May-2023
#
#   Input file for TinyLogo etc for cmake
#
#   This work is licensed under the Creative Commons Attribution 4.0 International License. 
#   To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/ or 
#   send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.
#
#   https://github.com/visualopsholdings/tinylogo

cmake_minimum_required(VERSION 3.5)

project (flashcmd)
  find_package(Boost COMPONENTS unit_test_framework REQUIRED)
  add_definitions(-DBOOST_ALL_DYN_LINK) 
  enable_testing()

add_executable(LGTestCompiler lgtestcompiler.cpp ../logo.cpp ../logocompiler.cpp ../arduinoflashcode.cpp)
  target_link_libraries(LGTestCompiler ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_executable(LGTestString lgteststring.cpp ../logo.cpp ../logocompiler.cpp ../arduinoflashcode.cpp)
  target_link_libraries(LGTestString ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_executable(LGTest lgtest.cpp ../logo.cpp ../logocompiler.cpp ../arduinoflashcode.cpp)
  target_link_libraries(LGTest ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_executable(LGTestErr lgtesterr.cpp ../logo.cpp ../logocompiler.cpp ../arduinoflashcode.cpp)
  target_link_libraries(LGTestErr ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_executable(LGTestWords lgtestwords.cpp ../logo.cpp ../logocompiler.cpp ../arduinoflashcode.cpp)
  target_link_libraries(LGTestWords ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_executable(LGTestLEDSketch lgtestledsketch.cpp ../logo.cpp ../logocompiler.cpp ../arduinoflashcode.cpp)
  target_link_libraries(LGTestLEDSketch ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_executable(LGTestRGBSketch lgtestrgbsketch.cpp ../logo.cpp ../logocompiler.cpp ../arduinoflashcode.cpp)
  target_link_libraries(LGTestRGBSketch ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_test(LGTestCompiler LGTestCompiler)
add_test(LGTestString LGTestString)
add_test(LGTest LGTest)
add_test(LGTestErr LGTestErr)
add_test(LGTestWords LGTestWords)
add_test(LGTestLEDSketch LGTestLEDSketch)
add_test(LGTestRGBSketch LGTestRGBSketch)
