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
#include "chemfiles/FormatMetadata.hpp"
using namespace chemfiles;

#define new_format(_name_)                                                     \
struct _name_ final: public Format {                                           \
    _name_(const std::string&, File::Mode, File::Compression) {}               \
    _name_(std::shared_ptr<MemoryBuffer>, File::Mode, File::Compression) {}    \
    size_t nsteps() override {return 42;}                                      \
}


new_format(DummyFormat);
new_format(DunnyFormat);
new_format(NoNameFormat);
new_format(NoDotExtensionFormat);
new_format(SpacesInNameFormat);
new_format(SpacesInExtensionFormat);
new_format(SpacesInDescriptionFormat);
new_format(SpacesInReferenceFormat);
new_format(BadReferenceFormat);
new_format(NoFormatMetadata);
new_format(SameNameFormat);
new_format(SameExtensionFormat);

struct UnimplementedTextFormat final: public TextFormat {
    UnimplementedTextFormat(const std::string& path, File::Mode mode, File::Compression compression):
        TextFormat(std::move(path), mode, compression) {}

    optional<uint64_t> forward() override {
        static int pos = -1;
        if (pos < 4) {
            pos++;
            return static_cast<uint64_t>(pos);
        } else {
            return nullopt;
        }
    }
};

namespace chemfiles {
    template<> const FormatMetadata& format_metadata<DummyFormat>() {
        static FormatMetadata meta;
        meta.name = "Dummy";
        meta.extension = ".dummy";
        return meta;
    }

    template<> const FormatMetadata& format_metadata<SameNameFormat>() {
        static FormatMetadata meta;
        meta.name = "Dummy";
        meta.extension = ".ext";
        return meta;
    }

    template<> const FormatMetadata& format_metadata<SameExtensionFormat>() {
        static FormatMetadata meta;
        meta.name = "SameExtension";
        meta.extension = ".dummy";
        return meta;
    }

    template<> const FormatMetadata& format_metadata<DunnyFormat>() {
        static FormatMetadata meta;
        meta.name = "Dunny";
        return meta;
    }

    template<> const FormatMetadata& format_metadata<NoNameFormat>() {
        static FormatMetadata meta;
        return meta;
    }

    template<> const FormatMetadata& format_metadata<NoDotExtensionFormat>() {
        static FormatMetadata meta;
        meta.name = "NoDotExtensionFormat";
        meta.extension = "no-dot";
        return meta;
    }

    template<> const FormatMetadata& format_metadata<UnimplementedTextFormat>() {
        static FormatMetadata meta;
        meta.name = "UnimplementedTextFormat";
        meta.extension = ".UnimplementedTextFormat";
        return meta;
    }

    template<> const FormatMetadata& format_metadata<SpacesInNameFormat>() {
        static FormatMetadata meta;
        meta.name = "   SpacesInNameFormat";
        return meta;
    }

    template<> const FormatMetadata& format_metadata<SpacesInExtensionFormat>() {
        static FormatMetadata meta;
        meta.name = "SpacesInExtensionFormat";
        meta.extension = "  .foo";
        return meta;
    }

    template<> const FormatMetadata& format_metadata<SpacesInDescriptionFormat>() {
        static FormatMetadata meta;
        meta.name = "SpacesInDescriptionFormat";
        meta.description = "test  \t";
        return meta;
    }

    template<> const FormatMetadata& format_metadata<SpacesInReferenceFormat>() {
        static FormatMetadata meta;
        meta.name = "SpacesInReferenceFormat";
        meta.reference = "http://test  \t";
        return meta;
    }

    template<> const FormatMetadata& format_metadata<BadReferenceFormat>() {
        static FormatMetadata meta;
        meta.name = "BadReferenceFormat";
        meta.reference = "foo.bar";
        return meta;
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

    CHECK(FormatFactory::get().formats().back().get().name == std::string("Dunny"));
}

TEST_CASE("Bad format info") {
    CHECK_THROWS_WITH(
        FormatFactory::get().add_format<NoNameFormat>(),
        "the format name can not be an empty string"
    );

    CHECK_THROWS_WITH(
        FormatFactory::get().add_format<NoDotExtensionFormat>(),
        "the extension for format 'NoDotExtensionFormat' must start with a dot"
    );

    CHECK_THROWS_WITH(
        FormatFactory::get().add_format<NoFormatMetadata>(),
        "format_metadata is not implemented for this format"
    );

    CHECK_THROWS_WITH(
        FormatFactory::get().add_format<SpacesInNameFormat>(),
        "the name can not start or end with spaces for format '   SpacesInNameFormat'"
    );

    CHECK_THROWS_WITH(
        FormatFactory::get().add_format<SpacesInExtensionFormat>(),
        "the extension can not start or end with spaces for format 'SpacesInExtensionFormat'"
    );

    CHECK_THROWS_WITH(
        FormatFactory::get().add_format<SpacesInDescriptionFormat>(),
        "the description can not start or end with spaces for format 'SpacesInDescriptionFormat'"
    );

    CHECK_THROWS_WITH(
        FormatFactory::get().add_format<SpacesInReferenceFormat>(),
        "the reference can not start or end with spaces for format 'SpacesInReferenceFormat'"
    );

    CHECK_THROWS_WITH(
        FormatFactory::get().add_format<BadReferenceFormat>(),
        "the reference for format 'BadReferenceFormat' must be an http link"
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

        CHECK_THROWS_WITH(
            Trajectory::memory_writer("UnimplementedTextFormat"),
            "in-memory IO is not supported for the 'UnimplementedTextFormat' format"
        );
    }
}
