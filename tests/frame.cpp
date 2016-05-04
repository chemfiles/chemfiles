#include <catch.hpp>
#include "chemfiles.hpp"
#include "chemfiles/Frame.hpp"
#include "chemfiles/UnitCell.hpp"
using namespace chemfiles;

TEST_CASE("Frame class usage", "[Frame]"){
    SECTION("Contructor"){
        auto frame = Frame(10);
        CHECK(frame.positions().size() == 10);
        CHECK(frame.cell().type() == UnitCell::INFINITE);
    }

    SECTION("Get and set"){
        auto frame = Frame();
        CHECK(frame.step() == 0);
        frame.set_step(1000);
        CHECK(frame.step() == 1000);

        frame.set_cell(UnitCell(10));
        CHECK(frame.cell().type() == UnitCell::ORTHOROMBIC);

        frame.resize(15);
        CHECK(frame.natoms() == 15);
        // No velocity data yet
        CHECK_FALSE(frame.velocities());

        frame.add_velocities();
        CHECK(frame.velocities());

        frame.positions()[0] = vector3d(1, 2, 3);
        CHECK(frame.positions()[0] == vector3d(1, 2, 3));

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

    SECTION("Guess bonds"){
        Trajectory file(SRCDIR "/data/xyz/methane.xyz");

        auto frame = file.read();
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
    }
}
