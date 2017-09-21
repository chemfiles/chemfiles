// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.hpp>
using namespace chemfiles;

int main() {
    // [no-run]
    // [example]
    auto trajectory = Trajectory("water.nc");

    auto frame = trajectory.read_step(4);
    // Use the frame for awesome science here!

    // This is one way to iterate over all the frames in a trajectory
    for (size_t i = 0; i < trajectory.nsteps(); i++) {
        frame = trajectory.read_step(i);
        // ...
    }
    // [example]
    return 0;
}
