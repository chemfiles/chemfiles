// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "catch.hpp"
#include "chemfiles.hpp"
#include "chemfiles/files/NcFile.hpp"
#include "helpers.hpp"
using namespace chemfiles;

TEST_CASE("Read a NetCDF file") {
    SECTION("Float variables") {
        NcFile file("data/netcdf/water.nc", File::READ);

        CHECK(file.global_attribute("Conventions") == "AMBER");
        // Usual dimmensions
        CHECK(file.dimension("atom") == 297);
        // Unlimited dimension
        CHECK(file.dimension("frame") == 100);

        CHECK(file.variable<nc::NcFloat>("cell_lengths").string_attribute("units") == "Angstrom");

        auto var = file.variable<nc::NcFloat>("coordinates");
        auto dims = var.dimmensions();
        CHECK(dims.size() == 3);
        CHECK(dims[0] == 100);
        CHECK(dims[1] == 297);
        CHECK(dims[2] == 3);

        auto EPS = 1e-5f;
        auto positions = var.get({0, 0, 0}, {1, 297, 3});
        CHECK(std::abs(positions[0] - 0.4172191f) < EPS);
        CHECK(std::abs(positions[1] - 8.303366f) < EPS);
        CHECK(std::abs(positions[2] - 11.73717f) < EPS);
    }

    SECTION("Double variables") {
        NcFile file("data/netcdf/water.ncrst", File::READ);

        CHECK(file.global_attribute("Conventions") == "AMBERRESTART");
        // Usual dimmensions
        CHECK(file.dimension("atom") == 297);

        CHECK(file.variable<nc::NcDouble>("cell_lengths").string_attribute("units") == "angstrom");

        auto var = file.variable<nc::NcDouble>("coordinates");
        auto dims = var.dimmensions();
        CHECK(dims.size() == 2);
        CHECK(dims[0] == 297);
        CHECK(dims[1] == 3);

        auto EPS = 1e-5;
        auto positions = var.get({0, 0}, {297, 3});
        CHECK(std::abs(positions[0] - 0.4172191) < EPS);
        CHECK(std::abs(positions[1] - 8.303366) < EPS);
        CHECK(std::abs(positions[2] - 11.73717) < EPS);
    }
}

TEST_CASE("Errors in NetCDF files") {
    SECTION("Float variables") {
        NcFile file("data/netcdf/water.nc", File::READ);

        CHECK_THROWS_WITH(file.global_attribute("FOO"),
                          "can not read attribute 'FOO': NetCDF: Attribute not found");
        CHECK_THROWS_WITH(file.dimension("FOO"), "missing dimmension 'FOO' in NetCDF file");
        CHECK_THROWS_WITH(
            file.variable<nc::NcFloat>("cell_lengths").string_attribute("Bar"),
            "can not read attribute id for attribute 'Bar': NetCDF: Attribute not found");
        CHECK_THROWS_WITH(file.variable<nc::NcFloat>("FOO"),
                          "can not get variable id for 'FOO: NetCDF: Variable not found");
    }

    SECTION("Double variables") {
        NcFile file("data/netcdf/water.ncrst", File::READ);

        CHECK_THROWS_WITH(file.global_attribute("FOO"),
                          "can not read attribute 'FOO': NetCDF: Attribute not found");
        CHECK_THROWS_WITH(file.dimension("FOO"), "missing dimmension 'FOO' in NetCDF file");
        CHECK_THROWS_WITH(
            file.variable<nc::NcDouble>("cell_lengths").string_attribute("Bar"),
            "can not read attribute id for attribute 'Bar': NetCDF: Attribute not found");
        CHECK_THROWS_WITH(file.variable<nc::NcDouble>("FOO"),
                          "can not get variable id for 'FOO: NetCDF: Variable not found");
    }
}

TEST_CASE("Write NetCDF files") {
    auto tmpfile = NamedTempPath(".nc");

    {
        NcFile file(tmpfile, File::WRITE);
        file.set_nc_mode(NcFile::DEFINE);
        file.add_global_attribute("global", "global.value");
        file.add_dimension("infinite");
        file.add_dimension("finite", 42);

        auto variable = file.add_variable<nc::NcFloat>("variable", "infinite", "finite");
        variable.add_string_attribute("attribute", "hello");

        auto variable_d = file.add_variable<nc::NcDouble>("variable_d", "infinite", "finite");
        variable_d.add_string_attribute("attribute", "world");

        file.set_nc_mode(NcFile::DATA);
        variable.add({0, 0}, {1, 42}, std::vector<float>(42, 38.2f));
        variable_d.add({0, 0}, {1, 42}, std::vector<double>(42, 37.4));
    }

    {
        NcFile file(tmpfile, File::APPEND);
        auto variable = file.variable<nc::NcFloat>("variable");
        variable.add({1, 0}, {1, 42}, std::vector<float>(42, 55.1f));
        auto variable_d = file.variable<nc::NcDouble>("variable_d");
        variable_d.add({1, 0}, {1, 42}, std::vector<double>(42, 66.3));
    }

    {
        NcFile file(tmpfile, File::READ);
        CHECK(file.global_attribute("global") == "global.value");
        CHECK(file.dimension("infinite") == 2);
        CHECK(file.dimension("finite") == 42);
        CHECK(file.variable_exists("variable"));
        CHECK(file.variable_exists("variable_d"));
        CHECK_FALSE(file.variable_exists("bar"));

        auto variable = file.variable<nc::NcFloat>("variable");
        CHECK(variable.string_attribute("attribute") == "hello");
        CHECK(variable.get({0, 0}, {1, 42}) == std::vector<float>(42, 38.2f));
        CHECK(variable.get({1, 0}, {1, 42}) == std::vector<float>(42, 55.1f));

        auto variable_d = file.variable<nc::NcDouble>("variable_d");
        CHECK(variable_d.string_attribute("attribute") == "world");
        CHECK(variable_d.get({0, 0}, {1, 42}) == std::vector<double>(42, 37.4));
        CHECK(variable_d.get({1, 0}, {1, 42}) == std::vector<double>(42, 66.3));
    }
}
