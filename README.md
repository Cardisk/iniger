# INIGER

## Intro

INI file parser made in C++.

## Quick Start

Reading:
```c++
#include "iniger.h"

int main(void) {
    // this will fail if the file extension isn't '.ini'
    // if the reading process fails, this will be empty
    ini::Object ini_1 = ini::read("path/to/my_file.ini");
    
    ini::Object ini_2("path/to/my_file.ini");
    
    // this will fail if the file extension isn't '.ini'
    // if the reading process fails, this will be empty
    bool result = ini::read(ini);
    
    ...
    
    return EXIT_SUCCESS;
}
```

Writing:
```c++
#include "iniger.h"

int main(void) {
    ini::Object ini("path/to/my_file.ini");
    
    // this will be a global property
    ini::add_property(ini, "key_1", "value_1");
    // this will be under 'Foo' section
    ini::add_property(ini, "key_2", "value_2", "Foo");
    
    // this will be under 'Bar' section
    // that is under 'Foo' section
    ini::add_property(ini, "key_3", "value_3", "Foo.Bar");
                                       
    // separator has to be ':' or '='
    // this will fail if the file extension isn't '.ini'
    bool result = ini::write(ini, ':');
    
    ...
    
    return EXIT_SUCCESS;
}
```

Accessing:
```c++
#include "iniger.h"

int main(void) {
    ini::Object ini = ini::read("path/to/my_file.ini");
    
    // if the path is not specified this will be searched as a global property
    // this will throw std::out_of_range if the key does not exist
    std::string global_property_value = ini::get_property(ini, "key_1");
    
    // this will throw std::out_of_range if the key does not exist
    // this will throw std::out_of_range if a section in the path does not exist
    std::string section_property_value = ini::get_property(ini, "key_2", "Foo");
    
    // if the path is not specified this will be searched as a global subsection
    // this will throw std::out_of_range if the section does not exist
    ini::Section global_subsec = ini::get_section(ini, "Foo"); 
    
    // this will throw std::out_of_range if a section in the path is missing
    // this will throw std::out_of_range if the section does not exist
    ini::Section subsection = ini::get_section(ini, "Baz", "Foo.Bar"); 
    
    ...
    
    return EXIT_SUCCESS;
}
```

## License