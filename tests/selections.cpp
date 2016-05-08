#include <catch.hpp>
#include "chemfiles.hpp"

using namespace chemfiles;

Frame testing_frame();

TEST_CASE("Selections", "[selection]") {
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

    SECTION("name") {
        auto sel = Selection("name O");
        auto res = std::vector<size_t>{1, 2};
        CHECK(sel.list(frame) == res);

        sel = Selection("name != O");
        res = std::vector<size_t>{0, 3};
        CHECK(sel.list(frame) == res);
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

        sel = Selection("index > 1 and name H");
        res = std::vector<size_t>{3};
        CHECK(sel.list(frame) == res);
    }

    SECTION("or") {
        auto sel = Selection("index < 1 or index > 2");
        auto res = std::vector<size_t>{0, 3};
        CHECK(sel.list(frame) == res);

        sel = Selection("index == 1 or name H");
        res = std::vector<size_t>{0, 1, 3};
        CHECK(sel.list(frame) == res);
    }

    SECTION("not") {
        auto sel = Selection("not index > 2");
        auto res = std::vector<size_t>{0, 1, 2};
        CHECK(sel.list(frame) == res);

        sel = Selection("not name H");
        res = std::vector<size_t>{1, 2};
        CHECK(sel.list(frame) == res);
    }
}

Frame testing_frame() {
    auto topology = Topology();
    topology.append(Atom("H"));
    topology.append(Atom("O"));
    topology.append(Atom("O"));
    topology.append(Atom("H"));

    auto frame = Frame(topology);
    float i = 0;
    for (auto& pos: frame.positions()) {
        pos[0] = i + 0;
        pos[1] = i + 1;
        pos[2] = i + 2;
        i += 1;
    }

    frame.add_velocities();
    i = 0;
    // FIXME: writing the loop as (auto& vel: *frame.velocities()) gives a
    // segfault using gcc 5.3 on OS X with optimization level O2 and O3. No
    // idea why ...
    auto span = frame.velocities();
    for (auto& vel: *span) {
        vel[0] = i + 1;
        vel[1] = i + 2;
        vel[2] = i + 0;
        i += 1;
    }

    return frame;
}
