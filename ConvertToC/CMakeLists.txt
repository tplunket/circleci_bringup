# \author Tom Plunket <tom@mightysprite.com>
# \copyright (c) 2017 Tom Plunket, all rights reserved
#
# Licensed under the MIT/X license. Do with these files what you will but leave this header intact.

cmake_minimum_required(VERSION 2.6)
set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_C_STANDARD 99)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
project(ConvertToC)

include_directories("${CMAKE_SOURCE_DIR}/..")

add_executable(ConvertToC
    ConvertToC.cpp
    ../Log/Log.c
    ../CommandLine/CommandLine.c)
