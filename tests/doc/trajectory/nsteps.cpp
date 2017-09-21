// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.hpp>
using namespace chemfiles;

int main() {
    // [no-run]
    // [example]
    auto trajectory = Trajectory("water.nc");

    auto nsteps = trajectory.nsteps();
    for (size_t i = 0; i < nsteps; i++) {
        auto frame = trajectory.read_step(i);
    }

    // [example]
    return 0;
}
