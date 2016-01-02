#include <cmath>
#include <catch.hpp>
#include "chemfiles.hpp"

using namespace chemfiles;

TEST_CASE("vector3d", "[vector3d]"){
    SECTION("Operators"){
        auto u = vector3d(1.0f, 1.0f, 1.0f);
        auto v = vector3d(-21.0f, 15.0f, 23.5f);

        CHECK((u + v) == vector3d(-20.0f, 16.0f, 24.5f));
        CHECK((u - v) == vector3d(22.0f, -14.0f, -22.5f));

        CHECK((3.0 * u) == vector3d(3.0f, 3.0f, 3.0f));
        CHECK((u / 2.0) == vector3d(0.5f, 0.5f, 0.5f));

        CHECK((v * 4.75) == (4.75 * v));
    }

    SECTION("Geometry"){
        auto v = vector3d(1.0f, 1.0f, 1.0f);

        CHECK(norm(v) == sqrt(3.0));
        CHECK(norm2(v) == 3.0);

        auto u = vector3d(-1.0f, 0.0f, 1.0f);
        CHECK(dot(u, v) == 0.0);

        auto w = vector3d(1.0f, 0.0f, 0.0f);
        auto k = vector3d(std::cos(1.3f), std::sin(1.3f), 0.0f);
        CHECK(fabs(dot(w, k) - cos(1.3)) < 1e-7);

        CHECK(dot(cross(w, k), k) == 0.0);
        CHECK(dot(cross(w, k), w) == 0.0);
    }
}
