// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "catch.hpp"
#include "chemfiles.hpp"
#include "helpers.hpp"
using namespace chemfiles;

TEST_CASE("Read files in XTC format") {
    SECTION("Read trajectory") {
        auto file = Trajectory("data/xtc/ubiquitin.xtc");
        auto frame = file.read();

        CHECK(frame.get("md_step")->as_double() == 0);
        CHECK(approx_eq(frame.get("time")->as_double(), 0));
        CHECK(frame.get("xtc_precision")->as_double() == 1000);
        CHECK(frame.size() == 20455);

        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(24.8300, 24.6600, 18.8100), 1e-4));
        CHECK(approx_eq(positions[11], Vector3D(23.7700, 24.5600, 21.4700), 1e-4));

        auto cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(approx_eq(cell.a(), 55.6800, 1e-4));
        CHECK(approx_eq(cell.b(), 58.8700, 1e-4));
        CHECK(approx_eq(cell.c(), 62.5700, 1e-4));

        file.read(); // Skip a frame
        frame = file.read();

        CHECK(frame.get("md_step")->as_double() == 200);
        CHECK(approx_eq(frame.get("time")->as_double(), 0.6));
        CHECK(frame.get("xtc_precision")->as_double() == 1000);
        CHECK(frame.size() == 20455);

        positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(24.7100, 24.5700, 18.4500), 1e-4));
        CHECK(approx_eq(positions[11], Vector3D(23.6700, 24.4800, 21.5200), 1e-4));

        cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(approx_eq(cell.a(), 55.6800, 1e-4));
        CHECK(approx_eq(cell.b(), 58.8700, 1e-4));
        CHECK(approx_eq(cell.c(), 62.5700, 1e-4));

        frame = file.read_step(230); // skip forward

        CHECK(frame.get("md_step")->as_double() == 23000);
        CHECK(approx_eq(frame.get("time")->as_double(), 46.0));
        CHECK(frame.get("xtc_precision")->as_double() == 1000);
        CHECK(frame.size() == 20455);

        positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(24.6300, 24.6700, 18.5000), 1e-4));
        CHECK(approx_eq(positions[11], Vector3D(23.6800, 24.0700, 21.3100), 1e-4));

        cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(approx_eq(cell.a(), 55.6800, 1e-4));
        CHECK(approx_eq(cell.b(), 58.8700, 1e-4));
        CHECK(approx_eq(cell.c(), 62.5700, 1e-4));

        frame = file.read_step(50); // skip behind previous step

        CHECK(frame.get("md_step")->as_double() == 5000);
        CHECK(approx_eq(frame.get("time")->as_double(), 10.0));
        CHECK(frame.get("xtc_precision")->as_double() == 1000);
        CHECK(frame.size() == 20455);

        positions = frame.positions();
        CHECK(approx_eq(positions[0], Vector3D(24.5100, 24.5300, 18.7800), 1e-4));
        CHECK(approx_eq(positions[11], Vector3D(23.5300, 24.0900, 21.3100), 1e-4));

        cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(approx_eq(cell.a(), 55.6800, 1e-4));
        CHECK(approx_eq(cell.b(), 58.8700, 1e-4));
        CHECK(approx_eq(cell.c(), 62.5700, 1e-4));
    }
}
