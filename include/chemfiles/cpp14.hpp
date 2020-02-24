// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_CPP14_HPP
#define CHEMFILES_CPP14_HPP

#include <memory>
#include <utility>
#include <type_traits>

// Implementation of various C++14 and later helpers missing from C++11
//
// We are restricted to C++11 to be able to support older compilers, but these
// functions are helpful and can be implemented with just C++11.

namespace chemfiles {
    template<typename T, typename... Args>
    std::unique_ptr<T> make_unique(Args&& ...args) {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }

    template<bool B, class T = void>
    using enable_if_t = typename std::enable_if<B, T>::type;
}

#endif
