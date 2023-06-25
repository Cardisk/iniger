project(inigerTest)

add_subdirectory(./lib/googletest)
set(gtest_SOURCE_DIR, ./lib/googletest/googletest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(inigerTest inigerTest.cpp)
target_link_libraries(inigerTest gtest gtest_main)