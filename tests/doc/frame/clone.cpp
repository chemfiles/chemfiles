// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <chemfiles.hpp>
using namespace chemfiles;

int main() {
    // [example]
    auto frame = Frame();
    assert(frame.size() == 0);

    auto copy = frame.clone();
    copy.resize(42);

    assert(frame.size() == 0);
    assert(copy.size() == 42);
    // [example]
    return 0;
}
