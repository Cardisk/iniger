//
// Created by Matteo Cardinaletti on 26/06/23.
//
#include "gtest/gtest.h"

#include "../iniger.h"

TEST(ReadWrite, WritingTest) {
    // if missing, the file will be created by the ini::write function
    ini::Object ini("../../test/writing_test.ini");
    bool result = ini::add_property(ini, "global_key", "global_value");
    ASSERT_EQ(true, result);
    // property insertion will automatically add sections inside section_path if they are missing
    result = ini::add_property(ini, "foo_key", "foo_value", "Foo");
    ASSERT_EQ(true, result);
    result = ini::write(ini, ':');
    ASSERT_EQ(true, result);
}

TEST(ReadWrite, ReadingTest) {
    /*
     * ini::Object ini = ini::read("../../test/reading_test.ini");
     * ASSERT_EQ("../../test/reading_test.ini", ini.get_file_path());
     */
    ini::Object ini("../../test/reading_test.ini");
    bool result = ini::read(ini);
    ASSERT_EQ(true, result);
}