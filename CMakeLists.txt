cmake_minimum_required(VERSION 3.20)
project(first_stack)

set(CMAKE_CXX_STANDARD 14)

add_executable(first_stack main.cpp main.h stack_funcs.cpp stack_funcs.h globals.h hash.cpp hash.h)
