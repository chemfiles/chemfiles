// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <chemfiles.hpp>
using namespace chemfiles;

int main() {
    // [example]
    auto frame = Frame();
    frame.resize(10);
    assert(frame.size() == 10);

    // new atoms contains default data
    for (auto position: frame.positions()) {
        assert(position[0] == 0.0);
        assert(position[1] == 0.0);
        assert(position[2] == 0.0);
    }

    for (auto atom: frame.topology()) {
        assert(atom.name() == "");
    }
    // [example]
    return 0;
}
