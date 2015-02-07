#include <cmath>

#include "catch.hpp"

#include "Harp.hpp"
#include "UnitCell.hpp"

using namespace harp;

TEST_CASE("Use the UnitCell type", "[UnitCell]"){

    SECTION("Constructors"){
        UnitCell infinite{};
        CHECK(infinite.type() == UnitCell::INFINITE);
        CHECK(infinite.a() == 0);
        CHECK(infinite.b() == 0);
        CHECK(infinite.c() == 0);
        CHECK(infinite.alpha() == 90);
        CHECK(infinite.beta() == 90);
        CHECK(infinite.gamma() == 90);

        CHECK_FALSE(infinite.periodic_x());
        CHECK_FALSE(infinite.periodic_y());
        CHECK_FALSE(infinite.periodic_z());

        UnitCell ortho1(10);
        CHECK(ortho1.type() == UnitCell::ORTHOROMBIC);
        CHECK(ortho1.a() == 10);
        CHECK(ortho1.b() == 10);
        CHECK(ortho1.c() == 10);
        CHECK(ortho1.alpha() == 90);
        CHECK(ortho1.beta() == 90);
        CHECK(ortho1.gamma() == 90);

        UnitCell ortho2(10, 11, 12);
        CHECK(ortho2.type() == UnitCell::ORTHOROMBIC);
        CHECK(ortho2.a() == 10);
        CHECK(ortho2.b() == 11);
        CHECK(ortho2.c() == 12);
        CHECK(ortho2.alpha() == 90);
        CHECK(ortho2.beta() == 90);
        CHECK(ortho2.gamma() == 90);

        UnitCell triclinic(10, 11, 12, 90, 80, 120);
        CHECK(triclinic.type() == UnitCell::TRICLINIC);
        CHECK(triclinic.a() == 10);
        CHECK(triclinic.b() == 11);
        CHECK(triclinic.c() == 12);
        CHECK(triclinic.alpha() == 90);
        CHECK(triclinic.beta() == 80);
        CHECK(triclinic.gamma() == 120);

        UnitCell infinite2(UnitCell::INFINITE);
        CHECK(infinite2.type() == UnitCell::INFINITE);
        CHECK(infinite2.a() == 0);
        CHECK(infinite2.b() == 0);
        CHECK(infinite2.c() == 0);
        CHECK(infinite2.alpha() == 90);
        CHECK(infinite2.beta() == 90);
        CHECK(infinite2.gamma() == 90);

        UnitCell triclinic2(UnitCell::TRICLINIC, 10);
        CHECK(triclinic2.type() == UnitCell::TRICLINIC);
        CHECK(triclinic2.a() == 10);
        CHECK(triclinic2.b() == 10);
        CHECK(triclinic2.c() == 10);
        CHECK(triclinic2.alpha() == 90);
        CHECK(triclinic2.beta() == 90);
        CHECK(triclinic2.gamma() == 90);

        UnitCell triclinic3(UnitCell::TRICLINIC, 10, 11, 12);
        CHECK(triclinic3.type() == UnitCell::TRICLINIC);
        CHECK(triclinic3.a() == 10);
        CHECK(triclinic3.b() == 11);
        CHECK(triclinic3.c() == 12);
        CHECK(triclinic3.alpha() == 90);
        CHECK(triclinic3.beta() == 90);
        CHECK(triclinic3.gamma() == 90);
    }

    SECTION("Set the values"){
        UnitCell cell{};

        cell.type(UnitCell::ORTHOROMBIC);
        CHECK(cell.type() == UnitCell::ORTHOROMBIC);

        cell.a(10);
        CHECK(cell.a() == 10);
        cell.b(15);
        CHECK(cell.b() == 15);
        cell.c(20);
        CHECK(cell.c() == 20);

        cell.type(UnitCell::TRICLINIC);
        CHECK(cell.type() == UnitCell::TRICLINIC);

        cell.alpha(80);
        CHECK(cell.alpha() == 80);
        cell.beta(120);
        CHECK(cell.beta() == 120);
        cell.gamma(60);
        CHECK(cell.gamma() == 60);

        cell.periodic_x(true);
        CHECK(cell.periodic_x());
        cell.periodic_y(true);
        CHECK(cell.periodic_y());
        cell.periodic_z(true);
        CHECK(cell.periodic_z());

        cell.periodic_z(false);
        CHECK_FALSE(cell.full_periodic());
        cell.full_periodic(true);
        CHECK(cell.full_periodic());
    }

    SECTION("Matricial representation"){
        UnitCell triclinic(10, 11, 12, 90, 60, 120);
        auto mat1 = triclinic.matricial();
        double a, b, c;

        a = mat1[0][0];
        b = sqrt(mat1[1][0]*mat1[1][0] + mat1[1][1]*mat1[1][1]);
        c = sqrt(mat1[2][0]*mat1[2][0] + mat1[2][1]*mat1[2][1] + mat1[2][2]*mat1[2][2]);

        CHECK(a == triclinic.a());
        CHECK(b == triclinic.b());
        CHECK(c == triclinic.c());


        auto mat2 = new double[3][3];
        triclinic.raw_matricial(mat2);

        for (size_t i=0; i<3; i++) {
            for (size_t j=0; j<3; j++) {
                CHECK(mat1[i][j] == mat2[i][j]);
            }
        }
        delete[] mat2;
    }
}
