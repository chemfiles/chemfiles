// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <catch.hpp>
#include "helpers.hpp"
#include "chemfiles.hpp"
using namespace chemfiles;

constexpr double PI = 3.141592653589793238463;

static bool roughly(double a, double b) {
    return fabs(a - b) < 1e-12;
}

TEST_CASE("Frame size") {
    auto frame = Frame();
    CHECK(frame.size() == 0);
    CHECK(frame.positions().size() == 0);
    // No velocity data yet
    CHECK_FALSE(frame.velocities());

    frame.resize(15);
    CHECK(frame.size() == 15);
    CHECK(frame.positions().size() == 15);
    CHECK_FALSE(frame.velocities());

    frame.add_velocities();
    REQUIRE(frame.velocities());
    CHECK(frame.velocities()->size() == 15);

    frame.resize(30);
    CHECK(frame.size() == 30);
    CHECK(frame.positions().size() == 30);
    REQUIRE(frame.velocities());
    CHECK(frame.velocities()->size() == 30);

    frame.resize(2);
    CHECK(frame.size() == 2);
    CHECK(frame.positions().size() == 2);
    REQUIRE(frame.velocities());
    CHECK(frame.velocities()->size() == 2);

    frame.add_atom(Atom("H"), Vector3D(1, 2, 3), Vector3D(4, 5, 6));
    CHECK(frame.size() == 3);
    CHECK(frame.positions().size() == 3);
    CHECK(frame.positions()[2] == Vector3D(1, 2, 3));
    REQUIRE(frame.velocities());
    CHECK(frame.velocities()->size() == 3);
    CHECK((*frame.velocities())[2] == Vector3D(4, 5, 6));

    frame.remove(0);
    CHECK(frame.size() == 2);
    CHECK(frame.positions().size() == 2);
    CHECK(frame.velocities()->size() == 2);

    CHECK_THROWS_AS(frame.remove(15), OutOfBounds);
}

TEST_CASE("Positions and velocities") {
    auto frame = Frame();
    frame.resize(15);

    frame.positions()[0] = Vector3D(1, 2, 3);
    CHECK(frame.positions()[0] == Vector3D(1, 2, 3));

    frame.add_velocities();
    (*frame.velocities())[0] = Vector3D(5, 6, 7);
    CHECK((*frame.velocities())[0] == Vector3D(5, 6, 7));

    {
        auto positions = frame.positions();
        auto velocities = frame.velocities();
        for (size_t i=0; i<15; i++) {
            positions[i] = Vector3D(4.0, 3.4, 1.0);
            (*velocities)[i] = Vector3D(4.0, 3.4, 1.0);
        }
    }

    auto positions = frame.positions();
    auto velocities = frame.velocities();
    for (size_t i=0; i<10; i++){
        CHECK(positions[i] == Vector3D(4.0, 3.4, 1.0));
        CHECK((*velocities)[i] == Vector3D(4.0, 3.4, 1.0));
    }
}

TEST_CASE("Frame step") {
    auto frame = Frame();
    CHECK(frame.step() == 0);
    frame.set_step(1000);
    CHECK(frame.step() == 1000);
}

TEST_CASE("Unit cell") {
    auto frame = Frame();
    CHECK(frame.cell().shape() == UnitCell::INFINITE);
    frame.set_cell(UnitCell(10));
    CHECK(frame.cell().shape() == UnitCell::ORTHORHOMBIC);
}

TEST_CASE("Frame errors") {
    auto frame = Frame();
    frame.resize(5);
    CHECK_THROWS_AS(frame.set_topology(Topology()), Error);
}

TEST_CASE("Guess topology") {
    SECTION("Simple case") {
        auto frame = Frame();
        frame.add_atom(Atom("H"), {0, 1, 0});
        frame.add_atom(Atom("O"), {0, 0, 0});
        frame.add_atom(Atom("O"), {1.5, 0, 0});
        frame.add_atom(Atom("H"), {1.5, 1, 0});
        frame.guess_topology();

        auto bonds = std::vector<Bond>{{0, 1}, {1, 2}, {2, 3}};
        auto angles = std::vector<Angle>{{0, 1, 2}, {1, 2, 3}};
        auto dihedrals = std::vector<Dihedral>{{0, 1, 2, 3}};
        CHECK(frame.topology().bonds() == bonds);
        CHECK(frame.topology().angles() == angles);
        CHECK(frame.topology().dihedrals() == dihedrals);
    }

    SECTION("Methane file") {
        auto frame = Trajectory("data/xyz/methane.xyz").read();
        frame.guess_topology();

        auto topology = frame.topology();
        CHECK(topology.bonds() == (std::vector<Bond>{{0, 1}, {0, 2}, {0, 3}, {0, 4}}));
        CHECK(topology.angles() == (std::vector<Angle>{
            {1, 0, 2}, {1, 0, 3}, {1, 0, 4}, {2, 0, 3}, {2, 0, 4}, {3, 0, 4},
        }));

        CHECK(topology.bonds().size() == 4);
        CHECK(topology.angles().size() == 6);
        CHECK(topology.dihedrals().size() == 0);

        frame.remove(1);
        topology = frame.topology();
        CHECK(topology.bonds().size() == 3);
        CHECK(topology.angles().size() == 3);
        CHECK(topology.bonds() == (std::vector<Bond>{{0, 1}, {0, 2}, {0, 3}}));
    }

    SECTION("Cleanup supplementaty H-H bonds") {
        auto frame = Frame();
        frame.add_atom(Atom("O"), {0, 0, 0});
        frame.add_atom(Atom("H"), {0.2, 0.8, 0});
        frame.add_atom(Atom("H"), {-0.2, 0.8, 0});

        frame.guess_topology();
        CHECK(frame.topology().bonds() == (std::vector<Bond>{{0, 1}, {0, 2}}));
    }

    // Weird geometries
    SECTION("Triangle molecule") {
        auto frame = Frame();
        frame.add_atom(Atom("C"), {0, 1, 0});
        frame.add_atom(Atom("C"), {0.5, 0, 0});
        frame.add_atom(Atom("C"), {-0.5, 0, 0});

        frame.guess_topology();
        CHECK(frame.topology().bonds() == (std::vector<Bond>{{0, 1}, {0, 2}, {1, 2}}));
        CHECK(frame.topology().angles() == (std::vector<Angle>{{0, 1, 2}, {0, 2, 1}, {1, 0, 2}}));
        CHECK(frame.topology().dihedrals() == (std::vector<Dihedral>{}));
    }

    // Weird geometries
    SECTION("Square molecule") {
        auto frame = Frame();
        frame.add_atom(Atom("C"), {0, 0, 0});
        frame.add_atom(Atom("C"), {1.5, 0, 0});
        frame.add_atom(Atom("C"), {1.5, 1.5, 0});
        frame.add_atom(Atom("C"), {0, 1.5, 0});

        frame.guess_topology();
        CHECK(frame.topology().bonds() == (std::vector<Bond>{{0, 1}, {0, 3}, {1, 2}, {2, 3}}));
        CHECK(frame.topology().angles() == (std::vector<Angle>{{0, 1, 2}, {0, 3, 2}, {1, 0, 3}, {1, 2, 3}}));
        CHECK(frame.topology().dihedrals() == (std::vector<Dihedral>{{0, 1, 2, 3}, {1, 0, 3, 2}, {1, 2, 3, 0}, {2, 1, 0, 3}}));
    }
}

TEST_CASE("PBC functions") {
    SECTION("Distance") {
        auto frame = Frame();
        frame.set_cell(UnitCell(3.0, 4.0, 5.0));
        frame.add_atom(Atom(), Vector3D(0, 0, 0));
        frame.add_atom(Atom(), Vector3D(1, 2, 6));

        CHECK(approx_eq(frame.distance(0, 1), sqrt(6.0)));
    }

    SECTION("Angles") {
        auto frame = Frame();
        frame.add_atom(Atom(), Vector3D(1, 0, 0));
        frame.add_atom(Atom(), Vector3D(0, 0, 0));
        frame.add_atom(Atom(), Vector3D(0, 1, 0));
        CHECK(roughly(frame.angle(0, 1, 2), PI / 2.0));

        frame.add_atom(Atom(), Vector3D(cos(1.877), sin(1.877), 0));
        CHECK(roughly(frame.angle(0, 1, 3), 1.877));
    }

    SECTION("Dihedrals") {
        auto frame = Frame();
        frame.add_atom(Atom(), Vector3D(0, 0, 0));
        frame.add_atom(Atom(), Vector3D(1, 0, 0));
        frame.add_atom(Atom(), Vector3D(1, 1, 0));
        frame.add_atom(Atom(), Vector3D(2, 1, 0));

        CHECK(roughly(frame.dihedral(0, 1, 2, 3), PI));

        frame.add_atom(Atom(), Vector3D(1.241, 0.444, 0.349));
        frame.add_atom(Atom(), Vector3D(-0.011, -0.441, 0.333));
        frame.add_atom(Atom(), Vector3D(-1.176, 0.296, -0.332));
        frame.add_atom(Atom(), Vector3D(-1.396, 1.211, 0.219));

        CHECK(roughly(frame.dihedral(4, 5, 6, 7), 1.045378962606));
    }

    SECTION("Out of plane") {
        auto frame = Frame();
        frame.add_atom(Atom(), Vector3D(0, 0, 0));
        frame.add_atom(Atom(), Vector3D(0, 0, 2));
        frame.add_atom(Atom(), Vector3D(1, 0, 0));
        frame.add_atom(Atom(), Vector3D(0, 1, 0));

        CHECK(frame.out_of_plane(0, 1, 2, 3) == 2);
    }
}

TEST_CASE("Property map") {
    auto frame = Frame();
    frame.set("foo", 35);
    frame.set("bar", false);

    CHECK(frame.get("foo")->as_double() == 35.0);
    CHECK(frame.get("bar")->as_bool() == false);

    frame.set("foo", "test");
    CHECK(frame.get("foo")->as_string() == "test");
    CHECK_FALSE(frame.get("not here"));
}
