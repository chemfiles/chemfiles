// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#define CATCH_CONFIG_RUNNER
#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles.hpp"

#include <boost/filesystem.hpp>
namespace fs=boost::filesystem;

#ifdef CHEMFILES_WINDOWS
#include <windows.h>
#endif

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

int main(int argc, char* argv[]) {
#ifdef __EMSCRIPTEN__
    // Give node.js an access to the root filesystem
    EM_ASM(
        FS.mkdir('root');
        FS.mount(NODEFS, { root: '/' }, 'root');
        FS.chdir('root/' + process.cwd());
    );
#endif
    silent_crash_handlers();
    return Catch::Session().run(argc, argv);
}

void silent_crash_handlers() {
#ifdef CHEMFILES_WINDOWS
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
    #ifdef _MSC_VER
        _set_abort_behavior(0, _WRITE_ABORT_MSG);
    #endif
#endif
}

bool approx_eq(const chemfiles::Vector3D& lhs, const chemfiles::Vector3D& rhs, double tolerance) {
    return (fabs(lhs[0] - rhs[0]) < tolerance)
        && (fabs(lhs[1] - rhs[1]) < tolerance)
        && (fabs(lhs[2] - rhs[2]) < tolerance);
}

bool approx_eq(const chemfiles::Matrix3D& lhs, const chemfiles::Matrix3D& rhs, double tolerance) {
    return (fabs(lhs[0][0] - rhs[0][0]) < tolerance)
        && (fabs(lhs[0][1] - rhs[0][1]) < tolerance)
        && (fabs(lhs[0][2] - rhs[0][2]) < tolerance)
        && (fabs(lhs[1][0] - rhs[1][0]) < tolerance)
        && (fabs(lhs[1][1] - rhs[1][1]) < tolerance)
        && (fabs(lhs[1][2] - rhs[1][2]) < tolerance)
        && (fabs(lhs[2][0] - rhs[2][0]) < tolerance)
        && (fabs(lhs[2][1] - rhs[2][1]) < tolerance)
        && (fabs(lhs[2][2] - rhs[2][2]) < tolerance);
}

bool approx_eq(double a, double b, double tolerance) {
    return (a - b) < tolerance;
}

NamedTempPath::NamedTempPath(std::string extension) {
    // Maybe operator overloading have been too far?
    auto path = fs::temp_directory_path() / fs::unique_path();
    // Convert std::wstring to std::string, needed on windows. This is fine
    // because we are only using this class internally, with characters in the
    // ASCII range.
    path_ = std::string(path.native().begin(), path.native().end());
    path_ += extension;
}

NamedTempPath::~NamedTempPath() {
    remove(path_.c_str());
}
