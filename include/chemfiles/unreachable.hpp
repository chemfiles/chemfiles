// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_UNREACHABLE_HPP
#define CHEMFILES_UNREACHABLE_HPP

#ifndef __has_builtin
  #define __has_builtin(x) 0
#endif

#ifdef __GNUC__
#define GCC_VERSION (__GNUC__ * 100 + __GNUC_MINOR__)
#else
#define GCC_VERSION 0
#endif

#if __has_builtin(__builtin_unreachable) || GCC_VERSION >= 405
    #define HAS_BUILTIN_UNREACHABLE 1
#else
    #define HAS_BUILTIN_UNREACHABLE 0
#endif

#undef GCC_VERSION

#if !HAS_BUILTIN_UNREACHABLE && !defined(_MSC_VER)
#include "chemfiles/Error.hpp"
#endif

namespace chemfiles {

[[noreturn]] inline void unreachable() {
#if HAS_BUILTIN_UNREACHABLE
    __builtin_unreachable();
#elif defined(_MSC_VER)
    __assume(false);
#else
    throw Error("entered unreachable code");
#endif
}

#undef HAS_BUILTIN_UNREACHABLE

}

#endif
