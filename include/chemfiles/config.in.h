// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

// clang-format off
#ifndef CHEMFILES_CONFIG_HPP
#define CHEMFILES_CONFIG_HPP

/// An integer containing the major (x.0.0) version number
#define CHEMFILES_VERSION_MAJOR @CHEMFILES_VERSION_MAJOR@
/// An integer containing the minor (0.y.0) version number
#define CHEMFILES_VERSION_MINOR @CHEMFILES_VERSION_MINOR@
/// An integer containing the patch (0.0.z) version number
#define CHEMFILES_VERSION_PATCH @CHEMFILES_VERSION_PATCH@
/// The full version of chemfiles ("x.y.z"), as a string
#define CHEMFILES_VERSION "@CHEMFILES_VERSION@"

#define CHEMFILES_SIZEOF_VOID_P @CMAKE_SIZEOF_VOID_P@

/// Are we building code on Windows?
#cmakedefine CHEMFILES_WINDOWS

// Should we include GEMMI code?
#cmakedefine CHFL_DISABLE_GEMMI

/// thread_local implementation
#ifdef __cplusplus
    #if @CHFL_HAS_THREAD_LOCAL@
        #define CHFL_THREAD_LOCAL thread_local
    #else
        #define CHFL_THREAD_LOCAL
    #endif
#endif

// clang-format on

#endif
