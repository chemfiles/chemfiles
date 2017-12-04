// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <cmath>
#include <catch.hpp>
#include "helpers.hpp"
#include "chemfiles.hpp"
using namespace chemfiles;

TEST_CASE("Vector3d"){
    auto u = Vector3D(1.0, 1.0, 1.0);
    auto v = Vector3D(-21.0, 15.0, 23.5);

    CHECK((u + v) == Vector3D(-20.0, 16.0, 24.5));
    CHECK((u - v) == Vector3D(22.0, -14.0, -22.5));

    CHECK((3.0 * u) == Vector3D(3.0, 3.0, 3.0));
    CHECK((u / 2.0) == Vector3D(0.5, 0.5, 0.5));

    CHECK((v * 4.75) == (4.75 * v));
}

TEST_CASE("Geometry"){
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

TEST_CASE("Matrix3"){
    SECTION("Constructors") {
        auto A = Matrix3D();
        for (size_t i=0; i<3; i++) {
            for (size_t j=0; j<3; j++) {
                CHECK(A[i][j] == 0);
            }
        }

        A = Matrix3D(1, 2, 3);
        for (size_t i=0; i<3; i++) {
            for (size_t j=0; j<3; j++) {
                if (i != j) {
                    CHECK(A[i][j] == 0);
                } else {
                    CHECK(A[i][j] == i + 1);
                }
            }
        }
    }

    SECTION("Matrix-Matrix Multiplications") {
        auto A = Matrix3D(
            2, 4, 9,
            1, -67, 8,
            9, 78.9, 65
        );
        auto I = Matrix3D(1, 1, 1);
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
        auto I = Matrix3D(1, 1, 1);
        auto v = Vector3D(7, -9, 2);

        CHECK((I * v) == v);
        CHECK((A * v) == Vector3D(-4, 77, -92));
    }

    SECTION("Inversion") {
        auto A = Matrix3D(10, 12, 16);
        auto B = A.invert();
        for (size_t i=0; i<3; i++) {
            for (size_t j=0; j<3; j++) {
                if (i != j) {
                    CHECK(A[i][j] == 0);
                }
            }
        }
        CHECK(B[0][0] == 1.0/10);
        CHECK(B[1][1] == 1.0/12);
        CHECK(B[2][2] == 1.0/16);

        auto I = Matrix3D(1, 1, 1);
        CHECK(approx_eq((A * B), I, 1e-12));

        CHECK_THROWS_AS(Matrix3D().invert(), Error);
    }
}
