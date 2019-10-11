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
    struct _name_ final: public Format {                                       \
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

struct UnimplementedTextFormat final: public TextFormat {
    UnimplementedTextFormat(const std::string& path, File::Mode mode, File::Compression compression):
        TextFormat(std::move(path), mode, compression) {}

    int64_t forward() override {
        static int64_t pos = -1;
        if (pos < 4) {
            pos++;
            return pos;
        } else {
            return -1;
        }
    }
};

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

    template<> FormatInfo format_information<UnimplementedTextFormat>() {
        return FormatInfo("UnimplementedTextFormat").with_extension(".UnimplementedTextFormat");
    }
}

TEST_CASE("Geting registered format") {
    FormatFactory::get().add_format<DummyFormat>();

    DummyFormat dummy("", File::READ, File::DEFAULT);
    auto format = FormatFactory::get().extension(".dummy")("", File::READ, File::DEFAULT);
    CHECK(typeid(dummy) == typeid(*format));
    format = FormatFactory::get().name("Dummy")("", File::READ, File::DEFAULT);
    CHECK(typeid(dummy) == typeid(*format));

    CHECK_THROWS_WITH(
        FormatFactory::get().name("UNKOWN"),
        "can not find a format named 'UNKOWN'"
    );
    CHECK_THROWS_WITH(
        FormatFactory::get().extension(".UNKOWN"),
        "can not find a format associated with the '.UNKOWN' extension"
    );
}

TEST_CASE("Already registered format/extension") {
    CHECK_THROWS_WITH(
        FormatFactory::get().add_format<SameNameFormat>(),
        "there is already a format associated with the name 'Dummy'"
    );
    CHECK_THROWS_WITH(
        FormatFactory::get().add_format<SameExtensionFormat>(),
        "the extension '.dummy' is already associated with format 'Dummy'"
    );
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
    CHECK_THROWS_WITH(
        format_information<NoNameFormat>(),
        "a format name can not be an empty string"
    );

    CHECK_THROWS_WITH(
        format_information<BadExtensionFormat>(),
        "a format extension must start with a dot"
    );

    CHECK_THROWS_WITH(
        format_information<NoFormatInfo>(),
        "format_informations is unimplemented for this format"
    );
}

TEST_CASE("Check error throwing in formats") {
    SECTION("Format") {
        auto tmpfile = NamedTempPath(".dummy");
        std::ofstream out(tmpfile);
        out << "hey !" << std::endl;
        out.close();

        auto trajectory = Trajectory(tmpfile, 'r');
        CHECK_THROWS_WITH(
            trajectory.read(),
            Catch::StartsWith("'read' is not implemented for this format")
        );
        CHECK_THROWS_WITH(
            trajectory.read_step(2),
            Catch::StartsWith("'read_step' is not implemented for this format")
        );

        auto frame = Frame();
        trajectory = Trajectory (tmpfile, 'w');
        CHECK_THROWS_WITH(
            trajectory.write(frame),
            Catch::StartsWith("'write' is not implemented for this format")
        );
    }

    SECTION("Text Format") {
        FormatFactory::get().add_format<UnimplementedTextFormat>();

        auto tmpfile = NamedTempPath(".UnimplementedTextFormat");
        std::ofstream out(tmpfile);
        out << "hey !" << std::endl;
        out.close();

        auto trajectory = Trajectory(tmpfile, 'r');
        CHECK_THROWS_WITH(
            trajectory.read(),
            Catch::StartsWith("'read' is not implemented for this format")
        );
        CHECK_THROWS_WITH(
            trajectory.read_step(2),
            Catch::StartsWith("'read' is not implemented for this format")
        );

        auto frame = Frame();
        trajectory = Trajectory (tmpfile, 'w');
        CHECK_THROWS_WITH(
            trajectory.write(frame),
            Catch::StartsWith("'write' is not implemented for this format")
        );
    }

}
