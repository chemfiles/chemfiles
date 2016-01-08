#include <catch.hpp>
#include "chemfiles.hpp"

using namespace chemfiles;

Frame testing_frame();

TEST_CASE("Selections", "[selection]") {
    auto frame = testing_frame();

    SECTION("all & none") {
        auto sel = Selection("all");
        auto res = std::vector<Bool>{true, true, true, true};
        CHECK(sel.evaluate(frame) == res);

        sel = Selection("none");
        res = std::vector<Bool>{false, false, false, false};
        CHECK(sel.evaluate(frame) == res);

        sel = Selection("index != 2 and all");
        res = std::vector<Bool>{true, true, false, true};
        CHECK(sel.evaluate(frame) == res);
    }

    SECTION("index") {
        auto sel = Selection("index == 2");
        auto res = std::vector<Bool>{false, false, true, false};
        CHECK(sel.evaluate(frame) == res);

        sel = Selection("index != 2");
        res = std::vector<Bool>{true, true, false, true};
        CHECK(sel.evaluate(frame) == res);
    }

    SECTION("mass") {
        auto sel = Selection("mass < 2");
        auto res = std::vector<Bool>{true, false, false, true};
        CHECK(sel.evaluate(frame) == res);
    }

    SECTION("name") {
        auto sel = Selection("name O");
        auto res = std::vector<Bool>{false, true, true, false};
        CHECK(sel.evaluate(frame) == res);

        sel = Selection("name != O");
        res = std::vector<Bool>{true, false, false, true};
        CHECK(sel.evaluate(frame) == res);
    }

    SECTION("positions") {
        auto sel = Selection("x < 2");
        auto res = std::vector<Bool>{true, true, false, false};
        CHECK(sel.evaluate(frame) == res);

        sel = Selection("y != 2");
        res = std::vector<Bool>{true, false, true, true};
        CHECK(sel.evaluate(frame) == res);

        sel = Selection("z >= 10");
        res = std::vector<Bool>{false, false, false, false};
        CHECK(sel.evaluate(frame) == res);
    }

    SECTION("velocities") {
        auto sel = Selection("vz < 2");
        auto res = std::vector<Bool>{true, true, false, false};
        CHECK(sel.evaluate(frame) == res);

        sel = Selection("vx != 2");
        res = std::vector<Bool>{true, false, true, true};
        CHECK(sel.evaluate(frame) == res);

        sel = Selection("vy >= 10");
        res = std::vector<Bool>{false, false, false, false};
        CHECK(sel.evaluate(frame) == res);
    }

    SECTION("and") {
        auto sel = Selection("index > 1 and index <= 2");
        auto res = std::vector<Bool>{false, false, true, false};
        CHECK(sel.evaluate(frame) == res);

        sel = Selection("index > 1 and name H");
        res = std::vector<Bool>{false, false, false, true};
        CHECK(sel.evaluate(frame) == res);
    }

    SECTION("or") {
        auto sel = Selection("index < 1 or index > 2");
        auto res = std::vector<Bool>{true, false, false, true};
        CHECK(sel.evaluate(frame) == res);

        sel = Selection("index == 1 or name H");
        res = std::vector<Bool>{true, true, false, true};
        CHECK(sel.evaluate(frame) == res);
    }

    SECTION("not") {
        auto sel = Selection("not index > 2");
        auto res = std::vector<Bool>{true, true, true, false};
        CHECK(sel.evaluate(frame) == res);

        sel = Selection("not name H");
        res = std::vector<Bool>{false, true, true, false};
        CHECK(sel.evaluate(frame) == res);
    }
}

Frame testing_frame() {
    auto topology = Topology();
    topology.append(Atom("H"));
    topology.append(Atom("O"));
    topology.append(Atom("O"));
    topology.append(Atom("H"));

    auto frame = Frame(topology);
    size_t i = 0;
    for (auto& pos: frame.positions()) {
        pos[0] = i;
        pos[1] = i + 1;
        pos[2] = i + 2;
        i++;
    }

    frame.velocities(Array3D(4));
    i = 0;
    for (auto& vel: *frame.velocities()) {
        vel[0] = i + 1;
        vel[1] = i + 2;
        vel[2] = i;
        i++;
    }

    return frame;
}
