// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>
#include "chemfiles.hpp"
using namespace chemfiles;

#include <iostream>

static Frame testing_frame();

TEST_CASE("Match class") {
    auto match = Match(1ul, 2ul, 3ul);
    CHECK(match.size() == 3);

    CHECK(match != Match(1ul, 2ul));
    CHECK(match != Match(1ul, 2ul, 4ul));
    CHECK(match == Match(1ul, 2ul, 3ul));

    CHECK_THROWS_AS(Match(1ul, 2ul)[3], OutOfBounds);
}

TEST_CASE("Atoms selections") {
    auto frame = testing_frame();

    SECTION("all & none") {
        auto selection = Selection("all");
        auto expected = std::vector<size_t>{0, 1, 2, 3};
        CHECK(selection.list(frame) == expected);

        selection = Selection("none");
        expected = std::vector<size_t>{};
        CHECK(selection.list(frame) == expected);

        selection = Selection("index != 2 and all");
        expected = std::vector<size_t>{0, 1, 3};
        CHECK(selection.list(frame) == expected);

        CHECK(selection.string() == "index != 2 and all");
    }

    SECTION("index") {
        auto selection = Selection("index == 2");
        auto expected = std::vector<size_t>{2};
        CHECK(selection.list(frame) == expected);

        selection = Selection("index != 2");
        expected = std::vector<size_t>{0, 1, 3};
        CHECK(selection.list(frame) == expected);
    }

    SECTION("mass") {
        auto selection = Selection("mass < 2");
        auto expected = std::vector<size_t>{0, 3};
        CHECK(selection.list(frame) == expected);
    }

    SECTION("type") {
        auto selection = Selection("type O");
        auto expected = std::vector<size_t>{1, 2};
        CHECK(selection.list(frame) == expected);

        selection = Selection("type != O");
        expected = std::vector<size_t>{0, 3};
        CHECK(selection.list(frame) == expected);
    }

    SECTION("name") {
        auto selection = Selection("name O");
        auto expected = std::vector<size_t>{1, 2};
        CHECK(selection.list(frame) == expected);

        selection = Selection("name != O");
        expected = std::vector<size_t>{0, 3};
        CHECK(selection.list(frame) == expected);

        selection = Selection("name H1");
        expected = std::vector<size_t>{0};
        CHECK(selection.list(frame) == expected);
    }

    SECTION("resname") {
        auto selection = Selection("resname resime");
        auto expected = std::vector<size_t>{2, 3};
        CHECK(selection.list(frame) == expected);

        selection = Selection("resname != resime");
        expected = std::vector<size_t>{0, 1};
        CHECK(selection.list(frame) == expected);

        selection = Selection("resname == water");
        CHECK(selection.list(frame).empty());
    }

    SECTION("resid") {
        auto selection = Selection("resid 3");
        auto expected = std::vector<size_t>{2, 3};
        CHECK(selection.list(frame) == expected);

        selection = Selection("resid < 5");
        expected = std::vector<size_t>{0, 1, 2, 3};
        CHECK(selection.list(frame) == expected);

        selection = Selection("resid != 3");
        expected = std::vector<size_t>{0, 1};
        CHECK(selection.list(frame) == expected);
    }

    SECTION("positions") {
        auto selection = Selection("x < 2");
        auto expected = std::vector<size_t>{0, 1};
        CHECK(selection.list(frame) == expected);

        selection = Selection("y != 2");
        expected = std::vector<size_t>{0, 2, 3};
        CHECK(selection.list(frame) == expected);

        selection = Selection("z >= 10");
        expected = std::vector<size_t>{};
        CHECK(selection.list(frame) == expected);
    }

    SECTION("velocities") {
        // The frame does not have velocities
        CHECK(Selection("vz == 0").list(frame).empty());
        CHECK(Selection("vx == 2").list(frame).empty());
        CHECK(Selection("vy >= 10").list(frame).empty());

        frame.add_velocities();
        auto velocities = *frame.velocities();
        velocities[0] = {1.0, 2.0, 0.0};
        velocities[1] = {2.0, 3.0, 1.0};
        velocities[2] = {3.0, 4.0, 2.0};
        velocities[3] = {4.0, 5.0, 3.0};

        auto selection = Selection("vz < 2");
        auto expected = std::vector<size_t>{0, 1};
        CHECK(selection.list(frame) == expected);

        selection = Selection("vx != 2");
        expected = std::vector<size_t>{0, 2, 3};
        CHECK(selection.list(frame) == expected);

        selection = Selection("vy >= 10");
        expected = std::vector<size_t>{};
        CHECK(selection.list(frame) == expected);
    }

    SECTION("is_bonded") {
        auto selection = Selection("two: name(#1) H1 and is_bonded(#1, #2)");
        auto expected = std::vector<Match>{{0ul, 1ul}};
        CHECK(selection.evaluate(frame) == expected);

        selection = Selection("is_bonded(#1, name H1)");
        expected = std::vector<Match>{{1ul}};
        CHECK(selection.evaluate(frame) == expected);

        auto first = Selection("two: type(#1) H and name(#2) O and is_bonded(#1, #2)");
        auto second = Selection("bonds: type(#1) H and name(#2) O");
        CHECK(first.evaluate(frame) == second.evaluate(frame));
    }

    SECTION("is_angle") {
        auto selection = Selection("three: name(#1) H1 and is_angle(#1, #3, #2)");
        auto expected = std::vector<Match>{{0ul, 2ul, 1ul}};
        CHECK(selection.evaluate(frame) == expected);

        auto first = Selection("three: type(#1) H and name(#2) O and is_angle(#1, #2, #3)");
        auto second = Selection("angles: type(#1) H and name(#2) O");
        CHECK(first.evaluate(frame) == second.evaluate(frame));
    }

    SECTION("is_dihedral") {
        auto selection = Selection("four: name(#1) H1 and is_dihedral(#3, #4, #2, #1)");
        auto expected = std::vector<Match>{{0ul, 1ul, 3ul, 2ul}};
        CHECK(selection.evaluate(frame) == expected);
    }

    SECTION("is_improper") {
        frame.add_atom(Atom("C"), Vector3D(), Vector3D());
        frame.add_bond(4, 1);

        auto selection = Selection("four: name(#1) H1 and is_improper(#1, #2, #3, #4)");
        auto expected = std::vector<Match>{{0ul, 1ul, 2ul, 4ul}, {0ul, 1ul, 4ul, 2ul}};
        CHECK(selection.evaluate(frame) == expected);
    }

    SECTION("and") {
        auto selection = Selection("index > 1 and index <= 2");
        auto expected = std::vector<size_t>{2};
        CHECK(selection.list(frame) == expected);

        selection = Selection("index > 1 and type H");
        expected = std::vector<size_t>{3};
        CHECK(selection.list(frame) == expected);
    }

    SECTION("or") {
        auto selection = Selection("index < 1 or index > 2");
        auto expected = std::vector<size_t>{0, 3};
        CHECK(selection.list(frame) == expected);

        selection = Selection("index == 1 or type H");
        expected = std::vector<size_t>{0, 1, 3};
        CHECK(selection.list(frame) == expected);
    }

    SECTION("not") {
        auto selection = Selection("not index > 2");
        auto expected = std::vector<size_t>{0, 1, 2};
        CHECK(selection.list(frame) == expected);

        selection = Selection("not type H");
        expected = std::vector<size_t>{1, 2};
        CHECK(selection.list(frame) == expected);
    }

    SECTION("Selection context") {
        auto selection = Selection("atoms: all");
        auto expected = std::vector<size_t>{0, 1, 2, 3};
        CHECK(selection.list(frame) == expected);

        selection = Selection("atoms : none");
        expected = std::vector<size_t>{};
        CHECK(selection.list(frame) == expected);

        selection = Selection("atoms :not type H");
        expected = std::vector<size_t>{1, 2};
        CHECK(selection.list(frame) == expected);

        CHECK_THROWS_WITH(Selection("kind: all"), "unknown selection context 'kind' in 'kind: all'");
        CHECK_THROWS_WITH(Selection("atoms: pairs: atoms"), "can not get selection context in 'atoms: pairs: atoms', too many ':'");
        CHECK_THROWS_WITH(Selection("pairs: name(#3) O"), "variable index 3 is too big for the current context (should be <= 2)");
        CHECK_THROWS_WITH(Selection("name(#2) O"), "variable index 2 is too big for the current context (should be <= 1)");
    }

    SECTION("math") {
        auto selection = Selection("x + 2 < 4");
        auto expected = std::vector<size_t>{0, 1};
        CHECK(selection.list(frame) == expected);

        selection = Selection("x - 2 < 0");
        CHECK(selection.list(frame) == expected);

        selection = Selection("-x > -2");
        CHECK(selection.list(frame) == expected);

        selection = Selection("x^2 > 3");
        expected = std::vector<size_t>{2, 3};
        CHECK(selection.list(frame) == expected);

        selection = Selection("sqrt(x^2) > sqrt(3)");
        CHECK(selection.list(frame) == expected);

        selection = Selection("y / 2 != 1");
        expected = std::vector<size_t>{0, 2, 3};
        CHECK(selection.list(frame) == expected);

        selection = Selection("z * 5 >= 50");
        expected = std::vector<size_t>{};
        CHECK(selection.list(frame) == expected);

        selection = Selection("index % 2 == 0");
        expected = std::vector<size_t>{0, 2};
        CHECK(selection.list(frame) == expected);
    }

    SECTION("numeric functions") {
        auto selection = Selection("four: distance(#1, #2) > 4");
        auto expected = std::vector<Match>{
            {0ul, 3ul, 1ul, 2ul}, {0ul, 3ul, 2ul, 1ul},
            {3ul, 0ul, 1ul, 2ul}, {3ul, 0ul, 2ul, 1ul},
        };
        CHECK(selection.evaluate(frame) == expected);

        selection = Selection("distance(#1, name O) > 3");
        expected = std::vector<Match>{{0ul}, {3ul}};
        CHECK(selection.evaluate(frame) == expected);

        selection = Selection("four: angle(#1, #2, #3) > deg2rad(120)");
        expected = std::vector<Match>{
            {0ul, 1ul, 2ul, 3ul}, {0ul, 1ul, 3ul, 2ul}, {0ul, 2ul, 3ul, 1ul},
            {1ul, 2ul, 3ul, 0ul}, {2ul, 1ul, 0ul, 3ul}, {3ul, 1ul, 0ul, 2ul},
            {3ul, 2ul, 0ul, 1ul}, {3ul, 2ul, 1ul, 0ul},
        };
        CHECK(selection.evaluate(frame) == expected);

        selection = Selection("angle(#1, name O, name H) > deg2rad(90)");
        expected = std::vector<Match>{{0ul}, {1ul}};
        CHECK(selection.evaluate(frame) == expected);

        // all atoms are co-linear in the test frame, add more of them to get
        // non-zero dihedral/out of plane values
        frame.add_atom(Atom("Cl"), {0, 0, 0});
        frame.add_atom(Atom("F"), {2, -2, 2});

        Trajectory("tmp.pdb", 'w').write(frame);

        selection = Selection("four: dihedral(#1, #2, #3, #4) > deg2rad(120) and name(#1) H1 and name(#2) Cl");
        expected = std::vector<Match>{
            {0ul, 4ul, 1ul, 2ul}, {0ul, 4ul, 1ul, 3ul}, {0ul, 4ul, 2ul, 3ul},
        };
        CHECK(selection.evaluate(frame) == expected);

        selection = Selection("dihedral(#1, name Cl, name O, name H) > deg2rad(30)");
        expected = std::vector<Match>{{0ul}, {1ul}};
        CHECK(selection.evaluate(frame) == expected);

        selection = Selection("four: out_of_plane(#1, #2, #3, #4) > 3 and name(#1) F");
        expected = std::vector<Match>{{5ul, 3ul, 4ul, 0ul}};
        CHECK(selection.evaluate(frame) == expected);

        selection = Selection("pairs: out_of_plane(#1, name O, name Cl, #2) > 1");
        expected = std::vector<Match>{{5ul, 0ul}};
        CHECK(selection.evaluate(frame) == expected);
    }
}

TEST_CASE("Multiple selections") {
    auto frame = testing_frame();

    SECTION("Pairs & two") {
        auto selection = Selection("pairs: all");
        std::vector<Match> expected{
            {0ul, 1ul}, {0ul, 2ul}, {0ul, 3ul},
            {1ul, 0ul}, {1ul, 2ul}, {1ul, 3ul},
            {2ul, 0ul}, {2ul, 1ul}, {2ul, 3ul},
            {3ul, 0ul}, {3ul, 1ul}, {3ul, 2ul}};
        CHECK(selection.evaluate(frame) == expected);
        auto natoms = frame.size();
        CHECK(expected.size() == natoms * (natoms - 1));

        selection = Selection("two: none");
        expected = std::vector<Match>();
        CHECK(selection.evaluate(frame) == expected);

        CHECK_THROWS_WITH(selection.list(frame), "can not call `Selection::list` on a multiple selection");
    }

    SECTION("Three") {
        auto selection = Selection("three: all");
        std::vector<Match> expected{
            {0ul, 1ul, 2ul}, {0ul, 1ul, 3ul}, {0ul, 2ul, 1ul}, {0ul, 2ul, 3ul},
            {0ul, 3ul, 1ul}, {0ul, 3ul, 2ul}, {1ul, 0ul, 2ul}, {1ul, 0ul, 3ul},
            {1ul, 2ul, 0ul}, {1ul, 2ul, 3ul}, {1ul, 3ul, 0ul}, {1ul, 3ul, 2ul},
            {2ul, 0ul, 1ul}, {2ul, 0ul, 3ul}, {2ul, 1ul, 0ul}, {2ul, 1ul, 3ul},
            {2ul, 3ul, 0ul}, {2ul, 3ul, 1ul}, {3ul, 0ul, 1ul}, {3ul, 0ul, 2ul},
            {3ul, 1ul, 0ul}, {3ul, 1ul, 2ul}, {3ul, 2ul, 0ul}, {3ul, 2ul, 1ul}
        };
        CHECK(selection.evaluate(frame) == expected);
        auto natoms = frame.size();
        CHECK(expected.size() == natoms * (natoms - 1) * (natoms - 2));

        CHECK_THROWS_WITH(selection.list(frame), "can not call `Selection::list` on a multiple selection");
    }

    SECTION("Four") {
        auto selection = Selection("four: all");
        auto expected = std::vector<Match>{
            {0ul, 1ul, 2ul, 3ul}, {0ul, 1ul, 3ul, 2ul}, {0ul, 2ul, 1ul, 3ul},
            {0ul, 2ul, 3ul, 1ul}, {0ul, 3ul, 1ul, 2ul}, {0ul, 3ul, 2ul, 1ul},
            {1ul, 0ul, 2ul, 3ul}, {1ul, 0ul, 3ul, 2ul}, {1ul, 2ul, 0ul, 3ul},
            {1ul, 2ul, 3ul, 0ul}, {1ul, 3ul, 0ul, 2ul}, {1ul, 3ul, 2ul, 0ul},
            {2ul, 0ul, 1ul, 3ul}, {2ul, 0ul, 3ul, 1ul}, {2ul, 1ul, 0ul, 3ul},
            {2ul, 1ul, 3ul, 0ul}, {2ul, 3ul, 0ul, 1ul}, {2ul, 3ul, 1ul, 0ul},
            {3ul, 0ul, 1ul, 2ul}, {3ul, 0ul, 2ul, 1ul}, {3ul, 1ul, 0ul, 2ul},
            {3ul, 1ul, 2ul, 0ul}, {3ul, 2ul, 0ul, 1ul}, {3ul, 2ul, 1ul, 0ul},
        };
        CHECK(selection.evaluate(frame) == expected);
        auto natoms = frame.size();
        CHECK(expected.size() == natoms * (natoms - 1) * (natoms - 2) * (natoms - 3));

        CHECK_THROWS_WITH(selection.list(frame), "can not call `Selection::list` on a multiple selection");
    }

    SECTION("Bonds") {
        auto selection = Selection("bonds: all");
        std::vector<Match> expected{{0ul, 1ul}, {1ul, 2ul}, {2ul, 3ul}};
        auto eval = selection.evaluate(frame);
        CHECK(expected.size() == eval.size());
        for (auto& match: expected) {
            CHECK(std::find(eval.begin(), eval.end(), match) != eval.end());
        }

        selection = Selection("bonds: name(#1) O and type(#2) H");
        expected = std::vector<Match>{{1ul, 0ul}, {2ul, 3ul}};
        eval = selection.evaluate(frame);
        CHECK(expected.size() == eval.size());
        for (auto& match: expected) {
            CHECK(std::find(eval.begin(), eval.end(), match) != eval.end());
        }

        CHECK_THROWS_WITH(selection.list(frame), "can not call `Selection::list` on a multiple selection");
    }

    SECTION("Angles") {
        auto selection = Selection("angles: all");
        std::vector<Match> expected{{0ul, 1ul, 2ul}, {1ul, 2ul, 3ul}};
        auto eval = selection.evaluate(frame);
        CHECK(expected.size() == eval.size());
        for (auto& match: expected) {
            CHECK(std::find(eval.begin(), eval.end(), match) != eval.end());
        }

        selection = Selection("angles: name(#1) O and name(#2) O and type(#3) H");
        expected = std::vector<Match>{{2ul, 1ul, 0ul}, {1ul, 2ul, 3ul}};
        eval = selection.evaluate(frame);
        CHECK(expected.size() == eval.size());
        for (auto& match: expected) {
            CHECK(std::find(eval.begin(), eval.end(), match) != eval.end());
        }

        CHECK_THROWS_WITH(selection.list(frame), "can not call `Selection::list` on a multiple selection");
    }

    SECTION("Dihedrals") {
        auto selection = Selection("dihedrals: all");
        std::vector<Match> expected{{0ul, 1ul, 2ul, 3ul}};
        CHECK(selection.evaluate(frame) == expected);

        selection = Selection("dihedrals: name(#3) O and name(#4) H1");
        expected = std::vector<Match>{{3ul, 2ul, 1ul, 0ul}};
        auto eval = selection.evaluate(frame);

        CHECK(expected.size() == eval.size());
        for (auto& match: expected) {
            CHECK(std::find(eval.begin(), eval.end(), match) != eval.end());
        }

        CHECK_THROWS_WITH(selection.list(frame), "can not call `Selection::list` on a multiple selection");
    }

    SECTION("Properties") {
        auto selection = Selection("[numeric] == 3");
        CHECK(selection.list(frame) == std::vector<size_t>{0ul});

        selection = Selection("[bool] and all");
        CHECK(selection.list(frame) == std::vector<size_t>{1ul});

        // No distinction between missing value and false value
        selection = Selection("not [bool]");
        CHECK(selection.list(frame) == (std::vector<size_t>{0ul, 2ul, 3ul}));

        selection = Selection("[string] == foo");
        CHECK(selection.list(frame) == std::vector<size_t>{2ul});

        // No distinction between missing value and false value
        selection = Selection("[string] != foo");
        CHECK(selection.list(frame) == (std::vector<size_t>{0ul, 1ul, 3ul}));

        selection = Selection("[\"string space\"] == \"foo bar\"");
        CHECK(selection.list(frame) == std::vector<size_t>{3ul});

        selection = Selection("[string] and all");
        CHECK_THROWS_WITH(selection.list(frame), "invalid type for property [string] on atom 2: expected bool, got string");

        selection = Selection("[bool] == foo");
        CHECK_THROWS_WITH(selection.list(frame), "invalid type for property [bool] on atom 1: expected string, got bool");

        selection = Selection("[string] < 34");
        CHECK_THROWS_WITH(selection.list(frame), "invalid type for property [string] on atom 2: expected double, got string");

        selection = Selection("[vector] < 34");
        CHECK_THROWS_WITH(selection.list(frame), "invalid type for property [vector] on atom 0: expected double, got Vector3D");

        selection = Selection("[res_numeric] < 3.15");
        CHECK(selection.list(frame) == std::vector<size_t>{2ul, 3ul});

        selection = Selection("[res_bool]");
        CHECK(selection.list(frame) == std::vector<size_t>{2ul, 3ul});

        selection = Selection("[res_string] == foo");
        CHECK(selection.list(frame) == std::vector<size_t>{2ul, 3ul});

        selection = Selection("[absent] == foo");
        CHECK(selection.list(frame).size()==0);

        //atom property has precedence over residue property
        selection = Selection("[string2] == foo");
        CHECK(selection.list(frame) == std::vector<size_t>{3ul});

        selection = Selection("[bool2]");
        CHECK(selection.list(frame) == std::vector<size_t>{2ul});

        selection = Selection("[numeric2] > 3");
        CHECK(selection.list(frame) == std::vector<size_t>{2ul});
    }
}

Frame testing_frame() {
    auto frame = Frame();
    frame.add_atom(Atom("H1", "H"), {0.0, 1.0, 2.0});
    frame.add_atom(Atom("O"), {1.0, 2.0, 3.0});
    frame.add_atom(Atom("O"), {2.0, 3.0, 4.0});
    frame.add_atom(Atom("H"), {3.0, 4.0, 5.0});

    frame.add_bond(0, 1);
    frame.add_bond(1, 2);
    frame.add_bond(2, 3);

    frame[0].set("numeric", 3);
    frame[1].set("bool", true);
    frame[2].set("bool", false);
    frame[2].set("bool2", true);
    frame[2].set("string", "foo");
    frame[2].set("numeric2", 3.14);
    frame[3].set("string", "bar");
    frame[3].set("string2", "foo");
    frame[3].set("string space", "foo bar");
    frame[0].set("vector", Vector3D(2.0, 3.0, 4.0));

    auto residue = Residue("resime", 3);
    residue.set("res_bool",true);
    residue.set("res_string","foo");
    residue.set("res_numeric",3.14);
    residue.set("string2","bar");
    residue.set("bool2", false);
    residue.set("numeric2",2.718);
    residue.add_atom(2);
    residue.add_atom(3);
    frame.add_residue(residue);

    return frame;
}
