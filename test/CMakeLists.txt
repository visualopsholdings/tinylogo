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

#set(CMAKE_BUILD_TYPE Debug)

project (logotest)
  find_package(Boost COMPONENTS unit_test_framework filesystem REQUIRED)
  add_definitions(-DBOOST_ALL_DYN_LINK) 
#  add_definitions(-DLOGO_DEBUG) 
  enable_testing()

add_library (Logo STATIC ../logo.cpp ../logocompiler.cpp ../logostring.cpp ../arduinoflashcode.cpp ../logowords.cpp ../list.cpp)
  target_link_libraries(Logo ${Boost_FILESYSTEM_LIBRARY})

add_executable(LGTestCompiler lgtestcompiler.cpp)
  target_link_libraries(LGTestCompiler Logo ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_executable(LGTestString lgteststring.cpp)
  target_link_libraries(LGTestString Logo ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_executable(LGTest lgtest.cpp)
  target_link_libraries(LGTest Logo ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_executable(LGTestErr lgtesterr.cpp)
  target_link_libraries(LGTestErr Logo ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_executable(LGTestWords lgtestwords.cpp)
  target_link_libraries(LGTestWords Logo ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_executable(LGTestExamples lgtestexamples.cpp)
  target_link_libraries(LGTestExamples Logo ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_executable(LGTestList lgtestlist.cpp)
  target_link_libraries(LGTestList Logo ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_executable(LGTestSketch lgtestsketch.cpp)
  target_link_libraries(LGTestSketch Logo ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_test(LGTestCompiler LGTestCompiler)
add_test(LGTestString LGTestString)
add_test(LGTest LGTest)
add_test(LGTestErr LGTestErr)
add_test(LGTestWords LGTestWords)
add_test(LGTestExamples LGTestExamples)
add_test(LGTestList LGTestList)
add_test(LGTestSketch LGTestSketch)
