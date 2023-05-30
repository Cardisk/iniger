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

bool ini::write(ini::Object &ini, const char key_val_separator) {
    if (!ini.get_file_path().ends_with(".ini")) {
        std::string error = "ERROR: file \"" + ini.get_file_path() + "\" has an incompatible extension type\n";
        throw ini::Extension_error(error);
    }

    if (key_val_separator != '=' && key_val_separator != ':') {
        std::string error = "ERROR: separator \"" + std::to_string(key_val_separator) + "\" isn't supported\n";
        std::cerr << error;
        return false;
    }

    std::string content;
    Section gl = ini.get_sections().at("global");
    if (!gl.props_empty()) {
        for (auto &kv : gl.get_props()) {
            // key symbol cannot contain "=" and ";" inside the Windows implementation.
            if (kv.first.contains("=") || kv.first.contains(";")) {
                std::cerr << "ERROR: key \"" + kv.first + R"(" cannot contain any "=" or ";" symbol)" << std::endl;
                return false;
            }
            // case-insensitive.
            content += to_lower(const_cast<std::string &>(kv.first))
                    + " " + std::to_string(key_val_separator)
                    + " ";
            // quoted values are used to explicit define spaces inside values.
            if (kv.second.contains(' ')) content += "\"";
            content += kv.second;
            if (kv.second.contains(' ')) content += "\"";
            content += "\n";
        }
    }

    // TODO: recursive algorithm to append sections and subsections
    return true;
}