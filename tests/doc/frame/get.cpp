// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <chemfiles.hpp>
using namespace chemfiles;

int main() {
    // [example]
    auto frame = Frame();
    frame.set("foo", Property(23));

    assert(frame.get("foo"));
    assert(frame.get("foo")->as_double() == 23);

    assert(!frame.get("bar"));
    // [example]
    return 0;
}
