#include <iostream>

#include "catch.hpp"

#include "Chemharp.hpp"
#include "config.hpp"
#include "files/NCFile.hpp"
using namespace harp;

#define FILESDIR SRCDIR "/data/"

#if HAVE_NETCDF

TEST_CASE("Read a NetCDF file", "[Files]"){
    NCFile file(FILESDIR "netcdf/water.nc");
    REQUIRE(file.is_open());

    CHECK(file.global_attribute("Conventions") == "AMBER");
    // Usual dimmensions
    CHECK(file.dimmension("atom") == 297);
    // Unlimited dimension
    CHECK(file.dimmension("frame") == 100);

    CHECK(file.s_attribute("cell_lengths", "units") == "Angstrom");
    CHECK(file.f_attribute("cell_lengths", "scale_factor") == 1.0f);

    auto var = file.variable("coordinates");
    // var->name() has NcToke, i.e. const char* type.
    CHECK(std::string(var->name()) == "coordinates");
    CHECK(var->num_dims() == 3);
    auto edges = var->edges();
    CHECK(edges[0] == 100);
    CHECK(edges[1] == 297);
    CHECK(edges[2] == 3);
    delete[] edges; // We have to delete edges ourselves.

    auto positions = new float[297*3];
    float EPS = 1e-5f;
    CHECK(var->get(positions, 1, 297, 3));
    CHECK(fabs(positions[0] - 0.4172191f) < EPS);
    CHECK(fabs(positions[1] - 8.303366f) < EPS);
    CHECK(fabs(positions[2] - 11.73717f) < EPS);

    delete[] positions;
}

TEST_CASE("Errors in NetCDF files", "[Files]"){
    NCFile file(FILESDIR "netcdf/water.nc");
    // Silent error messages.
    auto error = NcError(NcError::silent_nonfatal);
    REQUIRE(file.is_open());
    CHECK_THROWS_AS(file.global_attribute("FOO"), FileError);
    CHECK_THROWS_AS(file.dimmension("FOO"), FileError);
    CHECK_THROWS_AS(file.s_attribute("cell_lengths", "Bar"), FileError);
    CHECK_THROWS_AS(file.f_attribute("cell_lengths", "Bar"), FileError);
    CHECK_THROWS_AS(file.variable("FOO"), FileError);
}

#endif // HAVE_NETCDF
