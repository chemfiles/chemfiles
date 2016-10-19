#ifdef __clang__
#pragma clang diagnostic ignored "-Wpotentially-evaluated-expression"
#endif

#include <string>
#include <catch.hpp>

#include "chemfiles.hpp"
#include "chemfiles/FormatFactory.hpp"
#include "chemfiles/Error.hpp"
#include "chemfiles/formats/XYZ.hpp"

#include "chemfiles/Frame.hpp"
using namespace chemfiles;

// Dummy format clase
class DummyFormat : public Format {
public:
    DummyFormat(const std::string&, File::Mode){}
    std::string description() const override {return "";}
    size_t nsteps() override {return 42;}
};

TEST_CASE("Registering a new format", "[Trajectory factory]"){
    FormatFactory::get().register_extension(".testing", nullptr);
    // We can not register the same format twice
    CHECK_THROWS_AS(
        FormatFactory::get().register_extension(".testing", nullptr),
        FormatError
    );

    FormatFactory::get().register_format("Testing", nullptr);
    // We can not register the same format twice
    CHECK_THROWS_AS(
        FormatFactory::get().register_format("Testing", nullptr),
        FormatError
    );
}

TEST_CASE("Geting registered format", "[Trajectory factory]"){
    FormatFactory::get().register_extension(".dummy", new_format<DummyFormat>);
    FormatFactory::get().register_format("Dummy", new_format<DummyFormat>);

    DummyFormat dummy("", File::READ);
    auto format = FormatFactory::get().by_extension(".dummy")("", File::READ);
    CHECK(typeid(dummy) == typeid(*format));
    format = FormatFactory::get().format("Dummy")("", File::READ);
    CHECK(typeid(dummy) == typeid(*format));

    {
        // create a file on the filesystem
        std::ofstream file("tmp.dat");
    }


    XYZFormat XYZ("tmp.dat", File::READ);
    format = FormatFactory::get().by_extension(".xyz")("tmp.dat", File::READ);
    CHECK(typeid(XYZ) == typeid(*format));
    format = FormatFactory::get().format("XYZ")("tmp.dat", File::READ);
    CHECK(typeid(XYZ) == typeid(*format));

    remove("tmp.dat");

    CHECK_THROWS_AS(FormatFactory::get().format("UNKOWN"), FormatError);
    CHECK_THROWS_AS(FormatFactory::get().by_extension(".UNKOWN"), FormatError);
}

TEST_CASE("Check error throwing in formats", "[Format errors]"){
    // Create a dummy file
    std::string filename = "test-file.dummy";
    std::ofstream out(filename);
    out << "hey !" << std::endl;
    out.close();

    Frame frame;
    Trajectory traj(filename, 'a');
    CHECK_THROWS_AS(traj.read(), FormatError);
    CHECK_THROWS_AS(traj.read_step(2), FormatError);
    CHECK_THROWS_AS(traj.write(frame), FormatError);

    remove(filename.c_str());
}
