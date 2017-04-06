// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_TESTS_HELPERS_HPP
#define CHEMFILES_TESTS_HELPERS_HPP

#include <array>
namespace chemfiles {
    using Vector3D = std::array<double, 3>;
}

bool approx_eq(const chemfiles::Vector3D& a, const chemfiles::Vector3D& b, double tolerance=1e-15);

// On Windows, disable the "Application error" dialog box, because it
// requires an human intervention, and there is no one on Appveyor.
//
// On UNIX, does nothing
void silent_crash_handlers();

// Check return status of C API functions
#define CHECK_STATUS(expr) CHECK(expr == CHFL_SUCCESS);

#endif
