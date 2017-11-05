// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

#undef assert
#define assert CHECK

TEST_CASE() {
    // [example]
    auto frame = Frame();
    frame.resize(10);
    assert(frame.size() == 10);

    // reserve allocate memory, but does not change the size
    frame.reserve(100);
    assert(frame.size() == 10);
    // [example]
}
