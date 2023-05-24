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
 * all the properties declared before any section are defined as "global".
 *
 * ":" can be used instead of "=".
 *
 * section nesting is allowed with the usage of a "." notation. ✅
 * relative nesting is allowed without specifying the upper section.
 *
 * "#" can be used instead of ";" to declare a comment.
 *
 * duplicate definition of the same property may cause an abort, override the older value or define a multi-value. ✅
 *
 * duplicate definition of a section will merge the properties.
 *
 * quoted values are used to explicit define spaces inside values.
 */

#include <string>
#include <vector>
#include <unordered_map>

namespace ini {
    class Section {
    private:
        std::string sec_name;
        std::unordered_map<std::string, std::string> props;
        std::vector<Section> subsecs;
    };

    class Object {
    private:
        std::string file_path;
        std::unordered_map<std::string, Section> sections;
    };

    Object read(const std::string &path);
    void write(Object &ini);
}

#endif //INIGER_H
