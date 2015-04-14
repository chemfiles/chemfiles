#include <iostream>

#include "catch.hpp"

#include "config.hpp"
#if HAVE_NETCDF

#include "Chemharp.hpp"
#include "files/NCFile.hpp"
using namespace harp;

#define FILESDIR SRCDIR "/data/"

TEST_CASE("Read a NetCDF file", "[Files]"){
    NCFile file(FILESDIR "netcdf/water.nc");
    REQUIRE(file.is_open());

    CHECK(file.global_attribute("Conventions") == "AMBER");
    // Usual dimmensions
    CHECK(file.dimmension("atom") == 297);
    // Unlimited dimension
    CHECK(file.dimmension("frame") == 100);

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
    NCFile file(FILESDIR "netcdf/water.nc");
    REQUIRE(file.is_open());

    CHECK_THROWS_AS(file.global_attribute("FOO"), FileError);
    CHECK_THROWS_AS(file.dimmension("FOO"), FileError);
    CHECK_THROWS_AS(file.attribute<float>("cell_lengths", "Bar"), FileError);
    CHECK_THROWS_AS(file.variable("FOO"), FileError);
}

TEST_CASE("Write NetCDF files", "[Files]"){
    NCFile file("tmp.nc", "w");
    REQUIRE(file.is_open());

    file.add_global_attribute("global", "global.value");
    file.add_dimmension("infinite");
    file.add_dimmension("finite", 42);
    file.add_variable<float>("variable", "infinite", "finite");
    file.add_attribute("variable", "variable.string", "hello");
    file.add_attribute("variable", "variable.float", 35.67f);

    file.close();

    NCFile check("tmp.nc");
    CHECK(check.global_attribute("global") == "global.value");
    CHECK(check.dimmension("infinite") == 0);
    CHECK(check.dimmension("finite") == 42);
    CHECK(check.attribute<string>("variable", "variable.string") == "hello");
    CHECK(check.attribute<float>("variable", "variable.float") == 35.67f);

    unlink("tmp.nc");
}

#endif // HAVE_NETCDF
