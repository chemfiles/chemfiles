// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

TEST_CASE() {
    // [no-run]
    // [example]
    // Text based formats support writing to memory
    auto trajectory_memory = Trajectory::memory_writer("SMI");

    // Binary formats typically do not support this feature
    CHECK_THROWS_WITH(
        Trajectory::memory_writer("XTC"),
        "opening memory for XDR files (such as TRR and XTC) is not supported"
    );

    // [example]
}
