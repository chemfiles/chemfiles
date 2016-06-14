#include <catch.hpp>
#include "chemfiles.hpp"
using namespace chemfiles;

TEST_CASE("Frame class usage", "[Frame]"){
    SECTION("Size"){
        auto frame = Frame(10);
        CHECK(frame.natoms() == 10);
        CHECK(frame.positions().size() == 10);
        // No velocity data yet
        CHECK_FALSE(frame.velocities());

        frame.resize(15);
        CHECK(frame.natoms() == 15);
        CHECK(frame.positions().size() == 15);
        CHECK_FALSE(frame.velocities());

        frame.add_velocities();
        REQUIRE(frame.velocities());
        CHECK(frame.velocities()->size() == 15);

        frame.resize(30);
        CHECK(frame.natoms() == 30);
        CHECK(frame.positions().size() == 30);
        REQUIRE(frame.velocities());
        CHECK(frame.velocities()->size() == 30);

        frame.resize(2);
        CHECK(frame.natoms() == 2);
        CHECK(frame.positions().size() == 2);
        REQUIRE(frame.velocities());
        CHECK(frame.velocities()->size() == 2);
    }

    SECTION("Positions and velocities"){
        auto frame = Frame(15);

        frame.positions()[0] = vector3d(1, 2, 3);
        CHECK(frame.positions()[0] == vector3d(1, 2, 3));

        frame.add_velocities();
        (*frame.velocities())[0] = vector3d(5, 6, 7);
        CHECK((*frame.velocities())[0] == vector3d(5, 6, 7));

        {
            auto positions = frame.positions();
            auto velocities = frame.velocities();
            for (size_t i=0; i<15; i++) {
                positions[i] = vector3d(4.f, 3.4f, 1.f);
                (*velocities)[i] = vector3d(4.f, 3.4f, 1.f);
            }
        }

        auto positions = frame.positions();
        auto velocities = frame.velocities();
        for (size_t i=0; i<10; i++){
            CHECK(positions[i] == vector3d(4.f, 3.4f, 1.f));
            CHECK((*velocities)[i] == vector3d(4.f, 3.4f, 1.f));
        }
    }

    SECTION("Step"){
        auto frame = Frame();
        CHECK(frame.step() == 0);
        frame.set_step(1000);
        CHECK(frame.step() == 1000);
    }

    SECTION("Unit Cell"){
        auto frame = Frame();
        CHECK(frame.cell().type() == UnitCell::INFINITE);
        frame.set_cell(UnitCell(10));
        CHECK(frame.cell().type() == UnitCell::ORTHORHOMBIC);
    }

    SECTION("Topology"){
        auto frame = Trajectory(SRCDIR "/data/xyz/methane.xyz").read();
        frame.guess_topology();

        auto topology = frame.topology();
        for (size_t i=1; i<5; i++){
            CHECK(topology.isbond(0, i));
        }

        CHECK_FALSE(topology.isbond(2, 4));
        CHECK_FALSE(topology.isbond(1, 2));

        CHECK(topology.isangle(1, 0, 2));
        CHECK(topology.isangle(3, 0, 2));
        CHECK(topology.isangle(2, 0, 4));

        CHECK(topology.bonds().size() == 4);

        // Wrong topology size
        frame = Frame(5);
        CHECK_THROWS_AS(frame.set_topology(Topology()), APIError);
    }
}
