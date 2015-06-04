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
    size_t nsteps(File*) const {return 42;}
    REGISTER_FORMAT;
};

class DummyFormat2 : public Format {
public:
    DummyFormat2(){}
    std::string description() const {return "";}
    size_t nsteps(File*) const {return 42;}
    REGISTER_FORMAT;
};
REGISTER(DummyFormat, "Dummy");
REGISTER_EXTENSION(DummyFormat, ".dummy");

// Dummy file clase
class DummyFile : public BinaryFile {
public:
    DummyFile(const string&, const string&) : BinaryFile("") {}
    bool is_open() {return true;}
    void close() {}
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

    CHECK(TrajectoryFactory::register_format("Testing", {nullptr, nullptr}));
    // We can not register the same format twice
    CHECK_THROWS_AS(
        TrajectoryFactory::register_format("Testing", {nullptr, nullptr}),
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

    CHECK_THROWS_AS(TrajectoryFactory::format("UNKOWN"), FormatError);
    CHECK_THROWS_AS(TrajectoryFactory::by_extension(".UNKOWN"), FormatError);
}

TEST_CASE("Geting file type associated to a format", "[Trajectory factory]"){
    DummyFile dummy("", "");
    auto file = TrajectoryFactory::by_extension(".dummy").file_creator;
    CHECK(file == nullptr);

    file = TrajectoryFactory::by_extension(".dummy2").file_creator;
    CHECK(typeid(dummy) == typeid(*file("", "")));
}

TEST_CASE("Check error throwing in formats", "[Format errors]"){
    // Create a dummy file
    std::string filename("test-file.dummy");
    std::ofstream out(filename);
    out << "hey !" << std::endl;

    Frame frame;
    Trajectory traj(filename);
    CHECK_THROWS_AS(traj.read(), FormatError);
    CHECK_THROWS_AS(traj.read_step(2), FormatError);
    CHECK_THROWS_AS(traj.write(frame), FormatError);

    remove(filename.c_str());
}
