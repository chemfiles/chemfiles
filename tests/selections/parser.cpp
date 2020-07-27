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
    SECTION("Generic errors") {
        CHECK_THROWS_WITH(parse(""), "empty selection");
        CHECK_THROWS_WITH(parse("3 < 5 name bar"), "additional data after the end of the selection: name bar");
        CHECK_THROWS_WITH(parse("("), "expected content after '('");
        CHECK_THROWS_WITH(parse("(name bar"), "expected closing parenthesis after 'bar'");
    }

    // This section uses the pretty-printing of AST to check the parsing
    SECTION("Boolean operators") {
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

        ast = "or -> not index(#1) == 1\n   -> index(#1) == 3";
        CHECK(parse("not index 1 or index 3")->print() == ast);

        CHECK_THROWS_WITH(parse("name H and"), "expected content after 'and'");
        CHECK_THROWS_WITH(parse("and name H"), "unexpected content: 'and'");
        CHECK_THROWS_WITH(parse("name H or"), "expected content after 'or'");
        CHECK_THROWS_WITH(parse("or name H"), "unexpected content: 'or'");
        CHECK_THROWS_WITH(parse("not"), "expected content after 'not'");
        CHECK_THROWS_WITH(parse("name not H"), "expected one of '!=', '==' or a string value after 'name', found 'not'");
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

        CHECK_THROWS_WITH(
            parse("is_bonded(#1, pairs: name O)"),
            "invalid character ':' in 'is_bonded(#1, pairs: name O)'"
        );
    }

    SECTION("type") {
        CHECK(parse("type == goo")->print() == "type(#1) == goo");
        CHECK(parse("type(#1) == goo")->print() == "type(#1) == goo");
        CHECK(parse("type goo")->print() == "type(#1) == goo");
        CHECK(parse("type(#3) goo")->print() == "type(#3) == goo");
        CHECK(parse("type != goo")->print() == "type(#1) != goo");
        CHECK(parse("type == \"45\"")->print() == "type(#1) == \"45\"");

        CHECK(parse("type goo")->print() == "type(#1) == goo");
        auto ast = "or -> type(#1) == goo\n   -> type(#1) == foo";
        CHECK(parse("type goo foo")->print() == ast);

        CHECK_THROWS_WITH(parse("type < bar"),
            "expected one of '!=', '==' or a string value after 'type', found '<'"
        );
        CHECK_THROWS_WITH(parse("type >= bar"),
            "expected one of '!=', '==' or a string value after 'type', found '>='"
        );
    }

    SECTION("name") {
        CHECK(parse("name == goo")->print() == "name(#1) == goo");
        CHECK(parse("name(#1) == goo")->print() == "name(#1) == goo");
        CHECK(parse("name goo")->print() == "name(#1) == goo");
        CHECK(parse("name(#3) goo")->print() == "name(#3) == goo");
        CHECK(parse("name != goo")->print() == "name(#1) != goo");
        CHECK(parse("name \"45\"")->print() == "name(#1) == \"45\"");
        CHECK(parse("name \"名\"")->print() == "name(#1) == \"名\"");

        CHECK(parse("name goo")->print() == "name(#1) == goo");
        auto ast = "or -> name(#1) == goo\n   -> name(#1) == foo";
        CHECK(parse("name goo foo")->print() == ast);

        CHECK_THROWS_WITH(parse("name < bar"),
            "expected one of '!=', '==' or a string value after 'name', found '<'"
        );
        CHECK_THROWS_WITH(parse("name >= bar"),
            "expected one of '!=', '==' or a string value after 'name', found '>='"
        );
        CHECK_THROWS_WITH(parse("name == <="),
            "expected a string value after 'name ==', found <="
        );
        CHECK_THROWS_WITH(parse("name != 4"),
            "expected a string value after 'name !=', found 4"
        );
    }

    SECTION("index") {
        CHECK(parse("index == 4")->print() == "index(#1) == 4");
        CHECK(parse("index(#1) == 4")->print() == "index(#1) == 4");
        CHECK(parse("index 5")->print() == "index(#1) == 5");
        CHECK(parse("index(#2) 5")->print() == "index(#2) == 5");

        CHECK(parse("index <= 42")->print() == "index(#1) <= 42");
        CHECK(parse("index != 12")->print() == "index(#1) != 12");
        CHECK(parse("index >= 42.3")->print() == "index(#1) >= 42.300000");

        CHECK(parse("index 4")->print() == "index(#1) == 4");
        auto ast = "or -> index(#1) == 4\n   -> index(#1) == 3";
        CHECK(parse("index 4 3")->print() == ast);

        CHECK_THROWS_WITH(parse("index == bar"), "unexpected identifier 'bar' in mathematical expression");
    }

    SECTION("resname") {
        CHECK(parse("resname == goo")->print() == "resname(#1) == goo");
        CHECK(parse("resname(#1) == goo")->print() == "resname(#1) == goo");
        CHECK(parse("resname goo")->print() == "resname(#1) == goo");
        CHECK(parse("resname(#3) goo")->print() == "resname(#3) == goo");
        CHECK(parse("resname != goo")->print() == "resname(#1) != goo");
        CHECK(parse("resname \"45\"")->print() == "resname(#1) == \"45\"");

        CHECK(parse("resname goo")->print() == "resname(#1) == goo");
        auto ast = "or -> resname(#1) == goo\n   -> resname(#1) == foo";
        CHECK(parse("resname goo foo")->print() == ast);

        CHECK_THROWS_WITH(parse("resname < bar"),
            "expected one of '!=', '==' or a string value after 'resname', found '<'"
        );
        CHECK_THROWS_WITH(parse("resname >= bar"),
            "expected one of '!=', '==' or a string value after 'resname', found '>='"
        );
    }

    SECTION("resid") {
        CHECK(parse("resid == 4")->print() == "resid(#1) == 4");
        CHECK(parse("resid(#1) == 4")->print() == "resid(#1) == 4");
        CHECK(parse("resid 5")->print() == "resid(#1) == 5");
        CHECK(parse("resid(#2) 5")->print() == "resid(#2) == 5");

        CHECK(parse("resid <= 42")->print() == "resid(#1) <= 42");
        CHECK(parse("resid != 12")->print() == "resid(#1) != 12");
        CHECK(parse("resid >= 42.3")->print() == "resid(#1) >= 42.300000");

        CHECK(parse("resid 4")->print() == "resid(#1) == 4");
        auto ast = "or -> resid(#1) == 4\n   -> resid(#1) == 3";
        CHECK(parse("resid 4 3")->print() == ast);

        CHECK_THROWS_WITH(parse("resid == bar"), "unexpected identifier 'bar' in mathematical expression");
    }

    SECTION("mass") {
        CHECK(parse("mass == 4")->print() == "mass(#1) == 4");
        CHECK(parse("mass(#1) == 4")->print() == "mass(#1) == 4");
        CHECK(parse("mass 5.4")->print() == "mass(#1) == 5.400000");
        CHECK(parse("mass(#2) 5")->print() == "mass(#2) == 5");

        CHECK(parse("mass <= 42")->print() == "mass(#1) <= 42");
        CHECK(parse("mass != 12")->print() == "mass(#1) != 12");

        CHECK(parse("mass 4")->print() == "mass(#1) == 4");
        auto ast = "or -> mass(#1) == 4\n   -> mass(#1) == 3";
        CHECK(parse("mass 4 3")->print() == ast);

        CHECK_THROWS_WITH(parse("mass <= bar"), "unexpected identifier 'bar' in mathematical expression");
    }

    SECTION("Position & velocity") {
        CHECK(parse("x == 4.4")->print() == "x(#1) == 4.400000");
        CHECK(parse("x(#1) == 4")->print() == "x(#1) == 4");
        CHECK(parse("y < 4")->print() == "y(#1) < 4");
        CHECK(parse("z >= 4")->print() == "z(#1) >= 4");

        CHECK(parse("vx == 4")->print() == "vx(#1) == 4");
        CHECK(parse("vy < 4")->print() == "vy(#1) < 4");
        CHECK(parse("vz >= 4")->print() == "vz(#1) >= 4");

        CHECK(parse("x 4")->print() == "x(#1) == 4");
        auto ast = "or -> x(#1) == 4\n   -> x(#1) == 3";
        CHECK(parse("x 4 3")->print() == ast);

        CHECK_THROWS_WITH(parse("x <= bar"), "unexpected identifier 'bar' in mathematical expression");
        CHECK_THROWS_WITH(parse("vy > bar"), "unexpected identifier 'bar' in mathematical expression");
        CHECK_THROWS_WITH(parse("z != bar"), "unexpected identifier 'bar' in mathematical expression");
        CHECK_THROWS_WITH(parse("vx == bar"), "unexpected identifier 'bar' in mathematical expression");
    }

    SECTION("Properties") {
        SECTION("Boolean") {
            CHECK(parse("[foo]")->print() == "[foo](#1)");
            CHECK(parse("[foo](#2)")->print() == "[foo](#2)");

            CHECK(parse("[\"foo\"]")->print() == "[foo](#1)");
            CHECK(parse("[\"foo bar\"]")->print() == "[\"foo bar\"](#1)");

            auto ast = "or -> [foo](#1)\n   -> [bar](#1)";
            CHECK(parse("[foo] or [bar]")->print() == ast);

            ast = "and -> [foo](#1)\n    -> [bar](#1)";
            CHECK(parse("[foo] and [bar]")->print() == ast);

            ast = "not [foo](#1)";
            CHECK(parse("not [foo]")->print() == ast);
        }

        SECTION("String") {
            CHECK(parse("[foo] == bar")->print() == "[foo](#1) == bar");
            CHECK(parse("[\"foo\"] != bar")->print() == "[foo](#1) != bar");
            CHECK(parse("[\"foo bar\"](#3) == bar")->print() == "[\"foo bar\"](#3) == bar");

            CHECK(parse("[foo] bar")->print() == "[foo](#1) == bar");
            auto ast = "or -> [foo](#1) == bar\n   -> [foo](#1) == \"fizz foo\"";
            CHECK(parse("[foo] bar \"fizz foo\"")->print() == ast);
        }

        SECTION("Numeric") {
            CHECK(parse("[foo] < 6")->print() == "[foo](#1) < 6");
            CHECK(parse("[foo] <= 6")->print() == "[foo](#1) <= 6");
            CHECK(parse("[foo] > 6")->print() == "[foo](#1) > 6");
            CHECK(parse("[foo] >= 6")->print() == "[foo](#1) >= 6");
            CHECK(parse("[foo] != 6")->print() == "[foo](#1) != 6");
            CHECK(parse("[\"foo\"] == 4")->print() == "[foo](#1) == 4");
            CHECK(parse("[\"foo bar\"] == 4")->print() == "[\"foo bar\"](#1) == 4");

            CHECK(parse("5 - [foo] == 4")->print() == "(5 - [foo](#1)) == 4");
            CHECK(parse("[foo] + 3 == 4")->print() == "([foo](#1) + 3) == 4");

            CHECK(parse("[foo](#3) == 4")->print() == "[foo](#3) == 4");
            CHECK(parse("[foo](#4) + 3 == 4")->print() == "([foo](#4) + 3) == 4");
        }

        SECTION("Error") {
            CHECK_THROWS_WITH(parse("[3] == bar"), "expected property name after [, got 3");
            CHECK_THROWS_WITH(parse("[3 + 5] == bar"), "expected property name after [, got 3");
            CHECK_THROWS_WITH(parse("[] == bar"), "expected property name after [, got ]");
            CHECK_THROWS_WITH(parse("[foo == bar"), "expected ] after [foo, got ==");
            CHECK_THROWS_WITH(parse("foo] == bar"), "unexpected identifier 'foo' in mathematical expression");
            CHECK_THROWS_WITH(parse("[foo(#1)] == bar"), "expected ] after [foo, got (");
            CHECK_THROWS_WITH(parse("[\"foo bar\"(#1)] == bar"), "expected ] after [\"foo bar\", got (");

            CHECK_THROWS_WITH(parse("[foo] < bar"), "unexpected identifier 'bar' in mathematical expression");
            CHECK_THROWS_WITH(parse("[foo] <= bar"), "unexpected identifier 'bar' in mathematical expression");
            CHECK_THROWS_WITH(parse("[foo] > bar"), "unexpected identifier 'bar' in mathematical expression");
            CHECK_THROWS_WITH(parse("[foo] >= bar"), "unexpected identifier 'bar' in mathematical expression");
        }
    }

    SECTION("Variables") {
        auto ast = "and -> mass(#1) < 4\n    -> name(#3) == O";
        CHECK(parse("mass(#1) < 4 and name(#3) O")->print() == ast);
        ast = "name(#4) != Cs";
        CHECK(parse("name(#4) != Cs")->print() == ast);
        ast = "or -> index(#1) < 4\n   -> name(#2) == H";
        CHECK(parse("index(#1) < 4 or name(#2) H")->print() == ast);

        CHECK_THROWS_WITH(parse("index(x)"), "expected variable in parenthesis, got 'x'");
        CHECK_THROWS_WITH(parse("index(#1"), "expected closing parenthesis after variable, got '<end of selection>'");

        CHECK_THROWS_WITH(parse("distance #1 #2"), "expected 2 arguments in 'distance', got 0");
        CHECK_THROWS_WITH(parse("distance(#1"), "expected closing parenthesis after variable, got '<end of selection>'");

        CHECK_THROWS_WITH(parse("is_bonded(#1"), "expected closing parenthesis after variable, got '<end of selection>'");
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

        SECTION("Modulo") {
            CHECK(parse("3 % 2 == 0")->print() == "(3 % 2) == 0");

            // Checking operators precedence
            CHECK(parse("1 + 3 % 2 == 0")->print() == "(1 + (3 % 2)) == 0");
            CHECK(parse("5 * 3 % 2 == 0")->print() == "((5 * 3) % 2) == 0");
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

            ast = "exp(1) < 5";
            CHECK(parse("exp(1) < 5")->print() == ast);

            ast = "log(1) < 5";
            CHECK(parse("log(1) < 5")->print() == ast);

            ast = "log2(1) < 5";
            CHECK(parse("log2(1) < 5")->print() == ast);

            ast = "log10(1) < 5";
            CHECK(parse("log10(1) < 5")->print() == ast);
        }

        SECTION("distance function") {
            auto ast = "distance(#1, #2) < 5";
            CHECK(parse("distance(#1, #2) < 5")->print() == ast);

            ast = "distance(#1, name O) < 5";
            CHECK(parse("distance(#1, name O) < 5")->print() == ast);

            ast = "distance(index 3, #1) < 5";
            CHECK(parse("distance(index 3, #1) < 5")->print() == ast);
        }

        SECTION("angle function") {
            auto ast = "angle(#1, #3, #2) < 5";
            CHECK(parse("angle(#1, #3, #2) < 5")->print() == ast);

            ast = "angle(name O, #3, #2) < 5";
            CHECK(parse("angle(name O, #3, #2) < 5")->print() == ast);

            ast = "angle(#1, name H or mass > 3 + 4, #2) < 5";
            CHECK(parse("angle(#1, name H or mass > 3 + 4, #2) < 5")->print() == ast);

            ast = "angle(#1, #3, name Zn) < 5";
            CHECK(parse("angle(#1, #3, name Zn) < 5")->print() == ast);
        }

        SECTION("dihedral function") {
            auto ast = "dihedral(#1, #3, #2, #4) < 5";
            CHECK(parse("dihedral(#1, #3, #2, #4) < 5")->print() == ast);

            ast = "dihedral(name O, #3, #2, #3) < 5";
            CHECK(parse("dihedral(name O, #3, #2, #3) < 5")->print() == ast);

            ast = "dihedral(name O, #3, name H, #3) < 5";
            CHECK(parse("dihedral(name O, #3, name H, #3) < 5")->print() == ast);

            ast = "dihedral(#1, #3, #2, all) < 5";
            CHECK(parse("dihedral(#1, #3, #2, all) < 5")->print() == ast);
        }

        SECTION("out_of_plane function") {
            auto ast = "out_of_plane(#1, #3, #2, #4) < 5";
            CHECK(parse("out_of_plane(#1, #3, #2, #4) < 5")->print() == ast);

            ast = "out_of_plane(name O, #3, #2, #3) < 5";
            CHECK(parse("out_of_plane(name O, #3, #2, #3) < 5")->print() == ast);

            ast = "out_of_plane(name O, #3, name H, #3) < 5";
            CHECK(parse("out_of_plane(name O, #3, name H, #3) < 5")->print() == ast);

            ast = "out_of_plane(#1, #3, #2, all) < 5";
            CHECK(parse("out_of_plane(#1, #3, #2, all) < 5")->print() == ast);
        }

        SECTION("Complex expressions") {
            auto ast = "(x(#1) ^(2) + y(#1) ^(2)) < 10 ^(2)";
            CHECK(parse("x ^ 2 + y ^ 2 < 10 ^ 2")->print() == ast);
            CHECK(parse("x^2 + y^2 < 10^2")->print() == ast);

            ast = "(1 + (2 * 3)) == 0";
            CHECK(parse("1 + 2 * 3 == 0")->print() == ast);
            ast = "(1 - (2 * 3)) == 0";
            CHECK(parse("1 - 2 * 3 == 0")->print() == ast);

            ast = "(1 + (2 / 3)) == 0";
            CHECK(parse("1 + 2 / 3 == 0")->print() == ast);
            ast = "(1 - (2 / 3)) == 0";
            CHECK(parse("1 - 2 / 3 == 0")->print() == ast);

            ast = "(1 + (2 % 3)) == 0";
            CHECK(parse("1 + 2 % 3 == 0")->print() == ast);
            ast = "(1 - (2 % 3)) == 0";
            CHECK(parse("1 - 2 % 3 == 0")->print() == ast);

            ast = "(1 + 2 ^(3)) == 0";
            CHECK(parse("1 + 2 ^ 3 == 0")->print() == ast);
            ast = "(1 - 2 ^(3)) == 0";
            CHECK(parse("1 - 2 ^ 3 == 0")->print() == ast);
            ast = "(1 * 2 ^(3)) == 0";
            CHECK(parse("1 * 2 ^ 3 == 0")->print() == ast);
            ast = "(1 / 2 ^(3)) == 0";
            CHECK(parse("1 / 2 ^ 3 == 0")->print() == ast);
            ast = "(1 % 2 ^(3)) == 0";
            CHECK(parse("1 % 2 ^ 3 == 0")->print() == ast);

            // Change order of evaluation with parenthesis
            ast = "((1 + 2) * 3) == 0";
            CHECK(parse("(1 + 2) * 3 == 0")->print() == ast);

            ast = "(1 + 2) ^(3) == 0";
            CHECK(parse("(1 + 2) ^ 3 == 0")->print() == ast);

            ast = "(1 + 2) ^((3 + 2)) == 0";
            CHECK(parse("(1 + 2) ^ (3 + 2) == 0")->print() == ast);
        }

        SECTION("Errors") {
            CHECK_THROWS_WITH(parse("index < (3 + 4"), "expected closing parenthesis after '4'");
            CHECK_THROWS_WITH(parse("index < ("), "expected content after '('");

            CHECK_THROWS_WITH(parse("index < [foo"), "expected ] after [foo, got <end of selection>");
            CHECK_THROWS_WITH(parse("index < [foo(#1)]"), "expected ] after [foo, got (");
            CHECK_THROWS_WITH(parse("index < [\"foo bar\"(#1)]"), "expected ] after [\"foo bar\", got (");

            CHECK_THROWS_WITH(parse("index < sin"), "missing parenthesis after 'sin' function");
            CHECK_THROWS_WITH(parse("index < sin(4"), "missing closing parenthesis after 'sin' function call");
        }
    }

    SECTION("Parenthesis") {
        auto ast = "or -> and -> (2 + 3) < 3\n          -> name(#1) == Zn\n   -> name(#1) == H";
        CHECK(parse("((2 +3) < 3 and name Zn) or name H")->print() == ast);
        ast = "and -> name(#1) == F\n    -> (3 + 4) < 67";
        CHECK(parse("name F and ((((3 + 4)))) < 67")->print() == ast);
    }
}

TEST_CASE("Parsing errors") {
    std::vector<std::pair<std::string, std::string>> PARSE_FAIL = {
        // Bad usage of the boolean operators
        {"index == 23 and ", "expected content after 'and'"},
        {"and index == 23", "unexpected content: 'and'"},
        {"not and index == 23", "unexpected content: 'and'"},
        {"index == 23 or ", "expected content after 'or'"},
        {"or index == 23", "unexpected content: 'or'"},
        {"not or index == 23", "unexpected content: 'or'"},
        {"index == 23 not index == 1", "additional data after the end of the selection: not index == 1"},
        // https://github.com/chemfiles/chemfiles/issues/79
        {"type(#1) Al and type(#2) O and type(#3) H )", "additional data after the end of the selection: )"},
        // functions arity and arguments
        {"distance(#1) < 5", "expected 2 arguments in 'distance', got 1"},
        {"angle(#2, #3) < 5", "expected 3 arguments in 'angle', got 2"},
        {"dihedral(#2, #3) < 5", "expected 4 arguments in 'dihedral', got 2"},
        {"none(#2, #3)", "expected 0 arguments in 'none', got 2"},
        {"is_bonded(#2)", "expected 2 arguments in 'is_bonded', got 1"},
        {"is_angle(#2)", "expected 3 arguments in 'is_angle', got 1"},
        {"is_dihedral(#2)", "expected 4 arguments in 'is_dihedral', got 1"},
        {"is_improper(#2)", "expected 4 arguments in 'is_improper', got 1"},
        // Sub-selection
        {"is_bonded(#1, name(#3) Zn)", "variable index 3 is too big for the current context (should be <= 1)"},
        {"is_bonded(name N, name Zn)", "expected at least one variable (#1/#2/#3/#4) in 'is_bonded'"},
    };

    for (auto& failure: PARSE_FAIL) {
        CHECK_THROWS_WITH(parse(failure.first), failure.second);
    }
}

TEST_CASE("Optimizations") {
    SECTION("Doing something") {
        CHECK(parse_and_opt("-4 == +5")->print() == "-4 == 5");

        // Add
        CHECK(parse_and_opt("3 + 4 == 3 + 2")->print() == "7 == 5");
        CHECK(parse_and_opt("3 == 3 + 2")->print() == "3 == 5");
        CHECK(parse_and_opt("3 + 4 == 3")->print() == "7 == 3");

        // Sub
        CHECK(parse_and_opt("9 - 2 == 15 - 10")->print() == "7 == 5");
        CHECK(parse_and_opt("9 == 15 - 10")->print() == "9 == 5");
        CHECK(parse_and_opt("9 - 2 == 15")->print() == "7 == 15");

        // Mul
        CHECK(parse_and_opt("3 * 4 == 2 * 3")->print() == "12 == 6");
        CHECK(parse_and_opt("3 == 2 * 3")->print() == "3 == 6");
        CHECK(parse_and_opt("3 * 4 == 3")->print() == "12 == 3");

        // Div
        CHECK(parse_and_opt("24 / 2 == 24 / 4")->print() == "12 == 6");
        CHECK(parse_and_opt("24 == 24 / 4")->print() == "24 == 6");
        CHECK(parse_and_opt("24 / 2 == 4")->print() == "12 == 4");

        // Pow
        CHECK(parse_and_opt("3^2 == 2^5")->print() == "9 == 32");
        CHECK(parse_and_opt("3 == 2^5")->print() == "3 == 32");
        CHECK(parse_and_opt("3^2 == 2")->print() == "9 == 2");

        // Mod
        CHECK(parse_and_opt("8 % 3 == 17 % 2")->print() == "2 == 1");
        CHECK(parse_and_opt("8 == 17 % 2")->print() == "8 == 1");
        CHECK(parse_and_opt("8 % 3 == 17")->print() == "2 == 17");

        // Functions
        CHECK(parse_and_opt("sqrt(9) == sin(asin(0.5))")->print() == "3 == 0.500000");
        CHECK(parse_and_opt("9 == sin(asin(0.5))")->print() == "9 == 0.500000");
        CHECK(parse_and_opt("sqrt(9) == 0.5")->print() == "3 == 0.500000");
    }

    SECTION("No optimization") {
        CHECK(parse_and_opt("-index == +5")->print() == "(-index(#1)) == 5");
        CHECK(parse_and_opt("index + 2 == 5")->print() == "(index(#1) + 2) == 5");
        CHECK(parse_and_opt("index - 2 == 5")->print() == "(index(#1) - 2) == 5");
        CHECK(parse_and_opt("index * 2 == 5")->print() == "(index(#1) * 2) == 5");
        CHECK(parse_and_opt("index / 2 == 5")->print() == "(index(#1) / 2) == 5");
        CHECK(parse_and_opt("index % 2 == 5")->print() == "(index(#1) % 2) == 5");
        CHECK(parse_and_opt("index ^ 2 == 5")->print() == "index(#1) ^(2) == 5");
        CHECK(parse_and_opt("sqrt(index) == 5")->print() == "sqrt(index(#1)) == 5");
    }
}
