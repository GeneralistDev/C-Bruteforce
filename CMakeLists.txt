cmake_minimum_required(VERSION 2.8.4)
project(C_Bruteforce)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11 -lm -Wall -pedantic")

set(SOURCE_FILES
    bruteforce.c
    helptext.h
    makefile)

include_directories(.)

add_executable(C_Bruteforce ${SOURCE_FILES})

target_link_libraries(C_Bruteforce m)