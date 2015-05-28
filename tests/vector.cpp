#include <cmath>
#include "catch.hpp"
#include "Chemharp.hpp"

using namespace harp;

TEST_CASE("Vector3D", "[Vector3D]"){
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
