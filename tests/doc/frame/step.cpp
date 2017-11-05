// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <chemfiles.hpp>
using namespace chemfiles;

int main() {
    // [example]
    auto frame = Frame();
    assert(frame.step() == 0);

    frame.set_step(424);
    assert(frame.step() == 424);
    // [example]
    return 0;
}
