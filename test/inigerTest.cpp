//
// Created by Matteo Cardinaletti on 25/06/23.
//
#include "gtest/gtest.h"

#include "../iniger.h"

TEST(inigerTest, ObjectConstruction) {
    ini::Object ini("my_file.ini");
    ASSERT_EQ("my_file.ini", ini.get_file_path());
    ASSERT_EQ("global", ini.get_global().get_name());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}