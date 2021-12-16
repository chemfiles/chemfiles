// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_TESTS_HELPERS_HPP
#define CHEMFILES_TESTS_HELPERS_HPP

#include <string>
#include <vector>
#include <cstdint>

namespace chemfiles {
    class Vector3D;
    class Matrix3D;
}

bool approx_eq(const chemfiles::Vector3D& lhs, const chemfiles::Vector3D& rhs, double tolerance=1e-15);
bool approx_eq(const chemfiles::Matrix3D& lhs, const chemfiles::Matrix3D& rhs, double tolerance=1e-15);
bool approx_eq(double a, double b, double tolerance=1e-15);

// Check if the current test is running under both valgrind and Travis
bool is_valgrind_and_ci();

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

    // Get the underlying string
    const std::string& path() const {return path_;}

private:
    std::string path_;
};

/// copy the file at `src` to `dst`
void copy_file(std::string src, std::string dst);

/// get the content of the file at `path` as a vector of bytes
std::vector<uint8_t> read_binary_file(std::string path);

/// get the content of the file at `path` as a string
std::string read_text_file(std::string path);

// On Windows, disable the "Application error" dialog box, because it
// requires an human intervention, and there is no one on Appveyor.
//
// On UNIX, does nothing
void silent_crash_handlers();

/// Fail the next allocation by throwing std::bad_alloc. This is done by
/// replacing the global operator new and operator delete, and should thus catch
/// all C++ allocations.
void fail_next_allocation();

/// Check return status of C API functions
#define CHECK_STATUS(expr) CHECK(expr == CHFL_SUCCESS)

/// run the given command using the system shell, and return the standard output
std::string run_process(std::string command);

#endif
