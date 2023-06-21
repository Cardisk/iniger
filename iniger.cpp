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

typedef enum ini_Token_Type {
    E_O_F = 0,
    IDENTIFIER = 1,
    SEPARATOR = 2,
    SECTION = 3,
} ini_Token_Type;

class ini_Token {
public:
    explicit ini_Token(ini_Token_Type type = E_O_F, std::string txt = "") : type(type), txt(std::move(txt)) {}

    ini_Token_Type type;
    std::string txt;
};

class ini_Lexer {
public:
    explicit ini_Lexer(std::string source, std::string file_path) : source(std::move(source)),
                                                                    file_path(std::move(file_path)), tokens({}) {}

    std::vector<ini_Token> scan_tokens() {
        while (!end()) {
            start = current;
            if (!scan_token()) {
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
                    std::cerr << "[ERROR]: unclosed section definition inside '" << file_path << "'\n";
                    return false;
                }
                advance();
                tokens.emplace_back(SECTION, source.substr(start + 1, current - (start + 1) - 1));
                break;
            case '"':
                while (peek() != '"' && !end()) advance();
                if (end()) {
                    std::cerr << "[ERROR]: unclosed string definition inside '" << file_path << "'\n";
                    return false;
                }
                advance();
                tokens.emplace_back(IDENTIFIER, source.substr(start, current - (start + 1) - 1));
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
                // ignore.
                break;
            case '\n':
                line++;
                break;
            default:
                if (std::isalnum(static_cast<unsigned char>(c))) {
                    while (std::isalnum(static_cast<unsigned char>(peek())) ||
                            peek() == '.' || peek() == '_') {
                        advance();
                    }
                    tokens.emplace_back(IDENTIFIER, source.substr(start, current - start));
                    break;
                }

                std::cerr << "[ERROR]: unexpected character '" << c << "' found at '" << file_path << ":" << line
                          << "'\n";
                return false;
        }
        return true;
    }

    bool end() {
        return current >= source.size();
    }

    const std::string source;
    std::vector<ini_Token> tokens;
    std::string file_path;
    int line = 1;
    int start = 0;
    int current = 0;
};


class ini_Parser {
public:
    explicit ini_Parser(std::vector<ini_Token> &tokens) : tokens(tokens) {}

    bool parse_tokens(ini::Object &ini) {
        while (!end()) {
            if (!parse_token(ini)) return false;
        }

        return true;
    }

private:
    bool parse_token(ini::Object &ini) {
        ini_Token &t = advance();
        switch (t.type) {
            case IDENTIFIER: {
                if (!match(SEPARATOR)) {
                    std::cerr << "[ERROR]: invalid token '" << peek().txt << "' found after '" << t.txt << "'\n";
                    return false;
                }
                advance();

                if (!match(IDENTIFIER)) {
                    std::cerr << "[ERROR]: invalid token '" << peek().txt << "' found after '" << t.txt << "'\n";
                    return false;
                }
                ini_Token &v = advance();

                if (t.txt.contains(';') || t.txt.contains('#')
                    || t.txt.contains('=') || t.txt.contains(':') || t.txt.contains(' ')) {
                    std::cerr << "[ERROR]: invalid key identifier '" << t.txt << "', use only alphanumeric characters\n";
                    return false;
                }

                if (!ini::add_property(ini, to_lower(t.txt), v.txt, section_path)) {
                    std::cerr << "[ERROR]: something happened during '" << t.txt << "' -> '" << v.txt << "' insertion\n";
                    return false;
                }
            }
                break;
            case SECTION:
                if (!t.txt.starts_with('.')) {
                    section_path = t.txt;
                    break;
                }

                if (section_path.empty()) {
                    std::cerr << "[ERROR]: relative nesting of '" << t.txt << "' can't be performed, missing parent section\n";
                    return false;
                }
                section_path += t.txt;
                break;
            default:
                std::cerr << "[ERROR]: something wrong happened\n";
                return false;
        }
        return true;
    }

    bool match(ini_Token_Type type) {
        if (end()) return false;
        return tokens[current].type == type;
    }

    ini_Token &advance() {
        return tokens[current++];
    }

    ini_Token &peek() {
        return tokens[current];
    }

    bool end() {
        return current >= tokens.size();
    }

    std::vector<ini_Token> tokens;
    int current = 0;
    std::string section_path;
};

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

// sec_name empty == ini.get_global()
void ini_section_to_string(std::string &str, const char kvs, ini::Section &sec, const std::string &sec_name = "") {
    if (!sec_name.empty()) str += "[" + sec_name + "]\n";

    for (auto &kv : sec.get_props()) {
        str += kv.first;
        str.push_back(kvs);
        if (kv.second.contains(' ')) str += " \"" + kv.second + "\"";
        else str += " " + kv.second;
        str += "\n";
    }

    str += "\n";
    if (!sec_name.empty()) {
        for (auto &kv : sec.get_subsecs()) {
            ini_section_to_string(str, kvs, kv.second, sec_name + "." + kv.first);
        }
    }
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
    if (key.empty() || value.empty()) return false;

    if (key.contains('=') || key.contains(';')) {
        std::cerr << "[ERROR]: key symbol cannot contain \"=\" and \";\" inside the Windows implementation\n";
        return false;
    }

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

std::string &ini::get_property(ini::Object &ini, std::string &key, std::string &section_path) {
    ini::Section *sec = &ini.get_global();

    if (!section_path.empty()) {
        auto path = string_split(section_path, ".");
        for (auto &i : path) {
            try {
                i = to_lower(i);
                sec = &sec->get_subsecs().at(i);
            } catch (std::out_of_range &e) {
                throw std::out_of_range("ini::get_property: missing section '" + i + "'");
            }
        }
    }

    return sec->get_props().at(key);
}

std::string &ini::get_property(ini::Object &ini, std::string &key, std::string &&section_path) {
    return ini::get_property(ini, key, section_path);
}

std::string &ini::get_property(ini::Object &ini, std::string &&key, std::string &section_path) {
    return ini::get_property(ini, key, section_path);
}

std::string &ini::get_property(ini::Object &ini, std::string &&key, std::string &&section_path) {
    return ini::get_property(ini, key, section_path);
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
    if (new_section_name.empty()) return false;

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

ini::Section &ini::get_section(ini::Object &ini, std::string &section_name, std::string &section_path) {
    ini::Section *sec = &ini.get_global();

    if (!section_path.empty()) {
        auto path = string_split(section_path, ".");
        for (auto &i : path) {
            try {
                i = to_lower(i);
                sec = &sec->get_subsecs().at(i);
            } catch (std::out_of_range &e) {
                throw std::out_of_range("ini::get_section: missing section '" + i + "'");
            }
        }
    }

    section_name = to_lower(section_name);
    return sec->get_subsecs().at(section_name);
}

ini::Section &ini::get_section(ini::Object &ini, std::string &section_name, std::string &&section_path) {
    return ini::get_section(ini, section_name, section_path);
}

ini::Section &ini::get_section(ini::Object &ini, std::string &&section_name, std::string &section_path) {
    return ini::get_section(ini, section_name, section_path);
}

ini::Section &ini::get_section(ini::Object &ini, std::string &&section_name, std::string &&section_path) {
    return ini::get_section(ini, section_name, section_path);
}

ini::Object ini::read(std::string &path) {
    ini::Object ini(path);

    if (!path.ends_with(".ini")) {
        std::cerr << "ERROR: file \"" + ini.get_file_path() + "\" has an incompatible extension type\n";
        return ini;
    }

    if (!ini::read(ini)) {
        std::cerr << "[ERROR]: failed during file reading\n";
    }

    return ini;
}

ini::Object ini::read(std::string &&path) {
    return ini::read(path);
}

bool ini::read(ini::Object &ini) {
    std::string text;

    std::ifstream file;
    file.open(ini.get_file_path());

    if (!file.is_open()) {
        std::cerr << "[ERROR]: failed to open '" << ini.get_file_path() << "'\n";
        return false;
    }

    std::string line;
    while (file.good()) {
        std::getline(file, line);
        text += line + "\n";
    }
    file.close();

    // lexing.
    ini_Lexer lexer(text, ini.get_file_path());
    auto tokens = lexer.scan_tokens();

    // parsing.
    ini_Parser parser(tokens);
    return parser.parse_tokens(ini);
}

bool ini::write(ini::Object &ini, const char key_val_separator) {
    if (!ini.get_file_path().ends_with(".ini")) {
        std::cerr << "ERROR: file \"" + ini.get_file_path() + "\" has an incompatible extension type\n";
        return false;
    }

    if (key_val_separator != '=' && key_val_separator != ':') {
        std::cerr << "ERROR: separator \"" + std::to_string(key_val_separator) + "\" isn't supported\n";
        return false;
    }

    std::string content;
    ini_section_to_string(content, key_val_separator, ini.get_global());
    for (auto &kv: ini.get_global().get_subsecs()) {
        ini_section_to_string(content, key_val_separator, kv.second, kv.first);
    }

    std::ofstream output_file(ini.get_file_path());
    output_file << content;
    output_file.close();

    return true;
}