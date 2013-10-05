// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#include "ini_file.h"

#include <fstream>

#include <common/algo_string.h>

const std::string IniSection::empty_string_;

std::string &IniSection::operator[](const std::string &key) {
    return key_values_[key];
}

const std::string &IniSection::operator[](const std::string &key) const {
    auto pos = key_values_.find(key);
    if (pos == key_values_.end()) {
        return empty_string_;
    } else {
        return pos->second;
    }
}

const std::unordered_map<std::string, std::string> &IniSection::data() const {
    return key_values_;
}

const std::string IniFile::default_section_name_ = "default";
const IniSection IniFile::empty_section_;

IniFile::IniFile(std::istream &is): num_parse_errors_(0) {
    std::string line;
    std::string section_name = default_section_name_;
    while (std::getline(is, line)) {
        line = trim(line);
        if (line.front() == '[') {
            if (line.back() == ']') {
                section_name = line.substr(1, line.size() - 2);
            } else {
                ++num_parse_errors_;
            }
        } else if (!line.empty()) {
            std::vector<std::string> pair = split(line, '=');
            if (pair.size() == 2) {
                sections_[section_name][trim(pair[0])] = trim(pair[1]);
            } else {
                ++num_parse_errors_;
            }
        }
    }
}

const IniSection &IniFile::operator[](const std::string &section_name) const {
    auto pos = sections_.find(section_name);
    if (pos != sections_.end()) {
        return pos->second;
    } else {
        return empty_section_;
    }
}
const IniSection &IniFile::default_section() const {
    return operator[](default_section_name_);
}
