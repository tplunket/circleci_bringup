# \author Tom Plunket <tom@mightysprite.com>
# \copyright (c) 2017 Tom Plunket, all rights reserved
#
# Licensed under the MIT/X license. Do with these files what you will.

cmake_minimum_required(VERSION 2.6)
set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_C_STANDARD 99)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
project(Log)

project(LogTests)

include_directories("${CMAKE_SOURCE_DIR}/..")

add_library(Log Log.c)

add_executable(LogTests Log_t.cpp LogTarget_t.cpp)
target_link_libraries(LogTests Log)
