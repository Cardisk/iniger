//
// Created by Matteo Cardinaletti on 26/06/23.
//
#include "gtest/gtest.h"

#include "../iniger.h"

class InsertionTest : public ::testing::Test {
protected:
    void SetUp() override {
        ini = new ini::Object("my_file.ini");
    }

    void TearDown() override {
        delete ini;
    }

    ini::Object *ini = nullptr;
};

// using fixture only to try them
TEST_F(InsertionTest, GlobalPropertyInsertion) {
    bool result = ini::add_property(*ini, "test", "insertion");
    ASSERT_EQ(true, result);
    ASSERT_EQ("insertion", ini::get_property(*ini, "test"));
}

// using fixture only to try them
TEST_F(InsertionTest, PropertyInsertion) {
    bool result = ini::add_property(*ini, "test", "insertion", "Foo");
    ASSERT_EQ(true, result);
    ASSERT_EQ("insertion", ini::get_property(*ini, "test", "Foo"));
}