cmake_minimum_required(VERSION 3.8)
project(EX2)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES uthreads.h demo_itimer.c demo_jmp.c demo_singInt_handler.c uthreads.cpp
        tests_for_ex/ex_2_test_suite/test132.cpp)
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)
add_executable(EX2 ${SOURCE_FILES})
