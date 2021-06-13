#include <iostream>
#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

#undef assert
#define assert CHECK

TEST_CASE() {
    // [example]
    auto format = chemfiles::guess_format("filename.nc");
    assert(format == "Amber NetCDF");

    format = chemfiles::guess_format("filename.xyz.gz");
    assert(format == "XYZ / GZ");
    // [example]
}
