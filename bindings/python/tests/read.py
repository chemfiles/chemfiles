#!/usr/bin/env python
# -* coding: utf-8 -*

"""
Testing the reading of a trajectory, and the read access to all the types
"""
import add_chemharp_to_path
from chemharp import *
import os

XYZFILE = os.path.join(os.path.dirname(__file__), "..", "..", "..", "tests",
                       "data", "xyz", "helium.xyz")

FIRST_FRAME = {
    0: [0.49053, 8.41351, 0.0777257],
    124: [8.57951, 8.65712, 8.06678]
}

LAST_FRAME = {
    0: [-1.186037, 11.439334, 0.529939],
    124: [5.208778, 12.707273, 10.940157]
}

MIDLE_FRAME = {
    0: [-0.145821, 8.540648, 1.090281],
    124: [8.446093, 8.168162, 9.350953]
}


def check_frame(frame, reference):
    assert(not frame.has_velocities)
    assert(frame.natoms == 125)
    assert(len(frame) == 125)

    positions = frame.positions
    for atom, pos in reference.items():
        for i in range(3):
            assert(abs(positions[atom, i] - pos[i]) < 1e-5)


def check_topology(topology):
    assert(topology.natoms == 125)
    assert(len(topology) == 125)
    assert(topology[3].name == "He")


def check_cell(cell):
    assert(cell.type == CellType.INFINITE)
    assert(cell.a == 0.0)
    assert(cell.b == 0.0)
    assert(cell.c == 0.0)

    assert(cell.alpha == 90.0)
    assert(cell.beta == 90.0)
    assert(cell.gamma == 90.0)


if __name__ == "__main__":
    traj = Trajectory(XYZFILE)
    frame = traj.read_next_step()
    assert(frame.step == 0)

    check_frame(frame, FIRST_FRAME)
    check_topology(frame.topology)
    check_cell(frame.cell)

    while not traj.done():
        frame = traj.read_next_step()

    check_frame(frame, LAST_FRAME)

    frame = traj.read_at_step(42)
    check_frame(frame, MIDLE_FRAME)
