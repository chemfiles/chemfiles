// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.hpp>
using namespace chemfiles;

int main() {
    // [no-run]
    // [example]
    auto trajectory = Trajectory("water.nc");

    auto frame = trajectory.read();
    // Use the frame for awesome science here!

    // This is one way to iterate over all the frames in a trajectory
    while (!trajectory.done()) {
        frame = trajectory.read();
        // ...
    }
    // [example]
    return 0;
}
