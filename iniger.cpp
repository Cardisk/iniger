//
// Created by Matteo Cardinaletti on 22/05/23.
//

#include "iniger.h"

#include <fstream>

typedef enum Token_Type {
    E_O_F,
    IDENTIFIER,
    VALUE,
    SEPARATOR,
    SECTION,
} Token_Type;

class Token {
public:
    explicit Token(Token_Type type = E_O_F, std::string txt = "") : type(type), txt(std::move(txt)) {}

    Token_Type type;
    std::string txt;
};

class Lexer {
public:
    explicit Lexer(std::string source, std::string file_path) : source(std::move(source)), file_path(std::move(file_path)), tokens({}) {}

    std::vector<Token> scan_tokens() {
        while (!end()) {
            start = current;
            if (!scan_token()) {
                std::cerr << "ERROR: something bad happened.\n";
                tokens.clear();
                return {};
            }
        }

        return tokens;
    }
private:
    char advance() {
        return source[current++];
    }

    char peek() {
        if (end()) return '\0';
        return source[current];
    }

    bool scan_token() {
        char c = advance();
        switch (c) {
            case '[':
                while (peek() != ']' && !end()) advance();
                if (end()) {
                    std::cerr << "ERROR: unclosed section definition\n";
                    return false;
                }
                advance();
                tokens.emplace_back(SECTION, source.substr(start + 1, current - (start + 1) - 1));
                break;
            case '"':
                while (peek() != '"' && !end()) advance();
                if (end()) {
                    std::cerr << "ERROR: unclosed string definition\n";
                    return false;
                }
                advance();
                tokens.emplace_back(VALUE, source.substr(start, current - (start + 1) - 1));
                break;
            case ':':
            case '=':
                tokens.emplace_back(SEPARATOR, source.substr(start, current - start));
                break;
            case ';':
            case '#':
                while (peek() != '\n' && !end()) advance();
                break;
            case ' ':
                // ignore
                break;
            case '\n':
                line++;
                break;
            default:
                if (std::isalpha(c)) {
                    while (std::isalpha(peek()) ||
                            std::isdigit(peek()) ||
                                peek() == '.' || peek() == '_') advance();
                    tokens.emplace_back(IDENTIFIER, source.substr(start, current - start));
                    break;
                } else if (std::isdigit(c)) {
                    while (std::isdigit(peek()) || peek() == '.' || peek() == 'x' || peek() == 'b') advance();
                    tokens.emplace_back(VALUE, source.substr(start, current - start));
                    break;
                }

                std::cerr << "ERROR: unexpected character '" << c << "' found at '" << file_path << ":" << line << "'\n";
                return false;
        }
        return true;
    }

    bool end() {
        return current >= source.size();
    }

    const std::string source;
    std::vector<Token> tokens;
    std::string file_path;
    int line = 1;
    int start = 0;
    int current = 0;
};


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

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
void section_to_string(std::string &str, const char key_val_separator, const std::string &section_name, ini::Section &s) {
    if (section_name != "global") str += "[" + section_name + "]\n";

    if (!s.props_empty()) {
        for (auto &kv : s.get_props()) {
            // key symbol cannot contain "=" and ";" inside the Windows implementation.
            if (kv.first.contains("=") || kv.first.contains(";")) {
                std::string error = "ERROR: key \"" + kv.first + R"(" cannot contain any "=" or ";" symbol)" + "\n";
                throw ini::Key_error(error);
            }
            // assuming case-insensitive-ness.
            str += kv.first
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

void trim_left(std::string &str) {
    while (str.starts_with(" ") || str.starts_with("\t")) {
        str.erase(0, 1);
    }
}

std::vector<std::string> tokenize(std::string &source) {
    std::vector<std::string> v;
    while (!source.empty()) {
        bool is_key = false;
        char key_val_separator;
        trim_left(source);

        size_t next_pos = source.find(' ');
        if (next_pos == std::string::npos) {
            if (!source.empty()) {
                v.push_back(source);
                source.clear();
            }
            break;
        }

        std::string str = source.substr(0, next_pos);

        // ";" or "#" at the beginning defines a comment that will be ignored.
        if (str.starts_with(';') || str.starts_with('#')) str.clear();

        if (str.ends_with(":") || str.ends_with('=')) {
            is_key = true;
            key_val_separator = *str.end();
            str.pop_back();
        }

        if (!str.empty()) v.push_back(str);
        if (is_key) v.emplace_back(std::to_string(key_val_separator));
        source.erase(0, next_pos);
    }

    return v;
}

bool ini::add_property(ini::Object &ini, const std::string &section_path, const std::string &key, const std::string &value) {
    Section &sec = ini.get_sections().at("global");

    if (section_path != "global") {
        auto path = string_split(const_cast<std::string &>(section_path), ".");
        for (auto &s: path) {
            try {
                sec = ini.get_sections().at(s);
            } catch (std::out_of_range &e) {
                return false;
            }
        }
    }

    // case-insensitive.
    return sec.get_props().insert(std::make_pair(to_lower(const_cast<std::string &>(key)), value)).second;
}

bool ini::add_section(ini::Object &ini, const std::string &new_section_name, const std::string &section_path = "") {
    Section sec;
    if (section_path.empty()) {
        sec.set_name(new_section_name);
        return ini.get_sections().insert(std::make_pair(new_section_name, sec)).second;
    }

    auto path = string_split(const_cast<std::string &>(section_path), ".");
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

    ini::Object ini(path);

    std::string text;

    std::ifstream file;
    if (!file.is_open()) file.open(path);
    if (file.is_open()) {
        std::string line;
        while (file.good()) {
            std::getline(file, line);
            text += line + "\n";
        }
    }
    file.close();

    if (text.empty()) return ini;

    Lexer lexer(text, path);
    std::vector<Token> t = lexer.scan_tokens();

    // DEBUG
    for (auto &a : t) {
        std::cout << a.txt << std::endl;
    }

    return ini;
}

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
            section_to_string(content, key_val_separator, kv.first, kv.second);
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