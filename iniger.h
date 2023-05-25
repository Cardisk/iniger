//
// Created by Matteo Cardinaletti on 22/05/23.
//

#ifndef INIGER_H
#define INIGER_H

/*
 * ORIGINAL PARSER:
 *
 * key symbol cannot contain "=" and ";" inside the Windows implementation.
 *
 * [symbol] defines a section. There is no section ending until the EOF or another section declaration. ✅
 * originally sections cannot be nested.
 *
 * case-insensitive.
 *
 * ";" at the beginning defines a comment that will be ignored.
 *
 * the order of sections and properties is irrelevant. ✅
 *
 * DERIVED FEATURES:
 *
 * all the properties declared before any section are defined as "global". ✅
 *
 * ":" can be used instead of "=".
 *
 * section nesting is allowed with the usage of a "." notation. ✅
 * relative nesting is allowed without specifying the upper section.
 *
 * "#" can be used instead of ";" to declare a comment.
 *
 * duplicate definition of the same property may cause an abort, override the older value or define a multi-value. (abort) ✅
 *
 * duplicate definition of a section will merge the properties. ✅
 *
 * quoted values are used to explicit define spaces inside values.
 */

#include <string>
#include <utility>
#include <vector>
#include <unordered_map>

namespace ini {
    class Section {
    public:
        explicit Section() = default;
        explicit Section(std::string sec_name) : sec_name(std::move(sec_name)) {}

        [[nodiscard]] bool props_empty() const {
            return props.empty();
        }

        [[nodiscard]] bool subsecs_empty() const {
            return props.empty();
        }

        [[nodiscard]] const std::string &get_name() const {
            return this->sec_name;
        }

        [[nodiscard]] std::unordered_map<std::string, std::string> &get_props() {
            return this->props;
        }

        [[nodiscard]] std::vector<Section> &get_subsecs() {
            return this->subsecs;
        }

    private:
        std::string sec_name;
        std::unordered_map<std::string, std::string> props;
        std::vector<Section> subsecs;
    };

    class Object {
    public:
        explicit Object(std::string file_path) : file_path(std::move(file_path)) {
            sections.insert(std::make_pair("global", Section("global")));
        }

        [[nodiscard]] const std::string &get_file_path() const {
            return file_path;
        }

        [[nodiscard]] const std::unordered_map<std::string, Section> &get_sections() const {
            return sections;
        }

    private:
        std::string file_path;
        std::unordered_map<std::string, Section> sections;
    };

    class Extension_error : public std::exception {
    public:
        explicit Extension_error() = default;
        explicit Extension_error(std::string &msg) : m_msg(std::move(msg)) {}

        [[nodiscard]] const char *what() const noexcept override {
            return m_msg.c_str();
        }

    private:
        std::string m_msg;
    };

    bool add_property(Object &ini, std::string &section_path, const std::string &key, const std::string &value);

    bool add_section(Object &ini, std::string &section_path, const std::string &new_section_name);

    Object read(const std::string &path);

    void write(Object &ini);
}

#endif //INIGER_H
