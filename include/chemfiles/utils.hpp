// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_UTILS_HPP
#define CHEMFILES_UTILS_HPP

#include <vector>
#include <string>
#include <sstream>
#include <cctype>
#include <algorithm>

namespace chemfiles {

inline std::vector<std::string> split(const std::string &s, char delim) {
    std::stringstream ss(s);
    std::string item;
    std::vector<std::string> elems;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

inline std::string trim(const std::string& str) {
    auto front = std::find_if_not(str.begin(), str.end(), [](int c) {
        return std::isspace(c);
    });
    auto back = std::find_if_not(str.rbegin(), str.rend(), [](int c) {
        return std::isspace(c);
    }).base();
    return (back <= front ? std::string() : std::string(front, back));
}

/// Get the name of the computer used
std::string hostname();
/// Get the user name
std::string user_name();
/// Get the process current directory
std::string current_directory();

#ifndef __has_builtin
  #define __has_builtin(x) 0
#endif

#if __has_builtin(__builtin_unreachable)
    #define unreachable() __builtin_unreachable()
#elif GCC_VERSION >= 40500
    #define unreachable() __builtin_unreachable()
#elif defined(_MSC_VER)
    #define unreachable() __assume(false)
#else
    #define unreachable() do {                     \
        throw Error("Entered unreachable code");   \
    } while (false)
#endif
}

#endif
