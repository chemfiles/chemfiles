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

bool approx_eq(const chemfiles::Vector3D& a, const chemfiles::Vector3D& b, double tolerance) {
    return (fabs(a[0] - b[0]) < tolerance)
        && (fabs(a[1] - b[1]) < tolerance)
        && (fabs(a[2] - b[2]) < tolerance);
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
