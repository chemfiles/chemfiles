#include <cmath>
#include "catch.hpp"
#include "Chemharp.hpp"

using namespace harp;

TEST_CASE("Vector3D", "[Vector3D]"){

    SECTION("Operators"){
        Vector3D u(1.0f, 1.0f, 1.0f);
        Vector3D v(-21.0f, 15.0f, 23.5f);

        CHECK((u + v) == Vector3D(-20.0f, 16.0f, 24.5f));
        CHECK((u - v) == Vector3D(22.0f, -14.0f, -22.5f));

        CHECK((3.0 * u) == Vector3D(3.0f, 3.0f, 3.0f));
        CHECK((u / 2.0) == Vector3D(0.5f, 0.5f, 0.5f));

        CHECK((v * 4.75) == (4.75 * v));
    }

    SECTION("Geometry"){
        Vector3D v(1.0f, 1.0f, 1.0f);

        CHECK(norm(v) == sqrt(3.0));
        CHECK(norm2(v) == 3.0);

        Vector3D u(-1.0f, 0.0f, 1.0f);

        CHECK(dot(u, v) == 0.0);

        Vector3D w(1.0f, 0.0f, 0.0f);
        Vector3D k(std::cos(1.3f), std::sin(1.3f), 0.0f);
        CHECK(fabs(dot(w, k) - cos(1.3)) < 1e-7);
    }
}
