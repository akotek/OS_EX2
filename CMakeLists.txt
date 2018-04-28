cmake_minimum_required(VERSION 3.8)
project(EX2)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp uthreads.h demo_itimer.c demo_jmp.c demo_singInt_handler.c uthreads.cpp)
add_executable(EX2 ${SOURCE_FILES})