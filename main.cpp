#include <iostream>

#include "iniger.h"

int main() {
    ini::Object out("../test1.ini");
    ini::add_property(out, "test_global1", "value_global1");
    ini::add_property(out, "test_foo1", "value_foo1", "Foo");
    ini::add_property(out, "test_foo2", "value_foo2", "Foo");
    ini::add_property(out, "test_bar", "value_bar", "Bar");
    ini::add_property(out, "test_foo_baz", "value_foo_baz", "Foo.Baz");
    ini::add_property(out, "test_foo_baz_bar", "value_foo_baz_bar", "Foo.Baz.Bar");

    std::cout << std::boolalpha;
    std::cout << "[INFO]: write result: " << ini::write(out, ':') << std::endl;
    ini::Object in = ini::read("../test1.ini");
    in.set_file_path("../test2.ini");
    std::cout << "[INFO]: re-output result: " << ini::write(in, ':') << std::endl;

    std::cout << std::noboolalpha;
    return 0;
}
