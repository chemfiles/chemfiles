// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#define CATCH_CONFIG_RUNNER
#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles.hpp"

#ifdef CHEMFILES_WINDOWS
#include <windows.h>
#endif

int main(int argc, char* argv[]) {
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
