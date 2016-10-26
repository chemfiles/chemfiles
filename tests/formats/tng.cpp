#include "catch.hpp"
#include "chemfiles.hpp"
using namespace chemfiles;


bool roughly(const Vector3D& a, const Vector3D& b, const double eps){
    return (fabs(a[0] - b[0]) < eps)
        && (fabs(a[1] - b[1]) < eps)
        && (fabs(a[2] - b[2]) < eps);
}

TEST_CASE("Read files in TNG format", "[TNG]"){
    SECTION("Read trajectory") {
        auto file = Trajectory("data/tng/example.tng");
        auto frame = file.read();

        CHECK(frame.natoms() == 15);
        auto positions = frame.positions();
        CHECK(roughly(positions[0], vector3d(1.0f, 1.0f, 1.0f), 1e-6));
        CHECK(roughly(positions[11], vector3d(8.5f, 33.0f, 34.0f), 1e-6));

        auto cell = frame.cell();
        CHECK(cell.shape() == UnitCell::INFINITE);

        file.read(); // Skip a frame
        frame = file.read();

        CHECK(frame.natoms() == 15);
        positions = frame.positions();
        CHECK(roughly(positions[0], vector3d(1.01562f, 1.02344f, 1.03125f), 1e-4));
        CHECK(roughly(positions[11], vector3d(8.5f, 33.0f, 34.0f), 1e-6));
    }

    SECTION("Read cell") {
        auto file = Trajectory("data/tng/water.tng");
        auto frame = file.read();

        CHECK(frame.natoms() == 29700);

        auto cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(cell.a() == 15.0);
        CHECK(cell.b() == 15.0);
        CHECK(cell.c() == 15.0);
    }

    SECTION("Read topology") {
        auto file = Trajectory("data/tng/example.tng");
        auto topology = file.read().topology();

        CHECK(topology.natoms() == 15);
        CHECK(topology[0].label() == "O");
        CHECK(topology[0].element() == "O");
        CHECK(topology[1].label() == "HO1");
        CHECK(topology[1].element() == "H");
        CHECK(topology[2].label() == "HO2");
        CHECK(topology[2].element() == "H");

        CHECK(topology.residues().size() == 5);
        auto residue = topology.residues()[0];
        CHECK(residue.size() == 3);
        CHECK(residue.contains(0));
        CHECK(residue.contains(1));
        CHECK(residue.contains(2));

        auto bonds = topology.bonds();
        auto expected = std::vector<Bond>{
            {0, 1}, {0, 2}, {3, 4}, {3, 5}, {6, 7}, {6, 8},
            {9, 10}, {9, 11}, {12, 13}, {12, 14}
        };

        CHECK(bonds.size() == expected.size());
        for (auto bond: expected) {
            CHECK(std::find(bonds.begin(), bonds.end(), bond) != bonds.end());
        }
    }
}
