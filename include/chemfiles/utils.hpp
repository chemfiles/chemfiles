// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_UTILS_HPP
#define CHEMFILES_UTILS_HPP

#include <cctype>
#include <vector>
#include <string>
#include <algorithm>

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

// chemfiles' definition of whitespace only includes ' ', '\t' and new lines
inline bool is_whitespace(char c) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

/// Remove whitespaces at the begining and end of `string`
inline string_view trim(string_view string) {
    auto begin = string.begin();
    auto end = string.end();
    while (begin != end && is_whitespace(*begin)) {
        begin++;
    }

    if (begin != end) {
        end--;
        while (end != begin && is_whitespace(*end)) {
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

}

#endif
