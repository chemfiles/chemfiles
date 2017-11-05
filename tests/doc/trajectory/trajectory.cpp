// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

#undef assert
#define assert CHECK

TEST_CASE() {
    // [no-run]
    // [example]
    // Simplest case: open a file for reading it, the format is guessed from
    // the extension.
    auto water = Trajectory("water.nc");

    // Open a file in a specific mode
    auto copper = Trajectory("copper.xyz", 'w');

    // Specify the file format to use
    auto nanotube = Trajectory("nanotube.lmp", 'r', "LAMMPS Data");
    // [example]
}
