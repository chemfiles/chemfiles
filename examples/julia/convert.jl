# File convert.jl, example for the Chemharp library
# Any copyright is dedicated to the Public Domain.
# http://creativecommons.org/publicdomain/zero/1.0/

#!/usr/bin/env julia
using Chemharp

input_file = Trajectory("water.xyz")
water_topology = Topology()
# Orthorombic UnitCell with lengths of 20, 15 and 35 A
cell = UnitCell(20, 15, 35)

# Create Atoms
O = Atom("O")
H = Atom("H")

# Fill the topology with one water molecule
push!(water_topology, O)
push!(water_topology, H)
push!(water_topology, H)

add_bond!(water_topology, 0, 1)
add_bond!(water_topology, 0, 2)

output = Trajectory("water.pdb", "w")

for i=1:nsteps(input_file)
    frame = read(input_file)
    # Set the frame cell and topology
    set_cell!(frame, cell)
    set_topology!(frame, water_topology)
    # Write the frame to the output file, using PDB format
    write(output, frame)
end
