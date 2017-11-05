// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <chemfiles.hpp>
using namespace chemfiles;

int main() {
    // [example]
    auto frame = Frame();
    frame.resize(10);
    assert(frame.size() == 10);

    // reserve allocate memory, but does not change the size
    frame.reserve(100);
    assert(frame.size() == 10);
    // [example]
    return 0;
}
