#include <iostream>
#include <cstdio>

#include "catch.hpp"
#include "chemfiles/config.hpp"
#if CHEMFILES_NETCDF

#include "chemfiles.hpp"
#include "chemfiles/files/NcFile.hpp"
using namespace chemfiles;


TEST_CASE("Read a NetCDF file", "[Files]"){
    NcFile file("data/netcdf/water.nc", File::READ);

    CHECK(file.global_attribute("Conventions") == "AMBER");
    // Usual dimmensions
    CHECK(file.dimension("atom") == 297);
    // Unlimited dimension
    CHECK(file.dimension("frame") == 100);

    CHECK(file.variable<float>("cell_lengths").attribute("units") == "Angstrom");

    auto var = file.variable<float>("coordinates");
    auto dims = var.dimmensions();
    CHECK(dims.size() == 3);
    CHECK(dims[0] == 100);
    CHECK(dims[1] == 297);
    CHECK(dims[2] == 3);

    auto EPS = 1e-5;
    auto positions = var.get({0, 0, 0}, {1, 297, 3});
    CHECK(fabs(positions[0] - 0.4172191) < EPS);
    CHECK(fabs(positions[1] - 8.303366) < EPS);
    CHECK(fabs(positions[2] - 11.73717) < EPS);
}

TEST_CASE("Errors in NetCDF files", "[Files]"){
    NcFile file("data/netcdf/water.nc", File::READ);

    CHECK_THROWS_AS(file.global_attribute("FOO"), FileError);
    CHECK_THROWS_AS(file.dimension("FOO"), FileError);
    CHECK_THROWS_AS(file.variable<float>("cell_lengths").attribute("Bar"), FileError);
    CHECK_THROWS_AS(file.variable<float>("FOO"), FileError);
}

TEST_CASE("Write NetCDF files", "[Files]"){
    {
        NcFile file("tmp.nc", File::WRITE);
        file.set_nc_mode(NcFile::DEFINE);
        file.add_global_attribute("global", "global.value");
        file.add_dimension("infinite");
        file.add_dimension("finite", 42);
        auto variable = file.add_variable<float>("variable", "infinite", "finite");
        variable.add_attribute("attribute", "hello");
    }

    NcFile file("tmp.nc", File::READ);
    CHECK(file.global_attribute("global") == "global.value");
    CHECK(file.dimension("infinite") == 0);
    CHECK(file.dimension("finite") == 42);
    CHECK(file.variable_exists("variable"));
    CHECK_FALSE(file.variable_exists("bar"));
    CHECK(file.variable<float>("variable").attribute("attribute") == "hello");

    remove("tmp.nc");
}

#endif // CHEMFILES_NETCDF
