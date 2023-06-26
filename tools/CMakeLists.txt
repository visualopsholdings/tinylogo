#   CMakeLists.txt
#
#   Author: Paul Hamilton (paul@visualops.com)
#   Date: 2-Jun-2023
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

project (tinylogo)
  find_package(Boost COMPONENTS program_options filesystem unit_test_framework REQUIRED)
  add_definitions(-DBOOST_ALL_DYN_LINK -DUSE_FLASH_CODE) 
#  add_definitions(-DLOGO_DEBUG) 
  enable_testing()

add_library (Logo STATIC ../logo.cpp ../logocompiler.cpp ../logostring.cpp ../arduinoflashcode.cpp ../logowords.cpp)
  target_link_libraries(Logo ${Boost_FILESYSTEM_LIBRARY})

add_executable(flashcode flashcode.cpp realtimeprovider.cpp)
  target_link_libraries(flashcode Logo ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_FILESYSTEM_LIBRARY})

add_executable(run run.cpp realtimeprovider.cpp)
  target_link_libraries(run Logo ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} ${Boost_PROGRAM_OPTIONS_LIBRARY})
