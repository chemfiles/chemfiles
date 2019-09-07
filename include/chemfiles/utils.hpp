// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_UTILS_HPP
#define CHEMFILES_UTILS_HPP

#include <cctype>
#include <cstdlib>
#include <vector>
#include <string>
#include <algorithm>

#include "chemfiles/ErrorFmt.hpp"
#include "chemfiles/string_view.hpp"

namespace chemfiles {

/// Split `string` into components delimited by `delim`, ignoring empty
/// components.
inline std::vector<string_view> split(string_view string, char delim) {
    std::vector<string_view> elems;
    size_t last = 0;
    for (size_t i = 0; i<string.length(); i++) {
        if (string[i] == delim) {
            if (last != i) {
                // Don't add empty elements
                elems.push_back(string.substr(last, i - last));
            }
            last = i + 1;
        }
    }

    if (last < string.length()) {
        elems.push_back(string.substr(last, string.length() - last));
    }

    return elems;
}

inline std::vector<string_view> split(const char* string, char delim) {
    return split(string_view(string), delim);
}

// disallow temporary string
std::vector<string_view> split(std::string&& string, char delim) = delete;

namespace detail {
    inline bool is_space(char c) {
        return c == ' ' || c == '\t' || c == '\r' || c == '\n';
    }
}

/// Remove whitespaces at the begining and end of `string`
inline string_view trim(string_view string) {
    auto begin = string.begin();
    auto end = string.end();
    while (begin != end && detail::is_space(*begin)) {
        begin++;
    }

    if (begin != end) {
        end--;
        while (end != begin && detail::is_space(*end)) {
            end--;
        }
        end++;
    }

    return string_view(begin, static_cast<size_t>(end - begin));
}

inline string_view trim(const char* string) {
    return trim(string_view(string));
}

// disallow temporary string
string_view trim(std::string&& string) = delete;

/// Transform `string` to lower case
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
template<typename T> inline T parse(string_view string);

template<> inline double parse(string_view input) {
    // Create a copy of input, since `std::strtoll` needs the string to be
    // NULL-terminated
    auto string = input.to_string();
    char* end = nullptr;
    double value = std::strtod(string.c_str(), &end);
    if (string.empty() || end != string.c_str() + string.length()) {
        throw error("can not convert '{}' to a double", string);
    }

    if (errno == ERANGE) {
        errno = 0;
        throw error("{} is out of range for double type", string);
    }

    return value;
}

template<> inline long long parse(string_view input) {
    // Create a copy of input, since `std::strtoll` needs the string to be
    // NULL-terminated
    auto string = input.to_string();
    char* end = nullptr;
    long long int value = std::strtoll(string.c_str(), &end, 10);
    if (string.empty() || end != string.c_str() + string.length()) {
        throw error("can not convert '{}' to an integer", string);
    }

    if (errno == ERANGE) {
        errno = 0;
        throw error("{} is out of range for long long type", string);
    }

    return value;
}

template<> inline size_t parse(string_view string) {
    long long value = parse<long long>(string);
    if (value < 0) {
        throw error("invalid integer: should be positive, is {}", value);
    } else if (sizeof(long long) < sizeof(size_t) && value > static_cast<long long>(SIZE_MAX)) {
        throw error("{} is out of range for size_t type", value);
    } else {
        return static_cast<size_t>(value);
    }
}

/// A checked version of sscanf: the return value of sscanf is checked
/// to see if all the argument where matched.
#ifdef __GNUC__
__attribute__((format(scanf, 2, 3)))
#endif
void scan(const std::string& input, const char* format, ...);

}

#endif
