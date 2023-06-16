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

typedef enum Token_Type {
    E_O_F = 0,
    IDENTIFIER = 1,
    VALUE = 2,
    SEPARATOR = 3,
    SECTION = 4,
} Token_Type;

class Token {
public:
    explicit Token(Token_Type type = E_O_F, std::string txt = "") : type(type), txt(std::move(txt)) {}

    Token_Type type;
    std::string txt;
};

class Lexer {
public:
    explicit Lexer(std::string source, std::string file_path) : source(std::move(source)),
                                                                file_path(std::move(file_path)), tokens({}) {}

    std::vector<Token> scan_tokens() {
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
                // ignore.
                break;
            case '\n':
                line++;
                break;
            default:
                if (std::isalpha(c)) {
                    while (std::isalpha(peek()) ||
                           std::isdigit(peek()) ||
                           peek() == '.' || peek() == '_')
                        advance();
                    tokens.emplace_back(IDENTIFIER, source.substr(start, current - start));
                    break;
                } else if (std::isdigit(c)) {
                    while (std::isdigit(peek()) || peek() == '.' || peek() == 'x' || peek() == 'b') advance();
                    tokens.emplace_back(VALUE, source.substr(start, current - start));
                    break;
                }

                std::cerr << "ERROR: unexpected character '" << c << "' found at '" << file_path << ":" << line
                          << "'\n";
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

class Parser {
public:
    explicit Parser(std::vector<Token> &tokens) : tokens(tokens) {}

    void parse_tokens(ini::Object &ini) {
//        std::cout << "-------------\n";
//        for (auto &t : tokens) {
//            std::cout << t.type << " - " << t.txt << std::endl;
//        }
//        std::cout << "-------------\n";
        while (!end()) {
            std::cout << "-------------\n";
            std::cout << "Working section: " << section_path << std::endl;

            if (!parse_token(ini)) {
                //ini.get_sections().clear();
                //ini.get_sections().insert(std::make_pair("global", ini::Section("global")));
                return;
            }
        }
    }

private:
    bool parse_token(ini::Object &ini) {
        Token &t = advance();
        std::cout << "parsing: " << t.txt << std::endl;
        switch (t.type) {
            case IDENTIFIER: {
                if (!match(SEPARATOR)) {
                    std::cerr << "ERROR: invalid token '" << peek().txt << "' found after '" << t.txt << "'\n";
                    return false;
                }
                std::cout << "separator: " << advance().txt << std::endl;

                if (!match(VALUE) && !match(IDENTIFIER)) {
                    std::cerr << "ERROR: invalid token '" << peek().txt << "' found after '" << t.txt << "'\n";
                    return false;
                }
                Token &v = advance();
                std::cout << "value: " << v.txt << std::endl;

                if (t.txt.contains(';') || t.txt.contains('#')
                    || t.txt.contains('=') || t.txt.contains(':') || t.txt.contains(' ')) {
                    std::cerr << "ERROR: invalid key identifier '" << t.txt << "', use only alphanumeric characters\n";
                    return false;
                }

                if (!ini::add_property(ini, section_path, to_lower(t.txt), v.txt)) {
                    std::cerr << "ERROR: could not insert '" << t.txt << "' - '" << v.txt << "' pair\n";
                    return false;
                }
            }
                break;
            case SECTION:
                if (t.txt.starts_with('.'))
                    section_path += t.txt;
                else
                    section_path = t.txt;
                break;
            default:
                std::cerr << "ERROR: something wrong happened\n";
                return false;
        }
        return true;
    }

    bool match(Token_Type type) {
        if (end()) return false;
        return tokens[current].type == type;
    }

    Token &advance() {
        return tokens[current++];
    }

    Token &peek() {
        return tokens[current];
    }

    bool end() {
        return current >= tokens.size();
    }

    std::vector<Token> tokens;
    int current = 0;
    std::string section_path = "global";
};

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
void section_to_string(std::string &str, const char key_val_separator, const std::string &section_name, ini::Section &s) {
    if (section_name != "global") str += "[" + section_name + "]\n";

    if (!s.props_empty()) {
        for (auto &kv: s.get_props()) {
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
        for (auto &sub: s.get_subsecs()) {
            section_to_string(str, key_val_separator, section_name + "." + sub.first, sub.second);
        }
    }
}
#pragma clang diagnostic pop

bool ini::add_property(ini::Object &ini, const std::string &section_path, const std::string &key, const std::string &value) {
    Section &sec = ini.get_global();

    if (!section_path.empty() && section_path != "global") {
        auto path = string_split(const_cast<std::string &>(section_path), ".");

        for (auto &i : path) {
            try {
                sec = sec.get_subsecs().at(i);
            } catch (std::exception &e) {
                if (!ini::add_section(ini, i,
                                      section_path.substr(0, section_path.find(i)))) {
                    std::cerr << "ERROR: could not create new section '" << i << "'\n";
                    return false;
                }
            }
        }
    }

    const_cast<std::string &>(key) = to_lower(const_cast<std::string &>(key));

//    std::cout << sec.get_name() << std::endl;
//    std::cout << "***************\n";
//    for (auto &kv : sec.get_props()) {
//        std::cout << kv.first << " - " << kv.second << std::endl;
//    }
//    std::cout << "***************\n";

    // case-insensitive.
    return sec.get_props().insert(std::make_pair(key, value)).second;
}

bool ini::add_section(ini::Object &ini, const std::string &new_section_name, const std::string &section_path = "") {
    ini::Section sec;
    if (section_path.empty()) {
        sec.set_name(new_section_name);
        return ini.get_global().get_subsecs().insert(std::make_pair(new_section_name, sec)).second;
    }

    auto path = string_split(const_cast<std::string &>(section_path), ".");

    ini::Section &s = ini.get_global();
    for (auto &i: path) {
        try {
            sec = s.get_subsecs().at(i);
        } catch (std::out_of_range &e) {
            std::cerr << "EXCEPTION: " << e.what() << std::endl;
            return false;
        }
    }

    return sec.get_subsecs().insert(std::make_pair(new_section_name, Section(new_section_name))).second;
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
    Parser parser(t);
    parser.parse_tokens(ini);

    // DEBUG
    std::cout << "-------------\nOutput:\n\n";
    for (auto &okv : ini.get_global().get_props()) {
        std::cout << okv.first << " -> " << okv.second << std::endl;
    }
    for (auto &okv : ini.get_global().get_subsecs()) {
        std::cout << "[" << okv.first << "]\n";
        for (auto &kv: okv.second.get_props()) {
            std::cout << kv.first << " -> " << kv.second << std::endl;
        }
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
    section_to_string(content, key_val_separator, "global", ini.get_global());
    for (auto &kv: ini.get_global().get_subsecs()) {
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