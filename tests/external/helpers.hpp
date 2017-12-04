// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_TESTS_HELPERS_HPP
#define CHEMFILES_TESTS_HELPERS_HPP

#include <string>

namespace chemfiles {
    class Vector3D;
    class Matrix3D;
}

bool approx_eq(const chemfiles::Vector3D& lhs, const chemfiles::Vector3D& rhs, double tolerance=1e-15);
bool approx_eq(const chemfiles::Matrix3D& lhs, const chemfiles::Matrix3D& rhs, double tolerance=1e-15);
bool approx_eq(double a, double b, double tolerance=1e-15);

/// A temporary file path with an user specified extension, automatically
/// deleted at scope exit.
class NamedTempPath {
public:
    NamedTempPath(std::string extension);
    ~NamedTempPath();

    NamedTempPath(NamedTempPath&&) = default;
    NamedTempPath& operator=(NamedTempPath&&) = default;

    // Convert the path to a std::string
    operator std::string() const {return path_;}

    // Convert the path to a const char*
    const char* c_str() const {return path_.c_str();}

private:
    std::string path_;
};

// On Windows, disable the "Application error" dialog box, because it
// requires an human intervention, and there is no one on Appveyor.
//
// On UNIX, does nothing
void silent_crash_handlers();

// Check return status of C API functions
#define CHECK_STATUS(expr) CHECK(expr == CHFL_SUCCESS);

#endif
