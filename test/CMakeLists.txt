cmake_minimum_required(VERSION 3.25)
project(inigerTest)

set(CMAKE_CXX_STANDARD 23)

# I don't know why but cmake said it was not present inside the structure
set(GOOGLETEST_VERSION 1.13.0)

set(LIB ../iniger.h ../iniger.cpp)
set(TEST objectConstructionTest.cpp propertyInsertionFixture.cpp sectionInsertionTest.cpp readWriteTest.cpp)

add_subdirectory(./lib/googletest)
set(gtest_SOURCE_DIR, ./lib/googletest/googletest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_library(libInigerTest ${LIB})

add_executable(inigerTest inigerTest.cpp ${TEST})
target_link_libraries(inigerTest gtest gtest_main libInigerTest)