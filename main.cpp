#include <iostream>

#include "iniger.h"

int main() {
    std::unordered_map<std::string, int> map;
    map.insert(std::make_pair("uno", 1));
    map.insert(std::make_pair("due", 2));
    for (auto &i : map) {
        std::cout << i.first << " -> " << i.second << std::endl;
    }

    // collisions are handled by aborting the insertion.
    std::cout << map.insert(std::make_pair("uno", 10)).second << std::endl;
    for (auto &i : map) {
        std::cout << i.first << " -> " << i.second << std::endl;
    }

    return 0;
}
