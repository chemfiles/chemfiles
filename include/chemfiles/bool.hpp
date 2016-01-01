/* Chemfiles, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
 */

#ifndef CHFL_BOOL_HPP
#define CHFL_BOOL_HPP

namespace chemfiles {

//! Bool wrapper, to use in a std::vector in order to remove limitations of
//! std::vector<bool>
class Bool {
public:
    Bool() noexcept = default;
    Bool(const Bool& other) noexcept = default;
    Bool(Bool&& other) noexcept = default;
    Bool& operator=(const Bool& other) noexcept = default;
    Bool& operator=(Bool&& other) noexcept = default;

    Bool(bool value): value_(value) {}

    bool operator!() const {return !value_;}
    operator bool() const {return value_;}
    operator bool&() {return value_;}
private:
    bool value_;
};

inline Bool operator||(const Bool& lhs, const Bool& rhs) {
    return static_cast<bool>(lhs) || static_cast<bool>(rhs);
}

inline Bool operator&&(const Bool& lhs, const Bool& rhs) {
    return static_cast<bool>(lhs) && static_cast<bool>(rhs);
}

} // namespace chemfiles

#endif
