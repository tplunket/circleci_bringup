cmake_minimum_required(VERSION 2.6)
#set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_C_STANDARD 99)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
project(CommandLine)

project(CommandLineTests)

include_directories("${CMAKE_SOURCE_DIR}/..")

add_library(CommandLine CommandLine.c)

add_executable(CommandLineTests CommandLine_t.cpp)
target_link_libraries(CommandLineTests CommandLine)
