#include <fstream>
#include <sstream>
#include <cstring>

#include "helpers.h"
#include "helpers.hpp"
#include "chemfiles.hpp"

#if defined(WIN32) || defined(WIN64)
#include <windows.h>
#endif

extern "C" void silent_crash_handlers() {
#if defined(WIN32) || defined(WIN64)
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
    #ifdef _MSC_VER
    _set_abort_behavior(0, _WRITE_ABORT_MSG);
    #endif
#endif
}

extern "C" char* read_whole_file(const char* path) {
    std::ifstream file(path);
    std::stringstream stream;
    stream << file.rdbuf();
    std::string buffer(stream.str());

    size_t pos = 0;
    while((pos = buffer.find("\r\n", pos)) != std::string::npos) {
        buffer.replace(pos, 2, "\n");
    }

    char* result = static_cast<char*>(malloc(sizeof(char) * buffer.length() + 1));
    std::strcpy(result, buffer.c_str());
    return result;
}

bool approx_eq(const chemfiles::Vector3D& a, const chemfiles::Vector3D& b, double tolerance) {
    return (fabs(a[0] - b[0]) < tolerance)
        && (fabs(a[1] - b[1]) < tolerance)
        && (fabs(a[2] - b[2]) < tolerance);
}
