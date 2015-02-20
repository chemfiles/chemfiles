#include "catch.hpp"

#include "Chemharp.hpp"
#include "Frame.hpp"
#include "UnitCell.hpp"

using namespace harp;

TEST_CASE("Frame class usage", "[Frame]"){
    auto frame = Frame(10);
    Logger::set_level(Logger::DEBUG);

    SECTION("Contructor"){
        CHECK(frame.positions().capacity() == 10);
        CHECK(frame.topology().atom_list().capacity() == 10);
        CHECK(frame.cell().type() == UnitCell::INFINITE);
    }

    SECTION("Get and set"){

        CHECK(frame.step() == 0);
        frame.step(1000);
        CHECK(frame.step() == 1000);

        frame.cell(UnitCell(10));
        CHECK(frame.cell().type() == UnitCell::ORTHOROMBIC);

        CHECK(frame.topology().atom_types().size() == 0);
        frame.topology().add_atom(Atom("H"));
        CHECK(frame.topology().atom_types().size() == 1);

        frame.reserve(15);
        CHECK(frame.natoms() == 15);
        // No velocity data yet
        CHECK_FALSE(frame.has_velocities());

        frame.reserve(15, true);
        CHECK(frame.has_velocities());

        frame.positions()[0] = Vector3D(1, 2, 3);
        CHECK(frame.positions()[0] == Vector3D(1, 2, 3));
        frame.velocities()[0] = Vector3D(5, 6, 7);
        CHECK(frame.velocities()[0] == Vector3D(5, 6, 7));

        auto mat = new float[15][3];
        frame.raw_positions(mat, 15);
        CHECK(mat[0][0] == 1);
        CHECK(mat[0][1] == 2);
        CHECK(mat[0][2] == 3);

        frame.raw_velocities(mat, 15);
        CHECK(mat[0][0] == 5);
        CHECK(mat[0][1] == 6);
        CHECK(mat[0][2] == 7);

        delete[] mat;
    }

    SECTION("Errors"){
        auto mat = new float[3][3];

        CHECK_THROWS_AS(frame.raw_positions(mat, 3), MemoryError);

        // This is not throwing, but only filling the array with zeroes
        frame.raw_velocities(mat, 3);
        for (size_t i=0; i<3; i++)
            for (size_t j=0; j<3; j++)
                CHECK(mat[i][j] == 0);

        frame.reserve(10, true);
        CHECK_THROWS_AS(frame.raw_velocities(mat, 3), MemoryError);
        delete[] mat;
    }
}
