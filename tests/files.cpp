#include <string>
#include <vector>
#include <fstream>
#include <cstdio>

#include "catch.hpp"

#include "Chemharp.hpp"
#include "files/File.hpp"
#include "files/NCFile.hpp"
using namespace harp;

#define FILESDIR SRCDIR "/files/"

TEST_CASE("Read a text file", "[Files]"){
    BasicFile file(FILESDIR "xyz/helium.xyz");
    REQUIRE(file.is_open());

    CHECK(file.nlines() == 128);

    std::string line = file.getline();
    CHECK(line == "125");

    auto lines = file.readlines(42);
    REQUIRE(lines.size() == 42);
    CHECK(lines[0] == "Helium as a Lennard-Jone fluid");
    CHECK(lines[1] == "He 0.49053 8.41351 0.0777257");

    // Geting line count after some operations
    CHECK(file.nlines() == 128);

    file.rewind();
    line = file.getline();
    CHECK(line == "125");

    // State is preserved by the nlines function
    file.nlines();
    line = file.getline();
    CHECK(line == "Helium as a Lennard-Jone fluid");

    // Check stream version
    file.rewind();
    file >> line;
    CHECK(line == "125");
}

TEST_CASE("Write a text file", "[Files]"){
    BasicFile file(SRCDIR"/tmp.dat", "w");
    REQUIRE(file.is_open());

    file << "Test";

    file.writeline("Test again");

    std::vector<std::string> lines;
    lines.push_back("Hello");
    lines.push_back("world");
    file.writelines(lines);

    file.close();

    std::ifstream verification(SRCDIR"/tmp.dat");
    REQUIRE(verification.is_open());

    std::string line;
    std::getline(verification, line);
    CHECK(line == "Test");

    std::getline(verification, line);
    CHECK(line == "Test again");

    std::getline(verification, line);
    CHECK(line == "Hello");
    std::getline(verification, line);
    CHECK(line == "world");

    verification.close();

    remove(SRCDIR"/tmp.dat");
}

TEST_CASE("Errors in text files", "[Files]"){
    // TODO
}

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
    CHECK(string(var->name()) == "coordinates");
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
