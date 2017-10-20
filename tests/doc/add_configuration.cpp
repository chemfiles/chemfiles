// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <chemfiles.hpp>

// [example]
    int main() {
        chemfiles::add_configuration("local-file.toml");

        // Reading a trajectory will now use data from some/local/file.toml

        return 0;
    }
// [example]
