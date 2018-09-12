// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_UTILS_HPP
#define CHEMFILES_UTILS_HPP

#include <vector>
#include <string>
#include <sstream>
#include <cctype>
#include <cstdarg>
#include <algorithm>

#include "chemfiles/Error.hpp"

namespace chemfiles {

inline std::vector<std::string> split(const std::string &s, char delim) {
    std::stringstream ss(s);
    std::string item;
    std::vector<std::string> elems;
    while (std::getline(ss, item, delim)) {
        if (item != "") {
            elems.push_back(item);
        }
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
            throw chemfiles::Error("can not convert '" + string + "' to number");
        }
        return value;
    } catch (const std::invalid_argument&) {
        throw chemfiles::Error("can not convert '" + string + "' to number");
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
            throw chemfiles::Error("can not convert '" + string + "' to number");
        }
        return value;
    } catch (const std::invalid_argument&) {
        throw chemfiles::Error("can not convert '" + string + "' to number");
    } catch (const std::out_of_range&) {
        throw chemfiles::Error("'" + string + "' is out of range for long long int type");
    }
}

/// A checked version of sscanf: the return value of sscanf is checked to see if
/// all the argument where matched.
#ifdef __GNUC__
__attribute__((format(scanf, 2, 3)))
#endif
inline void scan(const std::string& input, const char* format, ...) {
    va_list vlist;
    va_start(vlist, format);
    int expected = 0;
    char c = format[0];
    for (size_t i = 0; c != 0; i++, c = format[i]) {
        if (c == '%') {
            // Do not count %n specifiers, or %* specifiers.
            if (format[i + 1] == 'n' || format[i + 1] == '*') {
                // We can access format[i + 1] safely, because we did not reach
                // the null-terminator yet
                continue;
            }
            expected += 1;
        }
    }
    auto actual = std::vsscanf(input.c_str(), format, vlist);
    if (actual != expected) {
        throw chemfiles::Error(
            "failed to read line '" + input + "' with format '" +
            std::string(format) + "': " + std::to_string(actual) +
            " matched out of " + std::to_string(expected)
        );
    }
}

}

#endif
