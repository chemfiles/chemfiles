// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_UNREACHABLE_HPP
#define CHEMFILES_UNREACHABLE_HPP

#include "chemfiles/Error.hpp"

#ifndef __has_builtin
  #define __has_builtin(x) 0
#endif

namespace chemfiles {

[[noreturn]] inline void unreachable() {
#if __has_builtin(__builtin_unreachable) || GCC_VERSION >= 40500
    __builtin_unreachable();
#elif defined(_MSC_VER)
    __assume(false);
#else
    throw Error("entered unreachable code");
#endif
}

}

#endif
