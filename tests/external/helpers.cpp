// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#define CATCH_CONFIG_RUNNER
#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles.hpp"

#include <cstdlib>
#include <cstdio>
#include <stdexcept>
#include <new>

#ifdef CHEMFILES_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <unistd.h>
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
    return fabs(a - b) < tolerance;
}

bool is_valgrind_and_travis() {
    auto travis = std::getenv("TRAVIS");
    auto valgrind = std::getenv("CHFL_TESTS_USE_VALGRIND");
    return (travis && valgrind && std::string(valgrind) == "ON");
}

NamedTempPath::NamedTempPath(std::string extension) {
#ifdef CHEMFILES_WINDOWS
    char temp_path[MAX_PATH + 1] = {0};
    auto status = GetTempPathA(MAX_PATH + 1, temp_path);
    if (status == 0) {
        throw std::runtime_error("failed to get temporary files directory");
    }

    auto attributes = GetFileAttributesA(temp_path);
    if (attributes == INVALID_FILE_ATTRIBUTES || !(attributes & FILE_ATTRIBUTE_DIRECTORY)) {
        throw std::runtime_error("temporary files directory '" + std::string(temp_path) + "' do not exists");
    }

    char buffer[MAX_PATH] = {0};
    status = GetTempFileNameA(temp_path, "chemfiles-tmp-", 0, buffer);
    if (status == 0) {
        throw std::runtime_error("failed to get a temporary file name");
    }
#else
    char buffer[] = "/tmp/chemfiles-tmp-XXXXXX";
    auto status = mkstemp(buffer);
    if (status == -1) {
        throw std::runtime_error("failed to get a temporary file name");
    }
#endif

    path_ = buffer + extension;
}

NamedTempPath::~NamedTempPath() {
    remove(path_.c_str());
}

void copy_file(std::string src, std::string dst) {
    std::ifstream input(src, std::ios::binary);
    std::ofstream output(dst, std::ios::binary);

    output << input.rdbuf();
}

static bool FAIL_NEXT_ALLOCATION = false;

void fail_next_allocation() {
    FAIL_NEXT_ALLOCATION = true;
}

// Replace global operator new/operator delete so that we can force them to
// fail as needed
void* operator new(size_t count, const std::nothrow_t&) noexcept {
    if (FAIL_NEXT_ALLOCATION) {
        FAIL_NEXT_ALLOCATION = false;
        return nullptr;
    }

    return std::malloc(count);
}

void* operator new(size_t count) {
    void* ptr = operator new(count, std::nothrow_t{});
    if (ptr != nullptr) {
        return ptr;
    } else {
        throw std::bad_alloc();
    }
}

void operator delete(void* ptr) noexcept {
    std::free(ptr);
}

void* operator new[](size_t count) {
    return operator new(count);
}

void operator delete[](void* ptr) noexcept {
    operator delete(ptr);
}
