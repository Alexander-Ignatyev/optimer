// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#ifndef COMMON_ALGO_STRING_H_
#define COMMON_ALGO_STRING_H_

#include <cctype>

#include <string>
#include <vector>
#include <locale>
#include <sstream>
#include <algorithm>
#include <functional>

// the code of split functions is based on
// http://stackoverflow.com/a/236803

inline std::vector<std::string> &split(const std::string &s, char delim
    , std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

inline std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

// the code of trim functions is based on
// http://stackoverflow.com/a/217605

// trim from start
inline std::string &ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end()
        , std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim from end
inline std::string &rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend()
        , std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

// trim from both ends
inline std::string &trim(std::string &s) {
    return ltrim(rtrim(s));
}

#endif  // COMMON_ALGO_STRING_H_
