// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <limits>
#include <cstdint>

#include "chemfiles/parse.hpp"
#include "chemfiles/utils.hpp"
#include "chemfiles/error_fmt.hpp"
#include "chemfiles/string_view.hpp"

using namespace chemfiles;

static bool is_digit(char c) { return '0' <= c && c <= '9'; }

template <> int64_t chemfiles::parse(string_view input) {
    if (input.empty()) {
        throw error("can not parse an integer from an empty string");
    }

    auto it = input.begin();
    auto end = input.end();

    // skip whitespaces before number
    while (it != end && is_whitespace(*it)) {
        it++;
    }

    int sign = 1;
    // Parse sign, if any
    if (it != end && *it == '-') {
        sign = -1;
        it++;
    } else if (it != end && *it == '+') {
        it++;
    }

    int64_t result = 0;
    for (/*no initialization*/; it!=end; it++) {
        if (is_digit(*it)) {
            int64_t digit = static_cast<int64_t>(*it - '0');
            if (sign == -1) {
                // Check for underflow
                if (result >= ((std::numeric_limits<int64_t>::min() + digit) / 10))
                    result = result * 10 - digit;
                else {
                    throw error("{} is out of range for 64-bit integer", input);
                }
            } else {
                // Check for overflow
                if (result <= ((std::numeric_limits<int64_t>::max() - digit) / 10))
                    result = result * 10 + digit;
                else {
                    throw error("{} is out of range for 64-bit integer", input);
                }
            }
        } else {
            // we found the end of the number
            break;
        }
    }

    // skip whitespaces after number if any
    while (it != end && is_whitespace(*it)) {
        it++;
    }

    if (it != end) {
        throw error("can not parse '{}' as an integer", input);
    }

    return result;
}

template <> uint64_t chemfiles::parse(string_view input) {
    if (input.empty()) {
        throw error("can not parse an integer from an empty string");
    }

    auto it = input.begin();
    auto end = input.end();

    // skip whitespaces before number
    while (it != end && is_whitespace(*it)) {
        it++;
    }

    // Parse sign, if any
    if (it != end && *it == '+') {
        it++;
    }

    uint64_t result = 0;
    for (/*no initialization*/; it!=end; it++) {
        if (is_digit(*it)) {
            uint64_t digit = static_cast<uint64_t>(*it - '0');
            // Check for overflow
            if (result <= ((std::numeric_limits<uint64_t>::max() - digit) / 10))
                result = result * 10 + digit;
            else {
                throw error("{} is out of range for 64-bit unsigned integer", input);
            }
        } else {
            // we found the end of the number
            break;
        }
    }

    // skip whitespaces after number if any
    while (it != end && is_whitespace(*it)) {
        it++;
    }

    if (it != end) {
        throw error("can not parse '{}' as a positive integer", input);
    }

    return result;
}


template <> double chemfiles::parse(string_view input) {
    if (input.empty()) {
        throw error("can not parse a double from an empty string");
    }

    auto it = input.begin();
    auto end = input.end();

    // skip whitespaces before number
    while (it != end && is_whitespace(*it)) {
        it++;
    }

    // Get sign, if any.
    int sign = 1.0;
    if (it != end && *it == '-') {
        sign = -1.0;
        it++;
    } else if (it != end && *it == '+') {
        it++;
    }

    auto digit_start = it;
    // Get digits before decimal point or exponent, if any.
    double value = 0.0;
    while (it != end && is_digit(*it)) {
        value = value * 10.0 + static_cast<double>(*it - '0');
        it++;
    }
    bool got_digits = (digit_start != it);

    // Get digits after decimal point, if any.
    if (it != end && *it == '.') {
        it++;
        auto frac_start = it;
        double pow10 = 10.0;
        while (it != end && is_digit(*it)) {
            value += static_cast<double>(*it - '0') / pow10;
            pow10 *= 10.0;
            it++;
        }

        got_digits = got_digits || (frac_start != it);
    }

    // Handle exponent, if any.
    bool frac = false;
    double scale = 1.0;
    if (it != end && (*it == 'e' || *it == 'E')) {
        it++;
        auto exponent_start = it;

        // Get sign of exponent, if any.
        if (it != end && *it == '-') {
            frac = true;
            it++;
        } else if (it != end && *it == '+') {
            it++;
        }

        // Get digits of exponent, if any.
        unsigned expon = 0;
        while (it != end && is_digit(*it)) {
            unsigned digit = static_cast<unsigned>(*it - '0');
            if (expon <= ((std::numeric_limits<unsigned>::max() - digit) / 10))
                expon = expon * 10 + digit;
            else {
                throw error("float exponent in {} is out of range for unsigned integer", input);
            }

            it++;
        }

        if (it == exponent_start) {
            throw error("missing exponent in '{}'", input);
        } else if (expon > 308) {
            throw error("{} is out of range for double", input);
        }

        // Calculate scaling factor.
        while (expon >= 50) { scale *= 1e50; expon -= 50; }
        while (expon >=  8) { scale *= 1e8;  expon -=  8; }
        while (expon >   0) { scale *= 1e1;  expon -=  1; }
    }

    // skip whitespaces after number if any
    while (it != end && is_whitespace(*it)) {
        it++;
    }

    if (it != end || !got_digits) {
        throw error("can not parse '{}' as a double", input);
    }

    // Return signed and scaled floating point result.
    return sign * (frac ? (value / scale) : (value * scale));
}
