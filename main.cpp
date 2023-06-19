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

    // just ugly logging for debug purposes.
    std::cout << "-----------\n";
    std::cout << "PROPERTIES:\n";
    for (auto &i : ini.get_global().get_props()) {
        std::cout << i.first << " -> " << i.second << std::endl;
    }
    std::cout << "SUBSECTIONS:\n";
    for (auto &i : ini.get_global().get_subsecs()) {
        std::cout << i.first << std::endl;
        for (auto &j : i.second.get_props()) {
            std::cout << "\t" << j.first << " -> " << j.second << std::endl;
        }
        std::cout << std::endl;
    }
    for (auto &i : ini.get_global().get_subsecs().at("foo").get_subsecs()) {
        std::cout << "\t" << i.first << std::endl;
        for (auto &j : i.second.get_props()) {
            std::cout << "\t\t" << j.first << " -> " << j.second << std::endl;
        }
    }
    for (auto &i : ini.get_global().get_subsecs().at("foo").get_subsecs().at("baz").get_subsecs()) {
        std::cout << "\t\t" << i.first << std::endl;
        for (auto &j : i.second.get_props()) {
            std::cout << "\t\t\t" << j.first << " -> " << j.second << std::endl;
        }
    }
    std::cout << "-----------\n\n";

    return 0;
}
