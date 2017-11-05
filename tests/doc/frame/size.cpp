// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <chemfiles.hpp>
using namespace chemfiles;

int main() {
    // [example]
    auto frame = Frame();
    assert(frame.size() == 0);

    frame.resize(10);
    assert(frame.size() == 10);
    // [example]
    return 0;
}
