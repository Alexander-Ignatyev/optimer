// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#ifndef COMMON_INI_FILE_H_
#define COMMON_INI_FILE_H_

#include <iosfwd>
#include <string>
#include <unordered_map>

class IniSection {
 public:
    IniSection() {}
    std::string &operator[](const std::string &key);
    const std::string &operator[](const std::string &key) const;

    const std::unordered_map<std::string, std::string> &data() const;
 private:
    static const std::string empty_string_;
    std::unordered_map<std::string, std::string> key_values_;
};

class IniFile {
 public:
    explicit IniFile(std::istream &is);
    const IniSection &operator[](const std::string &section_name) const;
    const IniSection &default_section() const;
    unsigned num_parse_errors() const {
        return num_parse_errors_;
    }
 private:
    static const std::string default_section_name_;
    static const IniSection empty_section_;

    unsigned num_parse_errors_;
    std::unordered_map<std::string, IniSection> sections_;
};

#endif  // COMMON_INI_FILE_H_
