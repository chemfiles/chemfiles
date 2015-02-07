#include <string>

#include "catch.hpp"

#include "Harp.hpp"
#include "FormatFactory.hpp"
#include "Frame.hpp"
#include "formats/XYZ.hpp"

using namespace harp;

// Dummy clases
/******************************************************************************/
class DummyReader : public FormatReader {
public:
    Frame& read_at_step(File& file, const int step){
        static Frame frame=Frame();
        return frame;
    }
    Frame& read_next_step(File& file){
        static Frame frame=Frame();
        return frame;
    }
private:
    READER_REGISTER_MEMBER(DummyReader);
};
REGISTER_READER(DummyReader, ".dummy_reader");

class DummyWriter : public FormatWriter {
public:
    void write_step(const File& file, const Frame& frame){
        // doing nothing
    }
private:
    WRITER_REGISTER_MEMBER(DummyWriter);
};
REGISTER_WRITER(DummyWriter, ".dummy_writer");
/******************************************************************************/

TEST_CASE("Get registered format", "[format factory]"){
    auto reader = FormatFactory::get_reader(".dummy_reader");
    auto dummy_reader = DummyReader();
    REQUIRE(typeid(dummy_reader) == typeid(*reader));

    auto writer = FormatFactory::get_writer(".dummy_writer");
    auto dummy_writer = DummyWriter();
    REQUIRE(typeid(dummy_writer) == typeid(*writer));

    auto reader_2 = FormatFactory::get_reader(".xyz");
    auto XYZ_reader = XYZReader();
    REQUIRE(typeid(XYZ_reader) == typeid(*reader_2));
}
