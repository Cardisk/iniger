project(inigerTest)

set(CMAKE_CXX_STANDARD 23)

set(LIB ../iniger.h ../iniger.cpp)
set(TEST objectConstructionTest.cpp propertyInsertionFixture.cpp sectionInsertionTest.cpp readWriteTest.cpp)

add_subdirectory(./lib/googletest)
set(gtest_SOURCE_DIR, ./lib/googletest/googletest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_library(libInigerTest ${LIB})

add_executable(inigerTest inigerTest.cpp ${TEST})
target_link_libraries(inigerTest gtest gtest_main libInigerTest)