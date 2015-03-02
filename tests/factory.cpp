#include <string>

#include "catch.hpp"

#include "Chemharp.hpp"
#include "TrajectoryFactory.hpp"
#include "Error.hpp"
#include "formats/XYZ.hpp"

#include "Frame.hpp"
using namespace harp;

// Dummy format clase
class DummyFormat : public Format {
public:
    DummyFormat(){}
    std::string description() const {return "";}
    REGISTER_FORMAT;
};

class DummyFormat2 : public Format {
public:
    DummyFormat2(){}
    std::string description() const {return "";}
    REGISTER_FORMAT;
};
REGISTER(DummyFormat, "Dummy");
REGISTER_EXTENSION(DummyFormat, ".dummy");

// Dummy file clase
class DummyFile : public BinaryFile {
public:
    DummyFile(const string&, const string&) : BinaryFile("") {}
    bool is_open(void) {return true;};
    void close(void) {};
};

REGISTER_WITH_FILE(DummyFormat2, "Dummy2", DummyFile);
REGISTER_EXTENSION_AND_FILE(DummyFormat2, ".dummy2", DummyFile);

TEST_CASE("Registering a new format", "[Trajectory factory]"){
    CHECK(TrajectoryFactory::register_extension(".testing", {nullptr, nullptr}));
    // We can not register the same format twice
    CHECK_THROWS_AS(
        TrajectoryFactory::register_extension(".testing", {nullptr, nullptr}),
        FormatError
    );

    CHECK(TrajectoryFactory::register_extension("Testing", {nullptr, nullptr}));
    // We can not register the same format twice
    CHECK_THROWS_AS(
        TrajectoryFactory::register_extension("Testing", {nullptr, nullptr}),
        FormatError
    );
}

TEST_CASE("Geting registered format", "[Trajectory factory]"){
    auto dummy = DummyFormat();
    auto format = TrajectoryFactory::by_extension(".dummy").format_creator();
    CHECK(typeid(dummy) == typeid(*format));
    format = TrajectoryFactory::format("Dummy").format_creator();
    CHECK(typeid(dummy) == typeid(*format));

    auto XYZ = XYZFormat();
    format = TrajectoryFactory::by_extension(".xyz").format_creator();
    CHECK(typeid(XYZ) == typeid(*format));
    format = TrajectoryFactory::format("XYZ").format_creator();
    CHECK(typeid(XYZ) == typeid(*format));
}

TEST_CASE("Geting file type associated to a format", "[Trajectory factory]"){
    DummyFile dummy("", "");
    auto file = TrajectoryFactory::by_extension(".dummy").file_creator;
    CHECK(file == nullptr);

    file = TrajectoryFactory::by_extension(".dummy2").file_creator;
    CHECK(typeid(dummy) == typeid(*file("", "")));
}
