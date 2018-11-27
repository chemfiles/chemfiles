#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

TEST_CASE() {
    // [example]
    chemfiles::add_configuration("local-file.toml");

    // Reading a trajectory will now use data from local-file.toml

    // [example]
}
