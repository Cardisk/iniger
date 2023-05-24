//
// Created by Matteo Cardinaletti on 22/05/23.
//

#include "iniger.h"

std::vector<std::string> string_split(std::string str, const std::string &delim) {
    std::vector<std::string> v;
    size_t next_pos;
    do {
        next_pos = str.find(delim);
        std::string txt = str.substr(0, next_pos);
        str.erase(0, next_pos + delim.length());

        if (!txt.empty()) v.push_back(txt);
    } while (next_pos != std::string::npos);

    return v;
}

bool ini::add_property(ini::Object &ini, std::string &section_path, const std::string &key, const std::string &value) {
    auto path = string_split(section_path, ".");
    Section sec;
    for (auto &s : path) {
        try {
            sec = ini.get_sections().at(s);
        } catch (std::out_of_range &e) {
            return false;
        }
    }

    return sec.get_props().insert(std::make_pair(key, value)).second;
}

bool ini::add_section(ini::Object &ini, std::string &section_path, const std::string &new_section_name) {
    auto path = string_split(section_path, ".");
    Section sec;
    for (auto &s : path) {
        try {
            sec = ini.get_sections().at(s);
        } catch (std::out_of_range &e) {
            return false;
        }
    }

    sec.get_subsecs().emplace_back(new_section_name);
    return true;
}

ini::Object ini::read(const std::string &path) {
    return ini::Object("");
}

void ini::write(ini::Object &ini) {

}