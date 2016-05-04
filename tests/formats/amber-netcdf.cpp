#include <cstdio>

#include "catch.hpp"
#include "chemfiles/config.hpp"
#if HAVE_NETCDF

#include "chemfiles.hpp"
using namespace chemfiles;

#include <boost/filesystem.hpp>
namespace fs=boost::filesystem;
const double EPS = 1e-5;

#define NCDIR SRCDIR "/data/netcdf/"

bool roughly(const Vector3D& a, const Vector3D& b, const double eps){
    return (fabs(a[0] - b[0]) < eps)
        && (fabs(a[1] - b[1]) < eps)
        && (fabs(a[2] - b[2]) < eps);
}

TEST_CASE("Read files in NetCDF format", "[Amber NetCDF]"){
    Trajectory file(NCDIR "water.nc");

    SECTION("Read one frame"){
        auto frame = file.read();
        CHECK(frame.natoms() == 297);
        // Check positions
        auto positions = frame.positions();
        CHECK(roughly(positions[0], vector3d(0.4172191f, 8.303366f, 11.73717f), 1e-4));
        CHECK(roughly(positions[296], vector3d(6.664049f, 11.61418f, 12.96149f), 1e-4));
    }

    SECTION("Read more than one frame"){
        auto frame = file.read();
        frame = file.read();
        frame = file.read();
        CHECK(frame.natoms() == 297);

        auto positions = frame.positions();
        CHECK(roughly(positions[0], vector3d(0.2990952f, 8.31003f, 11.72146f), 1e-4));
        CHECK(roughly(positions[296], vector3d(6.797599f, 11.50882f, 12.70423f), 1e-4));

        while (!file.done()){
            frame = file.read();
        }
        positions = frame.positions();
        CHECK(roughly(positions[0], vector3d(0.3185586f, 8.776042f, 11.8927f), 1e-4));
        CHECK(roughly(positions[296], vector3d(7.089802f, 10.35007f, 12.8159f), 1e-4));
    }
}


TEST_CASE("Write files in NetCDF format", "[Amber NetCDF]"){
    SECTION("Write the file") {
        Trajectory file("tmp.nc", 'w');
        Frame frame(4);
        auto positions = frame.positions();
        for(size_t i=0; i<4; i++)
            positions[i] = vector3d(1, 2, 3);

        frame.set_topology(dummy_topology(4));
        file.write(frame);
    }

    SECTION("Check the file") {
        Trajectory check("tmp.nc", 'r');
        auto frame = check.read();
        auto positions = frame.positions();
        CHECK(roughly(positions[0], vector3d(1, 2, 3), 1e-4));
        CHECK(roughly(positions[1], vector3d(1, 2, 3), 1e-4));
        CHECK(roughly(positions[2], vector3d(1, 2, 3), 1e-4));
        CHECK(roughly(positions[3], vector3d(1, 2, 3), 1e-4));
        remove("tmp.nc");
    }
}

#endif // HAVE_NETCDF
