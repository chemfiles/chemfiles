// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_UTILS_HPP
#define CHEMFILES_UTILS_HPP

#include <vector>
#include <string>
#include <sstream>
#include <cctype>
#include <algorithm>

#include "chemfiles/Error.hpp"

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

/// Convert a string to double, throwing a `chemfiles::Error` if the string is
/// not a valid double.
inline double string2double(const std::string& string) {
    try {
        size_t length = 0;
        double value = std::stod(string, &length);
        if (length != string.length()) {
            throw chemfiles::Error("Can not convert '" + string + "' to number");
        }
        return value;
    } catch (const std::invalid_argument&) {
        throw chemfiles::Error("Can not convert '" + string + "' to number");
    } catch (const std::out_of_range&) {
        throw chemfiles::Error("'" + string + "' is out of range for double type");
    }
}

/// Convert a string to long long integer, throwing a `chemfiles::Error` if the
/// string is not a valid long long int.
inline long long int string2longlong(const std::string& string) {
    try {
        size_t length = 0;
        long long int value = std::stoll(string, &length);
        if (length != string.length()) {
            throw chemfiles::Error("Can not convert '" + string + "' to number");
        }
        return value;
    } catch (const std::invalid_argument&) {
        throw chemfiles::Error("Can not convert '" + string + "' to number");
    } catch (const std::out_of_range&) {
        throw chemfiles::Error("'" + string + "' is out of range for long long int type");
    }
}

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
