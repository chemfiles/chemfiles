#!/usr/bin/env python
# -* coding: utf-8 -*

from chemharp import *
import os

FILENAME = "test-python.xyz"

CONTENT = """4
Written by Chemharp
He 0 0 0
He 0 0 0
He 0 0 0
He 0 0 0
"""

if __name__ == "__main__":
    traj = Trajectory(FILENAME, "w")

    top = Topology()
    for _ in range(4):
        top.append(Atom("He"))

    frame = Frame(4)
    # TODO set positions
    frame.topology = top
    traj.write(frame)
    traj.close()

    with open(FILENAME) as f:
        assert(f.read() == CONTENT)

    os.remove(FILENAME)
