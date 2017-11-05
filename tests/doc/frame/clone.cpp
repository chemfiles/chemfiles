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
    assert(frame.size() == 0);

    auto copy = frame.clone();
    copy.resize(42);

    assert(frame.size() == 0);
    assert(copy.size() == 42);
    // [example]
}
