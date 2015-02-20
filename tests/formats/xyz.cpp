#include "catch.hpp"

#include "Chemharp.hpp"
using namespace harp;

#define XYZDIR SRCDIR "/files/xyz/"

TEST_CASE("Read files in XYZ format", "[XYZ]"){
    auto file = Trajectory(XYZDIR"helium.xyz");
    Frame frame;

    SECTION("Stream style reading"){
        file >> frame;
        CHECK(frame.natoms() == 125);
        // Check positions
        auto positions = frame.positions();
        CHECK(positions[0] == Vector3D(0.49053f, 8.41351f, 0.0777257f));
        CHECK(positions[124] == Vector3D(8.57951f, 8.65712f, 8.06678f));
        // Check topology
        auto topology = frame.topology();
        CHECK(topology.atom_types().size() == 1);
        CHECK(topology.atom_list().size() == 125);
        CHECK(topology[0] == Atom("He"));
    }

    SECTION("Method style reading"){
        frame = file.read_next_step();
        CHECK(frame.natoms() == 125);
        // Check positions
        auto positions = frame.positions();
        CHECK(positions[0] == Vector3D(0.49053f, 8.41351f, 0.0777257f));
        CHECK(positions[124] == Vector3D(8.57951f, 8.65712f, 8.06678f));
        // Check topology
        auto topology = frame.topology();
        CHECK(topology.atom_types().size() == 1);
        CHECK(topology.atom_list().size() == 125);
        CHECK(topology[0] == Atom("He"));
    }
}
