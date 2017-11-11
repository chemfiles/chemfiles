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

    auto name = atom.full_name();
    // dereferencing an optional<T> to access the value
    assert(*name == "Carbon");
    // or use the `value` member functions
    assert(name.value() == "Carbon");

    // matching is performed with case-insensitive search
    atom = Atom("C23a", "c");
    assert(atom.full_name().value() == "Carbon");

    // the radius is guessed from the atom type, the atom name is ignored
    atom = Atom("C", "Baz");
    name = atom.full_name();
    assert(!name);

    // [example]
}
