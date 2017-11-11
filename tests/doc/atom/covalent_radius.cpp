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

    auto radius = atom.covalent_radius();
    // dereferencing an optional<T> to access the value
    assert(*radius == 0.77);
    // or use the `value` member functions
    assert(radius.value() == 0.77);

    // matching is performed with case-insensitive search
    atom = Atom("C23a", "c");
    assert(atom.covalent_radius().value() == 0.77);

    // the radius is guessed from the atom type, the atom name is ignored
    atom = Atom("C", "Baz");
    radius = atom.covalent_radius();
    assert(!radius);

    // [example]
}
