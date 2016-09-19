#include <cmath>
#include <catch.hpp>
#include "chemfiles.hpp"

using namespace chemfiles;

bool operator==(const Matrix3D& lhs, const Matrix3D& rhs) {
    return lhs[0][0] == rhs[0][0] && lhs[1][0] == rhs[1][0] && lhs[2][0] == rhs[2][0] &&
           lhs[0][1] == rhs[0][1] && lhs[1][1] == rhs[1][1] && lhs[2][1] == rhs[2][1] &&
           lhs[0][2] == rhs[0][2] && lhs[1][2] == rhs[1][2] && lhs[2][2] == rhs[2][2];

}


TEST_CASE("Vector3d", "[types]"){
    auto u = vector3d(1.0, 1.0, 1.0);
    auto v = vector3d(-21.0, 15.0, 23.5);

    CHECK((u + v) == vector3d(-20.0, 16.0, 24.5));
    CHECK((u - v) == vector3d(22.0, -14.0, -22.5));

    CHECK((3.0 * u) == vector3d(3.0, 3.0, 3.0));
    CHECK((u / 2.0) == vector3d(0.5, 0.5, 0.5));

    CHECK((v * 4.75) == (4.75 * v));
}

TEST_CASE("Geometry", "[types]"){
    auto v = vector3d(1.0, 1.0, 1.0);

    CHECK(norm(v) == sqrt(3.0));
    CHECK(norm2(v) == 3.0);

    auto u = vector3d(-1.0, 0.0, 1.0);
    CHECK(dot(u, v) == 0.0);

    auto w = vector3d(1.0, 0.0, 0.0);
    auto k = vector3d(std::cos(1.3), std::sin(1.3), 0.0);
    CHECK(fabs(dot(w, k) - cos(1.3)) < 1e-7);

    CHECK(dot(cross(w, k), k) == 0.0);
    CHECK(dot(cross(w, k), w) == 0.0);
}

TEST_CASE("Matrix3", "[types]"){
    SECTION("Constructors") {
        auto A = matrix3d();
        for (size_t i=0; i<3; i++) {
            for (size_t j=0; j<3; j++) {
                CHECK(A[i][j] == 0);
            }
        }

        A = matrix3d(1, 2, 3);
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
        auto A = Matrix3D{{
            {{2, 4, 9}},
            {{1, -67, 8}},
            {{9, 78.9, 65}},
        }};
        auto I = matrix3d(1, 1, 1);
        CHECK((A * I) == A);
        CHECK((I * A) == A);

        auto C = Matrix3D{{
            {{2, 4, 9}},
            {{1, -6, 8}},
            {{-3, 9, 5}},
        }};
        auto D = Matrix3D{{
            {{7, -1, 0}},
            {{2, 0, 4}},
            {{2, 8, -6}},
        }};

        auto E = Matrix3D{{
            {{40, 70, -38}},
            {{11, 63, -72}},
            {{7, 43, 6}},
        }};
        auto F = Matrix3D{{
            {{13, 34, 55}},
            {{-8, 44, 38}},
            {{30, -94, 52}}
        }};

        CHECK((C * D) == E);
        CHECK((D * C) == F);
    }

    SECTION("Matrix-Vector Multiplications") {
        auto A = Matrix3D{{
            {{2, 4, 9}},
            {{1, -6, 8}},
            {{-3, 9, 5}},
        }};
        auto I = matrix3d(1, 1, 1);
        auto v = vector3d(7, -9, 2);

        CHECK((I * v) == v);
        CHECK((A * v) == vector3d(-4, 77, -92));
    }

    SECTION("Inversion") {
        auto A = matrix3d(10, 12, 16);
        auto B = invert(A);
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

        auto I = matrix3d(1, 1, 1);
        CHECK((A * B) == I);
    }
}
