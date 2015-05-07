# File convert.py, example for the Chemharp library
# Any copyright is dedicated to the Public Domain.
# http://creativecommons.org/publicdomain/zero/1.0/

#!/usr/bin/env python
# -*- coding: utf-8 -*-

from chemharp import Trajectory, UnitCell, Atom, Topology

input_file = Trajectory("water.xyz")
water_topology = Topology()
# Orthorombic UnitCell with lengths of 20, 15 and 35 A
cell = UnitCell(20, 15, 35)

# Create Atoms
O = Atom("O")
H = Atom("H")

# Fill the topology with one water molecule
water_topology.append(O)
water_topology.append(H)
water_topology.append(H)
water_topology.add_bond(0, 1)
water_topology.add_bond(0, 2)

output = Trajectory("water.pdb", "w")

while not input_file.done():
    frame = input_file.read()
    # Set the frame cell and topology
    frame.cell = cell
    frame.topology = water_topology
    # Write the frame to the output file, using PDB format
    output.write(frame)
