// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <catch.hpp>
#include "chemfiles.hpp"
using namespace chemfiles;

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

        // Unknown selection kind
        CHECK_THROWS_AS(Selection("kind: all"), SelectionError);
        // Too much colons
        CHECK_THROWS_AS(Selection("atoms: pairs: atoms"), SelectionError);
        // Variable index is too big
        CHECK_THROWS_AS(Selection("pairs: name(#3) O"), SelectionError);
        CHECK_THROWS_AS(Selection("name(#2) O"), SelectionError);
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

        CHECK_THROWS_AS(selection.list(frame), SelectionError);
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

        CHECK_THROWS_AS(selection.list(frame), SelectionError);
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

        CHECK_THROWS_AS(selection.list(frame), SelectionError);
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

        CHECK_THROWS_AS(selection.list(frame), SelectionError);
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

        CHECK_THROWS_AS(selection.list(frame), SelectionError);
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

        CHECK_THROWS_AS(selection.list(frame), SelectionError);
    }
}

Frame testing_frame() {
    auto frame = Frame();
    frame.add_velocities();
    frame.add_atom(Atom("H1", "H"), {0.0, 1.0, 2.0}, {1.0, 2.0, 0.0});
    frame.add_atom(Atom("O"), {1.0, 2.0, 3.0}, {2.0, 3.0, 1.0});
    frame.add_atom(Atom("O"), {2.0, 3.0, 4.0}, {3.0, 4.0, 2.0});
    frame.add_atom(Atom("H"), {3.0, 4.0, 5.0}, {4.0, 5.0, 3.0});

    frame.add_bond(0, 1);
    frame.add_bond(1, 2);
    frame.add_bond(2, 3);

    auto residue = Residue("resime", 3);
    residue.add_atom(2);
    residue.add_atom(3);
    frame.add_residue(residue);

    return frame;
}
