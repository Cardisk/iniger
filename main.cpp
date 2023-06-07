#include <iostream>

#include "iniger.h"

int main() {
    ini::Object ini("../test.ini");

    std::cout << std::boolalpha;
    std::cout << "add_property result: "
                << ini::add_property(ini, "global", "writing", "test")
                << std::endl;
    std::cout << "global empty: " << ini.get_sections().at("global").props_empty() << std::endl;
    std::cout << "write result: " << ini::write(ini, ':');
    return 0;
}
