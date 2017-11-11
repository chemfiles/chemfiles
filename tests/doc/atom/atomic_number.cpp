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

    auto number = atom.atomic_number();
    // dereferencing an optional<T> to access the value
    assert(*number == 6);
    // or use the `value` member functions
    assert(number.value() == 6);

    // matching is performed with case-insensitive search
    atom = Atom("C23a", "c");
    assert(atom.atomic_number().value() == 6);

    // the radius is guessed from the atom type, the atom name is ignored
    atom = Atom("C", "Baz");
    number = atom.atomic_number();
    assert(!number);

    // [example]
}
