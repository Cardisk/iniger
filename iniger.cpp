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
    size_t start = 0;
    do {
        next_pos = str.find(delim, start);
        std::string txt = str.substr(start, next_pos - start);
        start = next_pos + 1;

        if (!txt.empty()) v.push_back(txt);
    } while (next_pos != std::string::npos);

    return v;
}

bool ini::add_property(ini::Object &ini, std::string &key, std::string &value, std::string &section_path) {
    if (key.empty() || value.empty()) return false;

    // key symbol cannot contain "=" and ";" inside the Windows implementation.
    if (key.contains('=') || key.contains(';')) {
        std::cerr << "[ERROR]: key symbol cannot contain \"=\" and \";\" inside the Windows implementation\n";
        return false;
    }

    ini::Section *sec = &ini.get_global();

    if (!section_path.empty()) {
        auto path = string_split(section_path, ".");
        for (auto &i : path) {
            try {
                i = to_lower(i);
                sec = &sec->get_subsecs().at(i);
            } catch (std::out_of_range &e) {
                // keep adding missing sections.
                if (!ini::add_section(*sec, i)) {
                    std::cerr << "[ERROR]: could not create new section '" << i << "'\n";
                    return false;
                }

                sec = &sec->get_subsecs().at(i);
            }
        }
    }

    // case-insensitive.
    return ini::add_property(*sec, key, value);
}

bool ini::add_property(ini::Object &ini, std::string &key, std::string &value, std::string &&section_path) {
    return ini::add_property(ini, key, value, section_path);
}

bool ini::add_property(ini::Object &ini, std::string &key, std::string &&value, std::string &section_path) {
    return ini::add_property(ini, key, value, section_path);
}

bool ini::add_property(ini::Object &ini, std::string &key, std::string &&value, std::string &&section_path) {
    return ini::add_property(ini, key, value, section_path);
}

bool ini::add_property(ini::Object &ini, std::string &&key, std::string &value, std::string &section_path) {
    return ini::add_property(ini, key, value, section_path);
}

bool ini::add_property(ini::Object &ini, std::string &&key, std::string &value, std::string &&section_path) {
    return ini::add_property(ini, key, value, section_path);
}

bool ini::add_property(ini::Object &ini, std::string &&key, std::string &&value, std::string &section_path) {
    return ini::add_property(ini, key, value, section_path);
}

bool ini::add_property(ini::Object &ini, std::string &&key, std::string &&value, std::string &&section_path) {
    return ini::add_property(ini, key, value, section_path);
}

bool ini::add_property(ini::Section &sec, std::string &key, std::string &value) {
    try {
        sec.get_props().insert(std::make_pair(to_lower(key), value));
    } catch (std::bad_alloc &e) {
        std::cerr << "[ERROR]: " << e.what() << std::endl;
        return false;
    }
    return true;
}

bool ini::add_property(ini::Section &sec, std::string &key, std::string &&value) {
    return ini::add_property(sec, key, value);
}

bool ini::add_property(ini::Section &sec, std::string &&key, std::string &value) {
    return ini::add_property(sec, key, value);
}

bool ini::add_property(ini::Section &sec, std::string &&key, std::string &&value) {
    return ini::add_property(sec, key, value);
}

bool ini::add_section(ini::Object &ini, std::string &new_section_name, std::string &section_path) {
    if (new_section_name.empty()) {
        std::cerr << "[ERROR]: section name should not be empty\n";
        return false;
    }

    ini::Section *sec = &ini.get_global();
    if (!section_path.empty()) {
        auto path = string_split(section_path, ".");

        for (auto &i : path) {
            try {
                sec = &sec->get_subsecs().at(i);
            } catch (std::out_of_range &e) {
                // keep adding missing sections.
                i = to_lower(i);
                // rollbacks aren't handled at all.
                // example: Foo          .Bar         .Baz
                //          ^ existing    ^ missing    ^ missing
                //                        ^ created    ^ failed
                //                        ^ this should be deleted as well
                // XXX: maybe this is useless because if I tried to create a specific
                //      path is because I needed it.
                if (!ini::add_section(*sec, i)) {
                    std::cerr << "[ERROR]: could not create missing '" << i << "' section\n";
                    return false;
                }
                sec = &sec->get_subsecs().at(i);
            }
        }
    }

    new_section_name = to_lower(new_section_name);
    return ini::add_section(*sec, new_section_name);
}

bool ini::add_section(ini::Object &ini, std::string &new_section_name, std::string &&section_path) {
    return ini::add_section(ini, new_section_name, section_path);
}

bool ini::add_section(ini::Object &ini, std::string &&new_section_name, std::string &section_path) {
    return ini::add_section(ini, new_section_name, section_path);
}

bool ini::add_section(ini::Object &ini, std::string &&new_section_name, std::string &&section_path) {
    return ini::add_section(ini, new_section_name, section_path);
}

bool ini::add_section(ini::Section &sec, std::string &new_section_name) {
    new_section_name = to_lower(new_section_name);
    try {
        sec.get_subsecs().insert(std::make_pair(new_section_name, Section(new_section_name)));
    } catch (std::bad_alloc &e) {
        std::cerr << "[ERROR]: " << e.what() << std::endl;
        return false;
    }
    return true;
}

bool ini::add_section(ini::Section &sec, std::string &&new_section_name) {
    return ini::add_section(sec, new_section_name);
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