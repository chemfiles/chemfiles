// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_UNREACHABLE_HPP
#define CHEMFILES_UNREACHABLE_HPP

#include "chemfiles/Error.hpp"

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
        throw Error("entered unreachable code");   \
    } while (false)
#endif

#endif
