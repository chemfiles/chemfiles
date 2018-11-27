// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <cmath>
#include <catch.hpp>
#include "helpers.hpp"
#include "chemfiles.hpp"
using namespace chemfiles;

TEST_CASE("Vector3d") {
    auto u = Vector3D(1.0, 1.0, 1.0);
    auto v = Vector3D(-21.0, 15.0, 23.5);

    CHECK((u + v) == Vector3D(-20.0, 16.0, 24.5));
    CHECK((u - v) == Vector3D(22.0, -14.0, -22.5));

    CHECK((3.0 * u) == Vector3D(3.0, 3.0, 3.0));
    CHECK((u / 2.0) == Vector3D(0.5, 0.5, 0.5));

    CHECK((v * 4.75) == (4.75 * v));

    CHECK((-v) == Vector3D(21.0, -15.0, -23.5));

    CHECK((v += u) == Vector3D(-20.0, 16.0, 24.5));
    CHECK(v == Vector3D(-20.0, 16.0, 24.5));

    CHECK((v -= u) ==  Vector3D(-21.0, 15.0, 23.5));
    CHECK(v ==  Vector3D(-21.0, 15.0, 23.5));

    CHECK((u *= 3) == Vector3D(3.0, 3.0, 3.0));
    CHECK(u == Vector3D(3.0, 3.0, 3.0));

    CHECK((u /= 3) == Vector3D(1.0, 1.0, 1.0));
    CHECK(u == Vector3D(1.0, 1.0, 1.0));
}

TEST_CASE("Geometry") {
    auto v = Vector3D(1.0, 1.0, 1.0);
    CHECK(v.norm() == sqrt(3.0));

    auto u = Vector3D(-1.0, 0.0, 1.0);
    CHECK(dot(u, v) == 0.0);

    auto w = Vector3D(1.0, 0.0, 0.0);
    auto k = Vector3D(std::cos(1.3), std::sin(1.3), 0.0);
    CHECK(fabs(dot(w, k) - cos(1.3)) < 1e-7);

    CHECK(dot(cross(w, k), k) == 0.0);
    CHECK(dot(cross(w, k), w) == 0.0);
}

TEST_CASE("Matrix3") {
    SECTION("Negate Matrix") {
        auto A = Matrix3D(
            2, 4, 9,
            1, -67, 8,
            9, 78.9, 65
        );

        auto B = Matrix3D(
            -2, -4, -9,
            -1, 67, -8,
            -9, -78.9, -65
        );

        CHECK(A == (-B));
        CHECK(B == (-A));
    }

    SECTION("Matrix-Matrix Addition") {
        auto A = Matrix3D(
            2, 4, 9,
            1, -67, 8,
            9, 78.9, 65
        );
        auto Z = Matrix3D::zero();
        CHECK((A + Z) == A);
        CHECK((Z + A) == A);
        CHECK((A - Z) == A);
        CHECK((Z - A) == (-A));

        auto C = Matrix3D(
            2, 4, 9,
            1, -6, 8,
            -3, 9, 5
        );

        auto D = Matrix3D(
            4, 8, 18,
            2, -73, 16,
            6, 87.9, 70
        );

        auto E = Matrix3D(
            0, 0, 0,
            0, -61, 0,
            12, 69.9, 60
        );

        CHECK((A + C) == D);
        CHECK((C + A) == D);
        CHECK((A - C) == E);
        CHECK((C - A) == (-E));

        CHECK((A += C) == D);
        CHECK(A == D);
        CHECK((A -= (C + C)) == E);
        CHECK(A == E);
    }

    SECTION("Matrix-Scalar Multiplication and Division") {
        auto A = Matrix3D(
            2, 4, 9,
            1, -67, 8,
            9, 78.9, 65
        );
        CHECK(A * 1 == A);
        CHECK(1 * A == A);
        CHECK(A / 1 == A);

        auto C = Matrix3D(
            4, 8, 18,
            2, -134, 16,
            18, 157.8, 130
        );

        auto D = Matrix3D(
            1, 2, 4.5,
            0.5, -33.5, 4,
            4.5, 39.45, 32.5
        );

        CHECK(A * 2 == C);
        CHECK(2 * A == C);
        CHECK(A / 2 == D);

        CHECK((A *= 2) == C);
        CHECK(A == C);
        CHECK((A /= 4) == D);
        CHECK(A == D);
    }

    SECTION("Matrix-Matrix Multiplications") {
        auto A = Matrix3D(
            2, 4, 9,
            1, -67, 8,
            9, 78.9, 65
        );
        auto I = Matrix3D::unit();
        CHECK((A * I) == A);
        CHECK((I * A) == A);

        auto C = Matrix3D(
            2, 4, 9,
            1, -6, 8,
            -3, 9, 5
        );
        auto D = Matrix3D(
            7, -1, 0,
            2, 0, 4,
            2, 8, -6
        );

        auto E = Matrix3D(
            40, 70, -38,
            11, 63, -72,
            7, 43, 6
        );
        auto F = Matrix3D(
            13, 34, 55,
            -8, 44, 38,
            30, -94, 52
        );

        CHECK((C * D) == E);
        CHECK((D * C) == F);
    }

    SECTION("Matrix-Vector Multiplications") {
        auto A = Matrix3D(
            2, 4, 9,
            1, -6, 8,
            -3, 9, 5
        );
        auto I = Matrix3D::unit();
        auto v = Vector3D(7, -9, 2);

        CHECK((I * v) == v);
        CHECK((A * v) == Vector3D(-4, 77, -92));
    }

    SECTION("Inversion") {
        auto A = Matrix3D(
            10,  2,  5,
            -1, 12,  8,
           0.2,  8, 16
        );

        CHECK(A.determinant() == 1263.2);

        auto B = A.invert();
        CHECK(approx_eq(B, Matrix3D(
             0.10132995566814439,  0.00633312222925902, -0.03483217226092463,
             0.01393286890436985,  0.12587080430652312, -0.06728942368587713,
            -0.00823305889803673, -0.063014566181127288, 0.09658011399620011
        ), 1e-9));

        auto I = Matrix3D::unit();
        CHECK(approx_eq((A * B), I, 1e-12));

        CHECK_THROWS_AS(Matrix3D::zero().invert(), Error);
    }

    SECTION("Transposition") {
        auto A = Matrix3D(
            3, 0, 5,
            1, 2, 6,
            2, 0, 1
        );
        auto transposed = Matrix3D(
            3, 1, 2,
            0, 2, 0,
            5, 6, 1
        );
        CHECK(A.transpose() == transposed);
    }
}
