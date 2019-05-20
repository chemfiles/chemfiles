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

#include "chemfiles/ErrorFmt.hpp"

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

inline void tolower(std::string& input) {
    std::transform(input.begin(), input.end(), input.begin(),
        [](std::string::value_type c) {
            return std::tolower(c);
        }
    );
}

/// Get the name of the computer used
std::string hostname();
/// Get the user name
std::string user_name();
/// Get the process current directory
std::string current_directory();

/// Convert a string to `T`, throwing a `chemfiles::Error` if the string is not
/// a valid `T`.
template<typename T> inline T parse(const std::string& string);

template<> inline double parse(const std::string& string) {
    try {
        size_t length = 0;
        double value = std::stod(string, &length);
        if (length != string.length()) {
            throw error("can not convert '{}' to a double", string);
        }
        return value;
    } catch (const std::invalid_argument&) {
        throw error("can not convert '{}' to a double", string);
    } catch (const std::out_of_range&) {
        throw error("{} is out of range for double type", string);
    }
}

template<> inline long long parse(const std::string& string) {
    try {
        size_t length = 0;
        long long int value = std::stoll(string, &length);
        if (length != string.length()) {
            throw error("can not convert '{}' to an integer", string);
        }
        return value;
    } catch (const std::invalid_argument&) {
        throw error("can not convert '{}' to an integer", string);
    } catch (const std::out_of_range&) {
        throw error("{} is out of range for long long type", string);
    }
}

template<> inline size_t parse(const std::string& string) {
    long long value = parse<long long>(string);
    if (value < 0) {
        throw error("invalid integer: should be positive, is {}", value);
    } else if (sizeof(long long) < sizeof(size_t) && value > static_cast<long long>(SIZE_MAX)) {
        throw error("{} is out of range for size_t type", value);
    } else {
        return static_cast<size_t>(value);
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
        throw error(
            "failed to read line '{}' with format '{}': {} matched out of {}",
            input, format, actual, expected
        );
    }
}

}

#endif
