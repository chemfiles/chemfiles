// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifdef __clang__
#pragma clang diagnostic ignored "-Wpotentially-evaluated-expression"
#endif

#include <fstream>
#include <catch.hpp>

#include "helpers.hpp"
#include "chemfiles.hpp"
#include "chemfiles/FormatFactory.hpp"
using namespace chemfiles;

// Dummy format clase
class DummyFormat: public Format {
public:
    DummyFormat(const std::string&, File::Mode){}
    size_t nsteps() override {return 42;}
};

TEST_CASE("Registering a new format"){
    FormatFactory::get().register_extension<DummyFormat>(".testing");
    // We can not register the same format twice
    CHECK_THROWS_AS(
        FormatFactory::get().register_extension<DummyFormat>(".testing"),
        FormatError
    );

    FormatFactory::get().register_name<DummyFormat>("Testing");
    // We can not register the same format twice
    CHECK_THROWS_AS(
        FormatFactory::get().register_name<DummyFormat>("Testing"),
        FormatError
    );
}

TEST_CASE("Geting registered format"){
    FormatFactory::get().register_extension<DummyFormat>(".dummy");
    FormatFactory::get().register_name<DummyFormat>("Dummy");

    DummyFormat dummy("", File::READ);
    auto format = FormatFactory::get().extension(".dummy")("", File::READ);
    CHECK(typeid(dummy) == typeid(*format));
    format = FormatFactory::get().name("Dummy")("", File::READ);
    CHECK(typeid(dummy) == typeid(*format));

    CHECK_THROWS_AS(FormatFactory::get().name("UNKOWN"), FormatError);
    CHECK_THROWS_AS(FormatFactory::get().extension(".UNKOWN"), FormatError);

    try {
        FormatFactory::get().name("Dully");
        CHECK(false);
    } catch (const FormatError& e) {
        CHECK(std::string(e.what()) == "can not find a format named 'Dully'. Did you mean 'Dummy'?");
    }

    try {
        FormatFactory::get().name("DUMMY");
        CHECK(false);
    } catch (const FormatError& e) {
        CHECK(std::string(e.what()) == "can not find a format named 'DUMMY'. Did you mean 'Dummy'?");
    }

    FormatFactory::get().register_name<DummyFormat>("Dunny");
    try {
        FormatFactory::get().name("Dully");
        CHECK(false);
    } catch (const FormatError& e) {
        CHECK(std::string(e.what()) == "can not find a format named 'Dully'. Did you mean 'Dunny' or 'Dummy'?");
    }
}

TEST_CASE("Check error throwing in formats"){
    auto tmpfile = NamedTempPath(".dummy");
    std::ofstream out(tmpfile);
    out << "hey !" << std::endl;
    out.close();

    Frame frame;
    Trajectory trajectory(tmpfile, 'a');
    CHECK_THROWS_AS(trajectory.read(), FormatError);
    CHECK_THROWS_AS(trajectory.read_step(2), FormatError);
    CHECK_THROWS_AS(trajectory.write(frame), FormatError);
}
