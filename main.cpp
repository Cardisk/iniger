#include <iostream>

#include "iniger.h"

int main() {
    ini::Object ini("../test.ini");
    ini::add_property(ini, "test_global1", "value_global1");
    ini::add_property(ini, "test_foo1", "value_foo1", "Foo");
    ini::add_property(ini, "test_foo2", "value_foo2", "Foo");
    ini::add_property(ini, "test_bar", "value_bar", "Bar");
    ini::add_property(ini, "test_foo_baz", "value_foo_baz", "Foo.Baz");
    ini::add_property(ini, "test_foo_baz_bar", "value_foo_baz_bar", "Foo.Baz.Bar");

    std::cout << std::boolalpha << "ini::write result: " << ini::write(ini, ':');

    return 0;
}
