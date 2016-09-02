#include <catch.hpp>
#include "chemfiles.hpp"
using namespace chemfiles;

TEST_CASE("Residue class usage", "[Residue]"){
    auto residue = Residue("ALA", 4);
    CHECK(residue.name() == "ALA");
    CHECK(residue.id() == 4);

    residue = Residue("GUA");
    CHECK(residue.name() == "GUA");
    CHECK(residue.id() == static_cast<size_t>(-1));

    CHECK(residue.size() == 0);
    residue.add_atom(0);
    residue.add_atom(30);
    residue.add_atom(56);
    CHECK(residue.size() == 3);

    residue.add_atom(56);
    CHECK(residue.size() == 3);

    std::set<size_t> atoms(residue.begin(), residue.end());
    CHECK(atoms.size() == 3);
    CHECK(atoms.find(0) != atoms.end());
    CHECK(atoms.find(30) != atoms.end());
    CHECK(atoms.find(56) != atoms.end());

    CHECK(residue.contains(56));
}
