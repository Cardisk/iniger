cmake_minimum_required(VERSION 3.25)
project(iniger)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(test)

add_executable(iniger main.cpp iniger.cpp iniger.h)
