//
// Created by Matteo Cardinaletti on 26/06/23.
//
#include "gtest/gtest.h"

#include "../iniger.h"

TEST(SectionInsertion, GlobalSectionInsertion) {
    ini::Object ini("my_file.ini");
    bool result = ini::add_section(ini, "Foo");
    ASSERT_EQ(true, result);
    // inside runtime representation, names are always lowercase
    ASSERT_EQ("foo", ini::get_section(ini, "Foo").get_name());
}

TEST(SectionInsertion, SectionNestingInsertion) {
    ini::Object ini("my_file.ini");
    bool result = ini::add_section(ini, "Bar", "Foo");
    ASSERT_EQ(true, result);
    // inside runtime representation, names are always lowercase
    ASSERT_EQ("bar", ini::get_section(ini, "Bar", "Foo").get_name());
}