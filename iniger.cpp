//
// Created by Matteo Cardinaletti on 22/05/23.
//

#include "iniger.h"

#include <fstream>

std::string &to_lower(std::string &str) {
    std::transform(str.begin(), str.end(), str.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return str;
}

std::vector<std::string> string_split(std::string &str, const std::string &delim) {
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

std::string join(const std::vector<std::string> &v, const std::string &delim) {
    std::string str;
    for (int i = 0; i < v.size(); ++i) {
        if (i > 0) str += delim;
        str += v[i];
    }

    return str;
}

bool ini::add_property(ini::Object &ini, std::string &key, std::string &value, std::string &section_path) {
    return true;
}

bool ini::add_property(ini::Object &ini, std::string &&key, std::string &&value, std::string &&section_path) {
    return ini::add_property(ini, key, value, section_path);
}

bool ini::add_section(ini::Object &ini, std::string &new_section_name, std::string &section_path) {
    return true;
}

bool ini::add_section(ini::Object &ini, std::string &&new_section_name, std::string &&section_path) {
    return ini::add_section(ini, new_section_name, section_path);
}

ini::Object ini::read(std::string &path) {
    return ini::Object("");
}

ini::Object ini::read(std::string &&path) {
    return ini::read(path);
}

bool ini::write(ini::Object &ini, const char key_val_separator) {
    return true;
}