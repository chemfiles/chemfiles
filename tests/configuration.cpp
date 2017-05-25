// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <catch.hpp>
#include "chemfiles/Configuration.hpp"
#include "chemfiles/generic.hpp"
using namespace chemfiles;

TEST_CASE("Configuration") {
    CHECK(Configuration::rename("foo") == "foo");
    CHECK(Configuration::rename("Oh") == "O");
    CHECK(Configuration::rename("Oz") == "O");
    CHECK(Configuration::rename("N2") == "N4");

    chemfiles::add_configuration("another_config");
    CHECK(Configuration::rename("Oz") == "F");
}
