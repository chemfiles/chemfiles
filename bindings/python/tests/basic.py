#!/usr/bin/env python
# -* coding: utf-8 -*

from chemharp import *

traj = Trajectory("../../../tests/files/xyz/helium.xyz")
frame = traj.read_next_step()

assert(frame.has_velocities() == False)

pos = frame.positions()

assert(abs(pos[0, 0] - 0.49053) < 1e-5)
assert(abs(pos[0, 1] - 8.41351) < 1e-5)
assert(abs(pos[0, 2] - 0.0777257) < 1e-5)

assert(len(frame) == 125)

assert(frame.step == 0)
frame.step = 2
assert(frame.step == 2)
