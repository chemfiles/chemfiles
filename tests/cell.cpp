// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <catch.hpp>
#include "helpers.hpp"
#include "chemfiles.hpp"
using namespace chemfiles;

TEST_CASE("Use the UnitCell type") {
    SECTION("Constructors") {
        SECTION("Infinite") {
            auto infinite = UnitCell();
            CHECK(infinite.shape() == UnitCell::INFINITE);
            CHECK(infinite.lengths() == Vector3D(0, 0, 0));
            CHECK(infinite.angles() == Vector3D(90, 90, 90));
            CHECK(infinite.volume() == 0);

            CHECK(UnitCell({0, 0, 0})== UnitCell());
            CHECK(UnitCell({0, 0, 0}, {90, 90, 90})== UnitCell());
            CHECK(UnitCell(Matrix3D::zero())== UnitCell());
        }

        SECTION("Orthorhombic") {
            auto cell = UnitCell({10, 11, 12});
            CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
            CHECK(cell.lengths() == Vector3D(10, 11, 12));
            CHECK(cell.angles() == Vector3D(90, 90, 90));
            CHECK(cell.volume() == 10 * 11 * 12);

            auto matrix = Matrix3D(10, 0, 0, 0, 11, 0, 0, 0, 12);
            cell = UnitCell(matrix);
            CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
            CHECK(cell.lengths() == Vector3D(10, 11, 12));
            CHECK(cell.angles() == Vector3D(90, 90, 90));
            CHECK(cell.volume() == 10 * 11 * 12);
        }

        SECTION("Triclinic") {
            auto cell = UnitCell({10, 11, 12}, {90, 80, 120});
            CHECK(cell.shape() == UnitCell::TRICLINIC);
            CHECK(approx_eq(cell.lengths(), {10, 11, 12}, 1e-12));
            CHECK(approx_eq(cell.angles(), {90, 80, 120}, 1e-12));
            CHECK(approx_eq(cell.volume(), 1119.9375925598192, 1e-12));

            auto matrix = cell.matrix();
            cell = UnitCell(matrix);
            CHECK(cell.shape() == UnitCell::TRICLINIC);
            CHECK(approx_eq(cell.lengths(), {10, 11, 12}, 1e-12));
            CHECK(approx_eq(cell.angles(), {90, 80, 120}, 1e-12));
            CHECK(approx_eq(cell.volume(), 1119.9375925598192, 1e-12));

            matrix = Matrix3D(
                26.2553,  0.0000, -4.4843,
                 0.0000, 11.3176,  0.0000,
                 0.0000,  0.0000,  11.011
            );

            cell = UnitCell(matrix);
            CHECK(cell.shape() == UnitCell::TRICLINIC);
            CHECK(approx_eq(cell.lengths(), {26.2553, 11.3176, 11.8892}, 1e-4));
            CHECK(approx_eq(cell.angles(), {90, 112.159, 90}, 1e-4));
        }
    }

    SECTION("Operators") {
        auto cell = UnitCell({10, 10, 10});
        CHECK(cell == UnitCell({10, 10, 10}));
        CHECK(cell != UnitCell({11, 10, 10}));

        CHECK(cell != UnitCell());
        CHECK(cell != UnitCell({10, 10, 10}, {120, 90, 90}));
    }

    SECTION("Set the values") {
        auto cell = UnitCell();

        cell.set_shape(UnitCell::ORTHORHOMBIC);
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(approx_eq(cell.matrix(), Matrix3D::zero()));

        cell.set_lengths({10, 15, 20});
        CHECK(cell.lengths() == Vector3D(10, 15, 20));

        cell.set_shape(UnitCell::TRICLINIC);
        CHECK(cell.shape() == UnitCell::TRICLINIC);

        cell.set_angles({80, 120, 60});
        CHECK(approx_eq(cell.angles(), {80, 120, 60}, 1e-12));
    }

    SECTION("Wraping vectors") {
        UnitCell infinite;
        UnitCell ortho({10, 11, 12});
        UnitCell triclinic_algo({10, 11, 12});
        triclinic_algo.set_shape(UnitCell::TRICLINIC);
        UnitCell triclinic({10, 11, 12}, {90, 90, 80});
        UnitCell tilted({10, 10, 10}, {140, 100, 100});
        auto v = Vector3D(22.0, -15.0, 5.8);

        CHECK(infinite.wrap(v) == v);
        CHECK(approx_eq(ortho.wrap(v), Vector3D(2.0, -4.0, 5.8), 1e-5));
        CHECK(approx_eq(ortho.wrap(v), triclinic_algo.wrap(v), 1e-5));
        CHECK(approx_eq(triclinic.wrap(v), Vector3D(3.91013, -4.16711, 5.8), 1e-5));
        CHECK(approx_eq(tilted.wrap(Vector3D(6, 8, -7)), Vector3D(4.26352, -0.08481, -1.37679), 1e-5));
    }

    SECTION("UnitCell errors") {
        auto cell = UnitCell();
        // Atempt to set values of an infinite unit cell
        CHECK_THROWS_WITH(cell.set_lengths({10, 10, 10}), "can not set lengths for an infinite cell");
        CHECK_THROWS_WITH(cell.set_angles({90, 90, 100}), "can not set angles for a non-triclinic cell");

        cell.set_shape(UnitCell::ORTHORHOMBIC);
        CHECK_THROWS_WITH(cell.set_angles({90, 90, 100}), "can not set angles for a non-triclinic cell");
    }

    SECTION("Setting shape errors") {
        auto cell = UnitCell({3, 4, 5}, {60, 70, 80});
        CHECK_THROWS_WITH(cell.set_shape(UnitCell::ORTHORHOMBIC), "can not set cell shape to ORTHORHOMBIC: some angles are not 90°");

        cell = UnitCell({3, 4, 5}, {60, 70, 80});
        CHECK_THROWS_WITH(cell.set_shape(UnitCell::INFINITE), "can not set cell shape to INFINITE: some angles are not 90°");
        cell = UnitCell({3, 4, 5});
        CHECK_THROWS_WITH(cell.set_shape(UnitCell::INFINITE), "can not set cell shape to INFINITE: some lengths are not 0");
    }
}
