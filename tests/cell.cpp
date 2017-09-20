// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <catch.hpp>
#include "helpers.hpp"
#include "chemfiles.hpp"
using namespace chemfiles;

TEST_CASE("Use the UnitCell type") {
    SECTION("Constructors") {
        UnitCell infinite{};
        CHECK(infinite.shape() == UnitCell::INFINITE);
        CHECK(infinite.a() == 0);
        CHECK(infinite.b() == 0);
        CHECK(infinite.c() == 0);
        CHECK(infinite.alpha() == 90);
        CHECK(infinite.beta() == 90);
        CHECK(infinite.gamma() == 90);
        CHECK(infinite.volume() == 0);

        UnitCell ortho1(10);
        CHECK(ortho1.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(ortho1.a() == 10);
        CHECK(ortho1.b() == 10);
        CHECK(ortho1.c() == 10);
        CHECK(ortho1.alpha() == 90);
        CHECK(ortho1.beta() == 90);
        CHECK(ortho1.gamma() == 90);

        UnitCell ortho2(10, 11, 12);
        CHECK(ortho2.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(ortho2.a() == 10);
        CHECK(ortho2.b() == 11);
        CHECK(ortho2.c() == 12);
        CHECK(ortho2.alpha() == 90);
        CHECK(ortho2.beta() == 90);
        CHECK(ortho2.gamma() == 90);
        CHECK(ortho2.volume() == 10*11*12);

        UnitCell triclinic(10, 11, 12, 90, 80, 120);
        CHECK(triclinic.shape() == UnitCell::TRICLINIC);
        CHECK(triclinic.a() == 10);
        CHECK(triclinic.b() == 11);
        CHECK(triclinic.c() == 12);
        CHECK(triclinic.alpha() == 90);
        CHECK(triclinic.beta() == 80);
        CHECK(triclinic.gamma() == 120);
        CHECK(triclinic.volume() == 1119.9375925598192);

        UnitCell infinite2(UnitCell::INFINITE);
        CHECK(infinite2.shape() == UnitCell::INFINITE);
        CHECK(infinite2.a() == 0);
        CHECK(infinite2.b() == 0);
        CHECK(infinite2.c() == 0);
        CHECK(infinite2.alpha() == 90);
        CHECK(infinite2.beta() == 90);
        CHECK(infinite2.gamma() == 90);

        UnitCell triclinic2(UnitCell::TRICLINIC, 10);
        CHECK(triclinic2.shape() == UnitCell::TRICLINIC);
        CHECK(triclinic2.a() == 10);
        CHECK(triclinic2.b() == 10);
        CHECK(triclinic2.c() == 10);
        CHECK(triclinic2.alpha() == 90);
        CHECK(triclinic2.beta() == 90);
        CHECK(triclinic2.gamma() == 90);

        UnitCell triclinic3(UnitCell::TRICLINIC, 10, 11, 12);
        CHECK(triclinic3.shape() == UnitCell::TRICLINIC);
        CHECK(triclinic3.a() == 10);
        CHECK(triclinic3.b() == 11);
        CHECK(triclinic3.c() == 12);
        CHECK(triclinic3.alpha() == 90);
        CHECK(triclinic3.beta() == 90);
        CHECK(triclinic3.gamma() == 90);
    }

    SECTION("Operators") {
        auto cell = UnitCell(10);
        CHECK(cell == UnitCell(10, 10, 10));
        CHECK(cell != UnitCell(11, 10, 10));
        CHECK(cell != UnitCell(UnitCell::TRICLINIC, 10, 10, 10));
    }

    SECTION("Set the values") {
        UnitCell cell;

        cell.shape(UnitCell::ORTHORHOMBIC);
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);

        cell.set_a(10);
        CHECK(cell.a() == 10);
        cell.set_b(15);
        CHECK(cell.b() == 15);
        cell.set_c(20);
        CHECK(cell.c() == 20);

        cell.shape(UnitCell::TRICLINIC);
        CHECK(cell.shape() == UnitCell::TRICLINIC);

        cell.set_alpha(80);
        CHECK(cell.alpha() == 80);
        cell.set_beta(120);
        CHECK(cell.beta() == 120);
        cell.set_gamma(60);
        CHECK(cell.gamma() == 60);
    }

    SECTION("Matricial representation") {
        UnitCell triclinic(10, 11, 12, 90, 60, 120);
        auto H = triclinic.matricial();
        double a = 0, b = 0, c = 0;

        a = H[0][0];
        b = sqrt(H[0][1] * H[0][1] + H[1][1] * H[1][1]);
        c = sqrt(H[0][2] * H[0][2] + H[1][2] * H[1][2] + H[2][2] * H[2][2]);

        CHECK(a == triclinic.a());
        CHECK(b == triclinic.b());
        CHECK(c == triclinic.c());

        double c_matrix[3][3];
        triclinic.raw_matricial(c_matrix);

        for (size_t i=0; i<3; i++) {
            for (size_t j=0; j<3; j++) {
                CHECK(H[i][j] == c_matrix[i][j]);
            }
        }
    }

    SECTION("Wraping vectors") {
        UnitCell infinite;
        UnitCell ortho(10, 11, 12);
        UnitCell triclinic_algo(UnitCell::TRICLINIC, 10, 11, 12);
        UnitCell triclinic(10, 11, 12, 90, 90, 80);
        UnitCell tilted(10, 10, 10, 140, 100, 100);
        auto v = vector3d(22.0, -15.0, 5.8);

        CHECK(infinite.wrap(v) == v);
        CHECK(approx_eq(ortho.wrap(v), vector3d(2.0, -4.0, 5.8), 1e-5));
        CHECK(approx_eq(ortho.wrap(v), triclinic_algo.wrap(v), 1e-5));
        CHECK(approx_eq(triclinic.wrap(v), vector3d(3.91013, -4.16711, 5.8), 1e-5));
        CHECK(approx_eq(tilted.wrap(vector3d(6, 8, -7)), vector3d(4.26352, -0.08481, -1.37679), 1e-5));
    }

    SECTION("UnitCell errors") {
        UnitCell cell;

        // Atempt to set values of an infinite unit cell
        CHECK_THROWS_AS(cell.set_a(10), Error);
        CHECK_THROWS_AS(cell.set_b(10), Error);
        CHECK_THROWS_AS(cell.set_c(10), Error);

        CHECK_THROWS_AS(cell.set_alpha(100), Error);
        CHECK_THROWS_AS(cell.set_beta(100), Error);
        CHECK_THROWS_AS(cell.set_gamma(100), Error);

        cell.shape(UnitCell::ORTHORHOMBIC);
        CHECK_THROWS_AS(cell.set_alpha(100), Error);
        CHECK_THROWS_AS(cell.set_beta(100), Error);
        CHECK_THROWS_AS(cell.set_gamma(100), Error);

        cell.shape(UnitCell::TRICLINIC);
        cell.set_alpha(80);
        cell.set_beta(120);
        cell.set_gamma(60);

        CHECK_THROWS_AS(cell.shape(UnitCell::ORTHORHOMBIC), Error);
    }
}
