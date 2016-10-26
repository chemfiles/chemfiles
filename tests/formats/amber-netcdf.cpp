#include <cstdio>

#include "catch.hpp"
#include "chemfiles/config.hpp"
#if HAVE_NETCDF

#include "chemfiles.hpp"
using namespace chemfiles;

#include <boost/filesystem.hpp>
namespace fs=boost::filesystem;


bool roughly(const Vector3D& a, const Vector3D& b, const double eps){
    return (fabs(a[0] - b[0]) < eps)
        && (fabs(a[1] - b[1]) < eps)
        && (fabs(a[2] - b[2]) < eps);
}

TEST_CASE("Read files in NetCDF format", "[Amber NetCDF]") {
    SECTION("Read one frame") {
        Trajectory file("data/netcdf/water.nc");
        auto frame = file.read();
        CHECK(frame.natoms() == 297);
        // Check positions
        auto positions = frame.positions();
        CHECK(roughly(positions[0], vector3d(0.4172191, 8.303366, 11.73717), 1e-4));
        CHECK(roughly(positions[296], vector3d(6.664049, 11.61418, 12.96149), 1e-4));
    }

    SECTION("Read more than one frame") {
        Trajectory file("data/netcdf/water.nc");
        auto frame = file.read();
        frame = file.read();
        frame = file.read();
        CHECK(frame.natoms() == 297);

        auto positions = frame.positions();
        CHECK(roughly(positions[0], vector3d(0.2990952, 8.31003, 11.72146), 1e-4));
        CHECK(roughly(positions[296], vector3d(6.797599, 11.50882, 12.70423), 1e-4));

        while (!file.done()){
            frame = file.read();
        }
        positions = frame.positions();
        CHECK(roughly(positions[0], vector3d(0.3185586, 8.776042, 11.8927), 1e-4));
        CHECK(roughly(positions[296], vector3d(7.089802, 10.35007, 12.8159), 1e-4));
    }

    SECTION("Missing unit cell") {
        Trajectory file("data/netcdf/no-cell.nc");
        auto frame = file.read();
        CHECK(frame.natoms() == 1989);
        CHECK(frame.cell() == UnitCell());
    }
}


TEST_CASE("Write files in NetCDF format", "[Amber NetCDF]") {
    SECTION("Write the file") {
        Trajectory file("tmp.nc", 'w');
        Frame frame(4);
        auto positions = frame.positions();
        for(size_t i=0; i<4; i++) {
            positions[i] = vector3d(1, 2, 3);
        }

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
