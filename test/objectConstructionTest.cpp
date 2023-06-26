//
// Created by Matteo Cardinaletti on 26/06/23.
//
#include "gtest/gtest.h"

#include "../iniger.h"

TEST(InigerTest, ObjectConstruction) {
    ini::Object ini("my_file.ini");
    ASSERT_EQ("my_file.ini", ini.get_file_path());
    ASSERT_EQ("global", ini.get_global().get_name());
}