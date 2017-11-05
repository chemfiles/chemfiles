// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <chemfiles.hpp>
using namespace chemfiles;

int main() {
    // [example]
    auto topology = Topology();
    topology.add_atom(Atom("H"));
    topology.add_atom(Atom("O"));
    topology.add_atom(Atom("H"));

    auto frame = Frame(topology, UnitCell(25));
    assert(frame.size() == 3);
    assert(frame.cell() == UnitCell(25));
    // [example]
    return 0;
}
