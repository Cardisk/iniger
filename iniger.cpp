//
// Created by Matteo Cardinaletti on 22/05/23.
//

#include "iniger.h"

#include <fstream>

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

bool ini::add_section(ini::Object &ini, const std::string &new_section_name, const std::string &section_path = "") {
    Section sec;
    if (section_path.empty()) {
        sec.set_name(new_section_name);
        return ini.get_sections().insert(std::make_pair(new_section_name, sec)).second;
    }

    auto path = string_split(section_path, ".");
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
    if (!path.ends_with(".ini")) {
        std::string error = "ERROR: file \"" + path + "\" has an incompatible extension type\n";
        throw ini::Extension_error(error);
    }

    return ini::Object(path);
}

std::string &to_lower(std::string &str) {
    std::transform(str.begin(), str.end(), str.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return str;
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
void section_to_string(std::string &str, const char key_val_separator, const std::string &section_name, ini::Section &s) {
    if (section_name != "global") str += "[" + section_name + "]";

    if (!s.props_empty()) {
        for (auto &kv : s.get_props()) {
            // key symbol cannot contain "=" and ";" inside the Windows implementation.
            if (kv.first.contains("=") || kv.first.contains(";")) {
                std::string error = "ERROR: key \"" + kv.first + R"(" cannot contain any "=" or ";" symbol)" + "\n";
                throw ini::Key_error(error);
            }
            // case-insensitive.
            str += to_lower(const_cast<std::string &>(kv.first))
                       + " " + std::to_string(key_val_separator)
                       + " ";
            // quoted values are used to explicit define spaces inside values.
            if (kv.second.contains(' ')) str += "\"";
            str += kv.second;
            if (kv.second.contains(' ')) str += "\"";
            str += "\n";
        }
    }

    if (!s.get_subsecs().empty()) {
        for (auto &sub : s.get_subsecs()) {
            section_to_string(str, key_val_separator, section_name + sub.get_name(), sub);
        }
    }
}
#pragma clang diagnostic pop

bool ini::write(ini::Object &ini, const char key_val_separator) {
    if (!ini.get_file_path().ends_with(".ini")) {
        std::string error = "ERROR: file \"" + ini.get_file_path() + "\" has an incompatible extension type\n";
        throw ini::Extension_error(error);
    }

    if (key_val_separator != '=' && key_val_separator != ':') {
        std::string error = "ERROR: separator \"" + std::to_string(key_val_separator) + "\" isn't supported\n";
        throw ini::Separator_error(error);
    }

    std::string content;
    for (auto &kv : ini.get_sections()) {
        try {
            section_to_string(content, key_val_separator, to_lower(const_cast<std::string &>(kv.first)), kv.second);
        } catch (std::exception &e) {
            std::cerr << e.what();
            return false;
        }
    }

    std::ofstream output_file(ini.get_file_path());
    output_file << content;
    output_file.close();

    return true;
}