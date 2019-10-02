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

#define new_format(_name_)                                                     \
    struct _name_: public Format {                                             \
        _name_(const std::string&, File::Mode, File::Compression) {}           \
        size_t nsteps() override {return 42;}                                  \
    }

new_format(DummyFormat);
new_format(DunnyFormat);
new_format(NoNameFormat);
new_format(BadExtensionFormat);
new_format(NoFormatInfo);
new_format(SameNameFormat);
new_format(SameExtensionFormat);

namespace chemfiles {
    template<> FormatInfo format_information<DummyFormat>() {
        return FormatInfo("Dummy").with_extension(".dummy");
    }

    template<> FormatInfo format_information<SameNameFormat>() {
        return FormatInfo("Dummy").with_extension(".ext");
    }

    template<> FormatInfo format_information<SameExtensionFormat>() {
        return FormatInfo("SameExtension").with_extension(".dummy");
    }

    template<> FormatInfo format_information<DunnyFormat>() {
        return FormatInfo("Dunny");
    }

    template<> FormatInfo format_information<NoNameFormat>() {
        return FormatInfo("");
    }

    template<> FormatInfo format_information<BadExtensionFormat>() {
        return FormatInfo("BadExtension").with_extension("no-dot");
    }
}

TEST_CASE("Geting registered format") {
    FormatFactory::get().add_format<DummyFormat>();

    DummyFormat dummy("", File::READ, File::DEFAULT);
    auto format = FormatFactory::get().extension(".dummy")("", File::READ, File::DEFAULT);
    CHECK(typeid(dummy) == typeid(*format));
    format = FormatFactory::get().name("Dummy")("", File::READ, File::DEFAULT);
    CHECK(typeid(dummy) == typeid(*format));

    CHECK_THROWS_AS(FormatFactory::get().name("UNKOWN"), FormatError);
    CHECK_THROWS_AS(FormatFactory::get().extension(".UNKOWN"), FormatError);
}

TEST_CASE("Already registered format/extension") {
    CHECK_THROWS_AS(FormatFactory::get().add_format<SameNameFormat>(), FormatError);
    CHECK_THROWS_AS(FormatFactory::get().add_format<SameExtensionFormat>(), FormatError);
}

TEST_CASE("Format names suggestions") {
    try {
        FormatFactory::get().name("Dully");
        CHECK(false);
    } catch (const FormatError& e) {
        CHECK(std::string(e.what()) == "can not find a format named 'Dully', did you mean 'Dummy'?");
    }

    try {
        FormatFactory::get().name("DUMMY");
        CHECK(false);
    } catch (const FormatError& e) {
        CHECK(std::string(e.what()) == "can not find a format named 'DUMMY', did you mean 'Dummy'?");
    }

    FormatFactory::get().add_format<DunnyFormat>();
    try {
        FormatFactory::get().name("Dully");
        CHECK(false);
    } catch (const FormatError& e) {
        CHECK(std::string(e.what()) == "can not find a format named 'Dully', did you mean 'Dummy' or 'Dunny'?");
    }

    CHECK(FormatFactory::get().formats().back().name() == "Dunny");
}

TEST_CASE("Bad format info") {
    CHECK_THROWS_AS(format_information<NoNameFormat>(), FormatError);
    CHECK_THROWS_AS(format_information<BadExtensionFormat>(), FormatError);
    CHECK_THROWS_AS(format_information<NoFormatInfo>(), FormatError);
}

TEST_CASE("Check error throwing in formats") {
    auto tmpfile = NamedTempPath(".dummy");
    std::ofstream out(tmpfile);
    out << "hey !" << std::endl;
    out.close();

    Frame frame;
    Trajectory trajectory_r(tmpfile, 'r');
    CHECK_THROWS_AS(trajectory_r.read(), FormatError);
    CHECK_THROWS_AS(trajectory_r.read_step(2), FormatError);
    Trajectory trajectory_w(tmpfile, 'w');
    CHECK_THROWS_AS(trajectory_w.write(frame), FormatError);
}
