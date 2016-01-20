#ifndef WIN32

#include <string>
#include <catch.hpp>

#include "chemfiles.hpp"
#include "chemfiles/TrajectoryFactory.hpp"
#include "chemfiles/Error.hpp"
#include "chemfiles/formats/XYZ.hpp"

#include "chemfiles/Frame.hpp"
using namespace chemfiles;

// Dummy format clase
class DummyFormat : public Format {
public:
    DummyFormat(File& file) : Format(file){}
    std::string description() const override {return "";}
    size_t nsteps() const override {return 42;}
};

// Dummy file clase
class DummyFile : public BinaryFile {
public:
    DummyFile(const string&, const string&) : BinaryFile("", "") {}
    bool is_open() override {return true;}
    void sync() override {}
};
class DummyFormat2 : public Format {
public:
    DummyFormat2(File& file) : Format(file){}
    std::string description() const override {return "";}
    size_t nsteps() const override {return 42;}
    using file_t = DummyFile;
};

TEST_CASE("Registering a new format", "[Trajectory factory]"){
    TrajectoryFactory::get().register_extension(".testing", {nullptr, nullptr});
    // We can not register the same format twice
    CHECK_THROWS_AS(
        TrajectoryFactory::get().register_extension(".testing", {nullptr, nullptr}),
        FormatError
    );

    TrajectoryFactory::get().register_format("Testing", {nullptr, nullptr});
    // We can not register the same format twice
    CHECK_THROWS_AS(
        TrajectoryFactory::get().register_format("Testing", {nullptr, nullptr}),
        FormatError
    );
}

TEST_CASE("Geting registered format", "[Trajectory factory]"){
    TrajectoryFactory::get().register_extension(".dummy", {new_format<DummyFormat>, new_file<typename DummyFormat::file_t>});
    TrajectoryFactory::get().register_format("Dummy", {new_format<DummyFormat>, new_file<typename DummyFormat::file_t>});

    BasicFile file("tmp.dat", "w");

    DummyFormat dummy(file);
    auto format = TrajectoryFactory::get().by_extension(".dummy").format_creator(file);
    CHECK(typeid(dummy) == typeid(*format));
    format = TrajectoryFactory::get().format("Dummy").format_creator(file);
    CHECK(typeid(dummy) == typeid(*format));

    XYZFormat XYZ(file);
    format = TrajectoryFactory::get().by_extension(".xyz").format_creator(file);
    CHECK(typeid(XYZ) == typeid(*format));
    format = TrajectoryFactory::get().format("XYZ").format_creator(file);
    CHECK(typeid(XYZ) == typeid(*format));

    CHECK_THROWS_AS(TrajectoryFactory::get().format("UNKOWN"), FormatError);
    CHECK_THROWS_AS(TrajectoryFactory::get().by_extension(".UNKOWN"), FormatError);
}

TEST_CASE("Geting file type associated to a format", "[Trajectory factory]"){
    TrajectoryFactory::get().register_extension(".dummy2", {new_format<DummyFormat2>, new_file<typename DummyFormat2::file_t>});
    DummyFile dummy("", "");
    auto file = TrajectoryFactory::get().by_extension(".dummy2").file_creator;
    CHECK(typeid(dummy) == typeid(*file("", "")));
}

TEST_CASE("Check error throwing in formats", "[Format errors]"){
    // Create a dummy file
    std::string filename = "test-file.dummy";
    std::ofstream out(filename);
    out << "hey !" << std::endl;
    out.close();

    Frame frame;
    Trajectory traj(filename, "a");
    CHECK_THROWS_AS(traj.read(), FormatError);
    CHECK_THROWS_AS(traj.read_step(2), FormatError);
    CHECK_THROWS_AS(traj.write(frame), FormatError);

    remove(filename.c_str());
}

#endif
