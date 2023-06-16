#include <iostream>

#include "iniger.h"

int main() {
    ini::Object ini("../test1.ini");

    std::cout << std::boolalpha;
    std::cout << "add_property result: "
                << ini::add_property(ini, "global", "writing", "test")
                << std::endl;
    std::cout << "global empty: " << ini.get_global().props_empty() << std::endl;

    std::cout << "-----------------\nGlobal:\n";
    for (auto &i : ini.get_global().get_props()) {
        std::cout << i.first << " -> " << i.second << std::endl;
    }
    std::cout << "-----------------\n";

    std::cout << "add_property result 2: "
              << ini::add_property(ini, "FOO", "bar", "baz")
              << std::endl;

    std::cout << "-----------------\nSubSecs:\n";
    for (auto &i : ini.get_global().get_subsecs()) {
        std::cout << i.first << std::endl;
        for (auto &j : i.second.get_props()) {
            std::cout << "\t" << j.first << " -> " << j.second << std::endl;
        }
    }
    std::cout << "-----------------\n";

//    for (auto &i : ini.get_global().get_props()) {
//        std::cout << i.first << " -> " << i.second << std::endl;
//    }
//    std::cout << "-------------------\n";
//    for (auto &i : ini.get_global().get_subsecs().at("FOO").get_props()) {
//        std::cout << i.first << " -> " << i.second << std::endl;
//    }

    std::cout << "write result: " << ini::write(ini, ':');

//    ini::read("../test.ini");
    return 0;
}
