#ifndef WIN32

#include <iostream>
#include <cstdio>

#include "catch.hpp"
#include "chemfiles/config.hpp"
#if HAVE_NETCDF

#include "chemfiles.hpp"
#include "chemfiles/files/NCFile.hpp"
using namespace chemfiles;

#define FILESDIR SRCDIR "/data/"

TEST_CASE("Read a NetCDF file", "[Files]"){
    NCFile file(FILESDIR "netcdf/water.nc", "r");
    REQUIRE(file.is_open());

    CHECK(file.global_attribute("Conventions") == "AMBER");
    // Usual dimmensions
    CHECK(file.dimension("atom") == 297);
    // Unlimited dimension
    CHECK(file.dimension("frame") == 100);

    CHECK(file.attribute<string>("cell_lengths", "units") == "Angstrom");
    CHECK(file.attribute<float>("cell_lengths", "scale_factor") == 1.0f);

    auto var = file.variable("coordinates");
    CHECK(var.getDimCount() == 3);
    auto dims = var.getDims();
    CHECK(dims[0].getSize() == 100);
    CHECK(dims[1].getSize() == 297);
    CHECK(dims[2].getSize() == 3);

    auto positions = std::vector<float>(297*3);
    float EPS = 1e-5f;
    var.getVar(std::vector<size_t>{0, 0, 0},
               std::vector<size_t>{1, 297, 3},
               positions.data());
    CHECK(fabs(positions[0] - 0.4172191f) < EPS);
    CHECK(fabs(positions[1] - 8.303366f) < EPS);
    CHECK(fabs(positions[2] - 11.73717f) < EPS);
}

TEST_CASE("Errors in NetCDF files", "[Files]"){
    NCFile file(FILESDIR "netcdf/water.nc", "r");
    REQUIRE(file.is_open());

    CHECK_THROWS_AS(file.global_attribute("FOO"), FileError);
    CHECK_THROWS_AS(file.dimension("FOO"), FileError);
    CHECK_THROWS_AS(file.attribute<float>("cell_lengths", "Bar"), FileError);
    CHECK_THROWS_AS(file.variable("FOO"), FileError);
}

TEST_CASE("Write NetCDF files", "[Files]"){
    NCFile file("tmp.nc", "w");
    REQUIRE(file.is_open());

    file.add_global_attribute("global", "global.value");
    file.add_dimension("infinite");
    file.add_dimension("finite", 42);
    file.add_variable<float>("variable", "infinite", "finite");
    file.add_attribute("variable", "variable.string", "hello");
    file.add_attribute("variable", "variable.float", 35.67f);

    file.close();

    NCFile check("tmp.nc", "r");
    CHECK(check.global_attribute("global") == "global.value");
    CHECK(check.dimension("infinite") == 0);
    CHECK(check.dimension("finite") == 42);
    CHECK(check.attribute<string>("variable", "variable.string") == "hello");
    CHECK(check.attribute<float>("variable", "variable.float") == 35.67f);

    remove("tmp.nc");
}

#endif // HAVE_NETCDF
#endif // WIN32
