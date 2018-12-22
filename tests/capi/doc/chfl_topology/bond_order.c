// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.h>
#include <stdlib.h>
#include <assert.h>

int main() {
    // [example]
    CHFL_TOPOLOGY* topology = chfl_topology();

    CHFL_ATOM* atom = chfl_atom("F");
    for (size_t i=0; i<5; i++) {
        chfl_topology_add_atom(topology, atom);
    }
    chfl_free(atom);

    chfl_topology_bond_with_order(topology, 0, 1, CHFL_BOND_SINGLE);
    chfl_topology_bond_with_order(topology, 2, 3, CHFL_BOND_DOUBLE);

    chfl_bond_order bond_orders[2] = {0};
    chfl_topology_bond_orders(topology, bond_orders, 2);
    assert(bond_orders[0] == CHFL_BOND_SINGLE);

    chfl_bond_order order;
    chfl_topology_bond_order(topology, 2, 3, &order);
    assert(order == CHFL_BOND_DOUBLE);

    chfl_free(topology);
    // [example]
    return 0;
}
