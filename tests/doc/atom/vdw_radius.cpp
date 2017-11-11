// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

#undef assert
#define assert CHECK

TEST_CASE() {
    // [example]
    auto atom = Atom("C23a", "C");

    auto radius = atom.vdw_radius();
    // dereferencing an optional<T> to access the value
    assert(*radius == 1.7);
    // or use the `value` member functions
    assert(radius.value() == 1.7);

    // matching is performed with case-insensitive search
    atom = Atom("C23a", "c");
    assert(atom.vdw_radius().value() == 1.7);

    // the radius is guessed from the atom type, the atom name is ignored
    atom = Atom("C", "Baz");
    radius = atom.vdw_radius();
    assert(!radius);

    // [example]
}
