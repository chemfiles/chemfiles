// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <catch.hpp>
#include "chemfiles/selections/parser.hpp"
#include "chemfiles/selections/expr.hpp"

using namespace chemfiles;
using namespace chemfiles::selections;

static Ast parse(std::string selection) {
    return Parser(Tokenizer(selection).tokenize()).parse();
}

static Ast parse_and_opt(std::string selection) {
    auto ast = Parser(Tokenizer(selection).tokenize()).parse();
    ast->optimize();
    return ast;
}

TEST_CASE("Parsing") {
    // This section uses the pretty-printing of AST to check the parsing
    SECTION("Operators") {
        auto ast = "and -> index(#1) == 1\n    -> index(#1) == 1";
        CHECK(parse("index == 1 and index == 1")->print() == ast);

        ast = "or -> index(#1) == 1\n   -> index(#1) == 1";
        CHECK(parse("index == 1 or index == 1")->print() == ast);

        ast = "not index(#1) == 1";
        CHECK(parse("not index == 1")->print() == ast);

        ast = "and -> index(#1) == 1\n    -> not index(#1) == 1";
        CHECK(parse("index == 1 and not index == 1")->print() == ast);

        ast = "or -> and -> index(#1) == 1\n          -> index(#1) == 1\n   -> index(#1) == 1";
        CHECK(parse("index == 1 and index == 1 or index == 1")->print() == ast);

        ast = "and -> index(#1) == 1\n    -> or -> index(#1) == 1\n          -> index(#1) == 1";
        CHECK(parse("index == 1 and (index == 1 or index == 1)")->print() == ast);

        CHECK_THROWS_AS(parse("name H and"), SelectionError);
        CHECK_THROWS_AS(parse("and name H"), SelectionError);
        CHECK_THROWS_AS(parse("name H or"), SelectionError);
        CHECK_THROWS_AS(parse("or name H"), SelectionError);
        CHECK_THROWS_AS(parse("not"), SelectionError);
        CHECK_THROWS_AS(parse("name not H"), SelectionError);
    }

    SECTION("all & none") {
        CHECK(parse("all")->print() == "all");
        CHECK(parse("none")->print() == "none");

        auto ast = "or -> all\n   -> name(#1) == H";
        CHECK(parse("all or name H")->print() == ast);

        ast = "or -> name(#1) == H\n   -> none";
        CHECK(parse("name H or none")->print() == ast);

        CHECK(parse("not all")->print() == "not all");
    }

    SECTION("boolean selectors") {
        auto ast = "is_bonded(#1, #3)";
        CHECK(parse("is_bonded(#1, #3)")->print() == ast);

        ast = "is_angle(#1, #3, #2)";
        CHECK(parse("is_angle(#1, #3, #2)")->print() == ast);

        ast = "is_dihedral(#1, #3, #2, #4)";
        CHECK(parse("is_dihedral(#1, #3, #2, #4)")->print() == ast);

        ast = "is_improper(#1, #3, #2, #2)";
        CHECK(parse("is_improper(#1, #3, #2, #2)")->print() == ast);

        ast = "is_bonded(#1, name O)";
        CHECK(parse("is_bonded(#1, name O)")->print() == ast);

        ast = "is_angle(name H, #2, name O)";
        CHECK(parse("is_angle(name H, #2, name O)")->print() == ast);
    }

    SECTION("type") {
        CHECK(parse("type == goo")->print() == "type(#1) == goo");
        CHECK(parse("type(#1) == goo")->print() == "type(#1) == goo");
        CHECK(parse("type goo")->print() == "type(#1) == goo");
        CHECK(parse("type(#3) goo")->print() == "type(#3) == goo");
        CHECK(parse("type != goo")->print() == "type(#1) != goo");
        CHECK(parse("type == \"45\"")->print() == "type(#1) == 45");

        CHECK_THROWS_AS(parse("type < bar"), SelectionError);
        CHECK_THROWS_AS(parse("type >= bar"), SelectionError);
    }

    SECTION("name") {
        CHECK(parse("name == goo")->print() == "name(#1) == goo");
        CHECK(parse("name(#1) == goo")->print() == "name(#1) == goo");
        CHECK(parse("name goo")->print() == "name(#1) == goo");
        CHECK(parse("name(#3) goo")->print() == "name(#3) == goo");
        CHECK(parse("name != goo")->print() == "name(#1) != goo");
        CHECK(parse("name \"45\"")->print() == "name(#1) == 45");
        CHECK(parse("name \"名\"")->print() == "name(#1) == 名");

        CHECK_THROWS_AS(parse("name < bar"), SelectionError);
        CHECK_THROWS_AS(parse("name >= bar"), SelectionError);
    }

    SECTION("index") {
        CHECK(parse("index == 4")->print() == "index(#1) == 4");
        CHECK(parse("index(#1) == 4")->print() == "index(#1) == 4");
        CHECK(parse("index 5")->print() == "index(#1) == 5");
        CHECK(parse("index(#2) 5")->print() == "index(#2) == 5");

        CHECK(parse("index <= 42")->print() == "index(#1) <= 42");
        CHECK(parse("index != 12")->print() == "index(#1) != 12");
        CHECK(parse("index >= 42.3")->print() == "index(#1) >= 42.300000");

        CHECK_THROWS_AS(parse("index == bar"), SelectionError);
    }

    SECTION("resname") {
        CHECK(parse("resname == goo")->print() == "resname(#1) == goo");
        CHECK(parse("resname(#1) == goo")->print() == "resname(#1) == goo");
        CHECK(parse("resname goo")->print() == "resname(#1) == goo");
        CHECK(parse("resname(#3) goo")->print() == "resname(#3) == goo");
        CHECK(parse("resname != goo")->print() == "resname(#1) != goo");
        CHECK(parse("resname \"45\"")->print() == "resname(#1) == 45");

        CHECK_THROWS_AS(parse("resname < bar"), SelectionError);
        CHECK_THROWS_AS(parse("resname >= bar"), SelectionError);
    }

    SECTION("resid") {
        CHECK(parse("resid == 4")->print() == "resid(#1) == 4");
        CHECK(parse("resid(#1) == 4")->print() == "resid(#1) == 4");
        CHECK(parse("resid 5")->print() == "resid(#1) == 5");
        CHECK(parse("resid(#2) 5")->print() == "resid(#2) == 5");

        CHECK(parse("resid <= 42")->print() == "resid(#1) <= 42");
        CHECK(parse("resid != 12")->print() == "resid(#1) != 12");
        CHECK(parse("resid >= 42.3")->print() == "resid(#1) >= 42.300000");

        CHECK_THROWS_AS(parse("resid == bar"), SelectionError);
    }

    SECTION("mass") {
        CHECK(parse("mass == 4")->print() == "mass(#1) == 4");
        CHECK(parse("mass(#1) == 4")->print() == "mass(#1) == 4");
        CHECK(parse("mass 5.4")->print() == "mass(#1) == 5.400000");
        CHECK(parse("mass(#2) 5")->print() == "mass(#2) == 5");

        CHECK(parse("mass <= 42")->print() == "mass(#1) <= 42");
        CHECK(parse("mass != 12")->print() == "mass(#1) != 12");

        CHECK_THROWS_AS(parse("mass <= bar"), SelectionError);
    }

    SECTION("Position & velocity") {
        CHECK(parse("x == 4.4")->print() == "x(#1) == 4.400000");
        CHECK(parse("x(#1) == 4")->print() == "x(#1) == 4");
        CHECK(parse("y < 4")->print() == "y(#1) < 4");
        CHECK(parse("z >= 4")->print() == "z(#1) >= 4");

        CHECK(parse("vx == 4")->print() == "vx(#1) == 4");
        CHECK(parse("vy < 4")->print() == "vy(#1) < 4");
        CHECK(parse("vz >= 4")->print() == "vz(#1) >= 4");

        CHECK_THROWS_AS(parse("x <= bar"), SelectionError);
        CHECK_THROWS_AS(parse("vy > bar"), SelectionError);
        CHECK_THROWS_AS(parse("z != bar"), SelectionError);
        CHECK_THROWS_AS(parse("vx == bar"), SelectionError);
    }

    SECTION("Multiple selections") {
        auto ast = "and -> mass(#1) < 4\n    -> name(#3) == O";
        CHECK(parse("mass(#1) < 4 and name(#3) O")->print() == ast);
        ast = "name(#4) != Cs";
        CHECK(parse("name(#4) != Cs")->print() == ast);
        ast = "or -> index(#1) < 4\n   -> name(#2) == H";
        CHECK(parse("index(#1) < 4 or name(#2) H")->print() == ast);
    }

    SECTION("Math selections") {
        SECTION("Unary plus and minus") {
            auto ast = "4 < 5";
            CHECK(parse("+4 < 5")->print() == ast);

            ast = "(-4) < 5";
            CHECK(parse("- 4 < 5")->print() == ast);

            ast = "((((3 - (-4)) + 5) + (-4)) - 8) < 5";
            CHECK(parse("3 - - 4 + + 5 + - 4 - +8 < 5")->print() == ast);

            ast = "(5 - (-(-(-(-(-(-(-4)))))))) < 5";
            CHECK(parse("+++++++ 5 --------4 < 5")->print() == ast);

            ast = "(5 + (-(-(-(-(-(-4))))))) < 5";
            CHECK(parse("5 + - + - + - + - + - + -4 < 5")->print() == ast);
        }

        SECTION("Sums") {
            auto ast = "(3 + 4) < 5";
            CHECK(parse("3 + 4 < 5")->print() == ast);
            CHECK(parse("3+4 < 5")->print() == ast);

            ast = "(3 - 4) < 5";
            CHECK(parse("3 - 4 < 5")->print() == ast);
            CHECK(parse("3-4 < 5")->print() == ast);

            ast = "((((3 - 4) + 5) + 4) - 8) < 5";
            CHECK(parse("3 - 4 + 5 + 4 - 8 < 5")->print() == ast);
            CHECK(parse("3-4+5+4-8<5")->print() == ast);
        }

        SECTION("Products") {
            auto ast = "(3 * 4) < 5";
            CHECK(parse("3 * 4 < 5")->print() == ast);
            CHECK(parse("3*4 < 5")->print() == ast);

            ast = "(3 / 4) < 5";
            CHECK(parse("3 / 4 < 5")->print() == ast);
            CHECK(parse("3/4 < 5")->print() == ast);

            ast = "(((3 * 4) / (5 * 4)) * 8) < 5";
            CHECK(parse("3 * 4 / (5 * 4) * 8 < 5")->print() == ast);
        }

        SECTION("Power") {
            auto ast = "3 ^(4) < 5";
            CHECK(parse("3 ^ 4 < 5")->print() == ast);
            CHECK(parse("3^4 < 5")->print() == ast);

            ast = "3 ^(4 ^(6)) < 5";
            CHECK(parse("3 ^ 4 ^ 6 < 5")->print() == ast);
        }

        SECTION("Functions") {
            auto ast = "sin(3) < 5";
            CHECK(parse("sin(3) < 5")->print() == ast);

            ast = "sin((3 - 4)) < 5";
            CHECK(parse("sin(3 - 4) < 5")->print() == ast);

            ast = "cos(1) < 5";
            CHECK(parse("cos(1) < 5")->print() == ast);

            ast = "tan(1) < 5";
            CHECK(parse("tan(1) < 5")->print() == ast);

            ast = "asin(1) < 5";
            CHECK(parse("asin(1) < 5")->print() == ast);

            ast = "acos(1) < 5";
            CHECK(parse("acos(1) < 5")->print() == ast);

            ast = "sqrt(1) < 5";
            CHECK(parse("sqrt(1) < 5")->print() == ast);

            ast = "rad2deg(1) < 5";
            CHECK(parse("rad2deg(1) < 5")->print() == ast);

            ast = "deg2rad(1) < 5";
            CHECK(parse("deg2rad(1) < 5")->print() == ast);
        }

        SECTION("Atomic functions") {
            auto ast = "distance(#1, #2) < 5";
            CHECK(parse("distance(#1, #2) < 5")->print() == ast);

            ast = "angle(#1, #3, #2) < 5";
            CHECK(parse("angle(#1, #3, #2) < 5")->print() == ast);

            ast = "dihedral(#1, #3, #2, #3) < 5";
            CHECK(parse("dihedral(#1, #3, #2, #3) < 5")->print() == ast);

            ast = "out_of_plane(#1, #3, #2, #4) < 5";
            CHECK(parse("out_of_plane(#1, #3, #2, #4) < 5")->print() == ast);
        }

        SECTION("Complex expressions") {
            auto ast = "(x(#1) ^(2) + y(#1) ^(2)) < 10 ^(2)";
            CHECK(parse("x ^ 2 + y ^ 2 < 10 ^ 2")->print() == ast);
            CHECK(parse("x^2 + y^2 < 10^2")->print() == ast);

            ast = "(3 - (4 * 2 ^(7))) < 5";
            CHECK(parse("3 - 4 * 2 ^ 7 < 5")->print() == ast);
        }
    }
}

TEST_CASE("Parsing errors") {
    std::vector<std::string> PARSE_FAIL = {
        // Giberish at the end of the selection
        "index == 23 6",
        "index == 23 njzk",
        "index == 23 !=",
        "index == 23 name == 1",
        // Bad usage of the boolean operators
        "index == 23 and ",
        "and index == 23",
        "not and index == 23",
        "index == 23 or ",
        "or index == 23",
        "not or index == 23",
        "index == 23 not index == 1",
        // string expressions with bad operators
        "name == <",
        "name > foo",
        "name >= foo",
        "name < foo",
        "name <= foo",
        "name ==",
        // identifiers as mathematical values
        "z == <",
        "y == bar",
        "x <= foo",
        "z bar",
        // https://github.com/chemfiles/chemfiles/issues/79
        "type(#1) Al and type(#2) O and type(#3) H )",
        // functions arity and arguments
        "distance(#1) < 5",
        "distance(x) < 5",
        "angle(#2, #3) < 5",
        "dihedral(#2, #3) < 5",
        "none(#2, #3)",
        "all(#2, #3)",
        "bonded(#2)",
        "is_angle(#2)",
        "is_dihedral(#2)",
        "is_improper(#2)",
        // Sub-selection
        "bonded(#2, name(#3) Zn)",
        "bonded(name N, name Zn)",
    };

    for (auto& failure: PARSE_FAIL) {
        CHECK_THROWS_AS(parse(failure), SelectionError);
    }
}

TEST_CASE("Optimizations") {
    SECTION("Doing something") {
        // unary plus/minus
        auto ast = "-4 == 5";
        CHECK(parse_and_opt("-4 == +5")->print() == ast);

        // Add
        ast = "7 == 5";
        CHECK(parse_and_opt("3 + 4 == 3 + 2")->print() == ast);

        // Sub
        ast = "7 == 5";
        CHECK(parse_and_opt("9 - 2 == 15 - 10")->print() == ast);

        // Mul
        ast = "12 == 6";
        CHECK(parse_and_opt("3 * 4 == 2 * 3")->print() == ast);

        // Div
        ast = "12 == 6";
        CHECK(parse_and_opt("24 / 2 == 24 / 4")->print() == ast);

        ast = "9 == 32";
        CHECK(parse_and_opt("3^2 == 2^5")->print() == ast);

        ast = "3 == 0.500000";
        CHECK(parse_and_opt("sqrt(9) == sin(asin(0.5))")->print() == ast);
    }

    SECTION("No optimization") {
        // unary plus/minus
        auto ast = "(-index(#1)) == 5";
        CHECK(parse_and_opt("-index == +5")->print() == ast);

        // Add
        ast = "(index(#1) + 2) == 5";
        CHECK(parse_and_opt("index + 2 == 5")->print() == ast);

        // Sub
        ast = "(index(#1) - 2) == 5";
        CHECK(parse_and_opt("index - 2 == 5")->print() == ast);

        // Mul
        ast = "(index(#1) * 2) == 5";
        CHECK(parse_and_opt("index * 2 == 5")->print() == ast);

        // Div
        ast = "(index(#1) / 2) == 5";
        CHECK(parse_and_opt("index / 2 == 5")->print() == ast);

        ast = "index(#1) ^(2) == 5";
        CHECK(parse_and_opt("index ^ 2 == 5")->print() == ast);

        ast = "sqrt(index(#1)) == 5";
        CHECK(parse_and_opt("sqrt(index) == 5")->print() == ast);
    }
}
