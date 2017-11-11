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
        auto sel = Selection("all");
        auto res = std::vector<size_t>{0, 1, 2, 3};
        CHECK(sel.list(frame) == res);

        sel = Selection("none");
        res = std::vector<size_t>{};
        CHECK(sel.list(frame) == res);

        sel = Selection("index != 2 and all");
        res = std::vector<size_t>{0, 1, 3};
        CHECK(sel.list(frame) == res);

        CHECK(sel.string() == "index != 2 and all");
    }

    SECTION("index") {
        auto sel = Selection("index == 2");
        auto res = std::vector<size_t>{2};
        CHECK(sel.list(frame) == res);

        sel = Selection("index != 2");
        res = std::vector<size_t>{0, 1, 3};
        CHECK(sel.list(frame) == res);
    }

    SECTION("mass") {
        auto sel = Selection("mass < 2");
        auto res = std::vector<size_t>{0, 3};
        CHECK(sel.list(frame) == res);
    }

    SECTION("type") {
        auto sel = Selection("type O");
        auto res = std::vector<size_t>{1, 2};
        CHECK(sel.list(frame) == res);

        sel = Selection("type != O");
        res = std::vector<size_t>{0, 3};
        CHECK(sel.list(frame) == res);
    }

    SECTION("name") {
        auto sel = Selection("name O");
        auto res = std::vector<size_t>{1, 2};
        CHECK(sel.list(frame) == res);

        sel = Selection("name != O");
        res = std::vector<size_t>{0, 3};
        CHECK(sel.list(frame) == res);

        sel = Selection("name H1");
        res = std::vector<size_t>{0};
        CHECK(sel.list(frame) == res);
    }

    SECTION("resname") {
        auto sel = Selection("resname resime");
        auto res = std::vector<size_t>{2, 3};
        CHECK(sel.list(frame) == res);

        sel = Selection("resname != resime");
        CHECK(sel.list(frame).empty());

        sel = Selection("resname == water");
        CHECK(sel.list(frame).empty());
    }

    SECTION("resid") {
        auto sel = Selection("resid 3");
        auto res = std::vector<size_t>{2, 3};
        CHECK(sel.list(frame) == res);

        sel = Selection("resid < 5");
        res = std::vector<size_t>{2, 3};
        CHECK(sel.list(frame) == res);

        sel = Selection("resid != 3");
        CHECK(sel.list(frame).empty());
    }

    SECTION("positions") {
        auto sel = Selection("x < 2");
        auto res = std::vector<size_t>{0, 1};
        CHECK(sel.list(frame) == res);

        sel = Selection("y != 2");
        res = std::vector<size_t>{0, 2, 3};
        CHECK(sel.list(frame) == res);

        sel = Selection("z >= 10");
        res = std::vector<size_t>{};
        CHECK(sel.list(frame) == res);
    }

    SECTION("velocities") {
        auto sel = Selection("vz < 2");
        auto res = std::vector<size_t>{0, 1};
        CHECK(sel.list(frame) == res);

        sel = Selection("vx != 2");
        res = std::vector<size_t>{0, 2, 3};
        CHECK(sel.list(frame) == res);

        sel = Selection("vy >= 10");
        res = std::vector<size_t>{};
        CHECK(sel.list(frame) == res);
    }

    SECTION("and") {
        auto sel = Selection("index > 1 and index <= 2");
        auto res = std::vector<size_t>{2};
        CHECK(sel.list(frame) == res);

        sel = Selection("index > 1 and type H");
        res = std::vector<size_t>{3};
        CHECK(sel.list(frame) == res);
    }

    SECTION("or") {
        auto sel = Selection("index < 1 or index > 2");
        auto res = std::vector<size_t>{0, 3};
        CHECK(sel.list(frame) == res);

        sel = Selection("index == 1 or type H");
        res = std::vector<size_t>{0, 1, 3};
        CHECK(sel.list(frame) == res);
    }

    SECTION("not") {
        auto sel = Selection("not index > 2");
        auto res = std::vector<size_t>{0, 1, 2};
        CHECK(sel.list(frame) == res);

        sel = Selection("not type H");
        res = std::vector<size_t>{1, 2};
        CHECK(sel.list(frame) == res);
    }

    SECTION("Selection context") {
        auto sel = Selection("atoms: all");
        auto res = std::vector<size_t>{0, 1, 2, 3};
        CHECK(sel.list(frame) == res);

        sel = Selection("atoms : none");
        res = std::vector<size_t>{};
        CHECK(sel.list(frame) == res);

        sel = Selection("atoms :not type H");
        res = std::vector<size_t>{1, 2};
        CHECK(sel.list(frame) == res);

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
        auto sel = Selection("pairs: all");
        std::vector<Match> res{
            {0ul, 1ul}, {0ul, 2ul}, {0ul, 3ul},
            {1ul, 0ul}, {1ul, 2ul}, {1ul, 3ul},
            {2ul, 0ul}, {2ul, 1ul}, {2ul, 3ul},
            {3ul, 0ul}, {3ul, 1ul}, {3ul, 2ul}};
        CHECK(sel.evaluate(frame) == res);
        auto natoms = frame.size();
        CHECK(res.size() == natoms * (natoms - 1));

        sel = Selection("two: none");
        res = std::vector<Match>();
        CHECK(sel.evaluate(frame) == res);

        CHECK_THROWS_AS(sel.list(frame), SelectionError);
    }

    SECTION("Three") {
        auto sel = Selection("three: all");
        std::vector<Match> res{
            {0ul, 1ul, 2ul}, {0ul, 1ul, 3ul}, {0ul, 2ul, 1ul}, {0ul, 2ul, 3ul},
            {0ul, 3ul, 1ul}, {0ul, 3ul, 2ul}, {1ul, 0ul, 2ul}, {1ul, 0ul, 3ul},
            {1ul, 2ul, 0ul}, {1ul, 2ul, 3ul}, {1ul, 3ul, 0ul}, {1ul, 3ul, 2ul},
            {2ul, 0ul, 1ul}, {2ul, 0ul, 3ul}, {2ul, 1ul, 0ul}, {2ul, 1ul, 3ul},
            {2ul, 3ul, 0ul}, {2ul, 3ul, 1ul}, {3ul, 0ul, 1ul}, {3ul, 0ul, 2ul},
            {3ul, 1ul, 0ul}, {3ul, 1ul, 2ul}, {3ul, 2ul, 0ul}, {3ul, 2ul, 1ul}
        };
        CHECK(sel.evaluate(frame) == res);
        auto natoms = frame.size();
        CHECK(res.size() == natoms * (natoms - 1) * (natoms - 2));

        CHECK_THROWS_AS(sel.list(frame), SelectionError);
    }

    SECTION("Four") {
        auto sel = Selection("four: all");
        auto res = std::vector<Match>{
            {0ul, 1ul, 2ul, 3ul}, {0ul, 1ul, 3ul, 2ul}, {0ul, 2ul, 1ul, 3ul},
            {0ul, 2ul, 3ul, 1ul}, {0ul, 3ul, 1ul, 2ul}, {0ul, 3ul, 2ul, 1ul},
            {1ul, 0ul, 2ul, 3ul}, {1ul, 0ul, 3ul, 2ul}, {1ul, 2ul, 0ul, 3ul},
            {1ul, 2ul, 3ul, 0ul}, {1ul, 3ul, 0ul, 2ul}, {1ul, 3ul, 2ul, 0ul},
            {2ul, 0ul, 1ul, 3ul}, {2ul, 0ul, 3ul, 1ul}, {2ul, 1ul, 0ul, 3ul},
            {2ul, 1ul, 3ul, 0ul}, {2ul, 3ul, 0ul, 1ul}, {2ul, 3ul, 1ul, 0ul},
            {3ul, 0ul, 1ul, 2ul}, {3ul, 0ul, 2ul, 1ul}, {3ul, 1ul, 0ul, 2ul},
            {3ul, 1ul, 2ul, 0ul}, {3ul, 2ul, 0ul, 1ul}, {3ul, 2ul, 1ul, 0ul},
        };
        CHECK(sel.evaluate(frame) == res);
        auto natoms = frame.size();
        CHECK(res.size() == natoms * (natoms - 1) * (natoms - 2) * (natoms - 3));

        CHECK_THROWS_AS(sel.list(frame), SelectionError);
    }

    SECTION("Bonds") {
        auto sel = Selection("bonds: all");
        std::vector<Match> res{{0ul, 1ul}, {1ul, 2ul}, {2ul, 3ul}};
        auto eval = sel.evaluate(frame);
        CHECK(res.size() == eval.size());
        for (auto& match: res) {
            CHECK(std::find(eval.begin(), eval.end(), match) != eval.end());
        }

        sel = Selection("bonds: name(#1) O and type(#2) H");
        res = std::vector<Match>{{1ul, 0ul}, {2ul, 3ul}};
        eval = sel.evaluate(frame);
        CHECK(res.size() == eval.size());
        for (auto& match: res) {
            CHECK(std::find(eval.begin(), eval.end(), match) != eval.end());
        }

        CHECK_THROWS_AS(sel.list(frame), SelectionError);
    }

    SECTION("Angles") {
        auto sel = Selection("angles: all");
        std::vector<Match> res{{0ul, 1ul, 2ul}, {1ul, 2ul, 3ul}};
        auto eval = sel.evaluate(frame);
        CHECK(res.size() == eval.size());
        for (auto& match: res) {
            CHECK(std::find(eval.begin(), eval.end(), match) != eval.end());
        }

        sel = Selection("angles: name(#1) O and name(#2) O and type(#3) H");
        res = std::vector<Match>{{2ul, 1ul, 0ul}, {1ul, 2ul, 3ul}};
        eval = sel.evaluate(frame);
        CHECK(res.size() == eval.size());
        for (auto& match: res) {
            CHECK(std::find(eval.begin(), eval.end(), match) != eval.end());
        }

        CHECK_THROWS_AS(sel.list(frame), SelectionError);
    }

    SECTION("Dihedrals") {
        auto sel = Selection("dihedrals: all");
        std::vector<Match> res{{0ul, 1ul, 2ul, 3ul}};
        CHECK(sel.evaluate(frame) == res);

        sel = Selection("dihedrals: name(#3) O and name(#4) H1");
        res = std::vector<Match>{{3ul, 2ul, 1ul, 0ul}};
        auto eval = sel.evaluate(frame);

        CHECK(res.size() == eval.size());
        for (auto& match: res) {
            CHECK(std::find(eval.begin(), eval.end(), match) != eval.end());
        }

        CHECK_THROWS_AS(sel.list(frame), SelectionError);
    }
}

Frame testing_frame() {
    auto topology = Topology();
    topology.add_atom(Atom("H1", "H"));
    topology.add_atom(Atom("O"));
    topology.add_atom(Atom("O"));
    topology.add_atom(Atom("H"));

    topology.add_bond(0, 1);
    topology.add_bond(1, 2);
    topology.add_bond(2, 3);

    auto res = Residue("resime", 3);
    res.add_atom(2);
    res.add_atom(3);
    topology.add_residue(res);

    auto frame = Frame(topology);
    int i = 0;
    for (auto& pos: frame.positions()) {
        pos[0] = i + 0;
        pos[1] = i + 1;
        pos[2] = i + 2;
        i++;
    }

    frame.add_velocities();
    i = 0;
    // FIXME: writing the loop as (auto& vel: *frame.velocities()) gives a
    // segfault using gcc 5.3 on OS X with optimization level O2 and O3. No
    // idea why ...
    auto velocities = frame.velocities();
    for (auto& vel: *velocities) {
        vel[0] = i + 1;
        vel[1] = i + 2;
        vel[2] = i + 0;
        i += 1;
    }

    return frame;
}
