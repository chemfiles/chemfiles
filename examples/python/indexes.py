# File indexes.py, example for the Chemharp library
# Any copyright is dedicated to the Public Domain.
# http://creativecommons.org/publicdomain/zero/1.0/

#!/usr/bin/env python
# -*- coding: utf-8 -*-

from chemharp import Trajectory

traj = Trajectory("filename.xyz")
frame = traj.read_next_step()
positions = frame.positions()

indexes = []

for i in range(len(frame)):
    # positions is a numpy ndarray
    if positions[i, 0] < 5:
        indexes.append(i);

print("Atoms with x < 5: ")
for i in indexes:
    print("  - {}".format(i))
