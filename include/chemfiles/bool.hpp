/* Chemfiles, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
 */

#ifndef CHEMFFILES_BOOL_HPP
#define CHEMFFILES_BOOL_HPP

namespace chemfiles {

//! Because the standard spacialization `std::vector<bool>` is optimized for
//! space requirements and hard to work with, chemfiles uses a small wrapper
//! around the  `bool` type, the `Bool` class.
class Bool {
public:
    Bool() noexcept = default;
    Bool(const Bool& other) noexcept = default;
    Bool(Bool&& other) noexcept = default;
    Bool& operator=(const Bool& other) noexcept = default;
    Bool& operator=(Bool&& other) noexcept = default;

    //! Construct a `Bool` from a given `bool` value
    Bool(bool value) : value_(value) {}
    //! Negate this boolean
    bool operator!() const { return !value_; }
    //! Convert this boolean to the `bool` standard type
    operator bool() const { return value_; }

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
