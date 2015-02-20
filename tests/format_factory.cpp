#include <string>

#include "catch.hpp"

#include "Chemharp.hpp"
#include "FormatFactory.hpp"
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
REGISTER(DummyFormat, "Dummy");
REGISTER_EXTENSION(DummyFormat, ".dummy");

TEST_CASE("Registering a new format", "[format factory]"){
    CHECK(FormatFactory::register_extension(".testing",[](){
                return unique_ptr<Format>(new DummyFormat());
         })
    );
    // We can not register the same format twice
    CHECK_THROWS_AS(FormatFactory::register_extension(".testing",[](){
                return unique_ptr<Format>(new DummyFormat());
         }),
         FormatError
    );

    CHECK(FormatFactory::register_format("DummyFormat",[](){
                return unique_ptr<Format>(new DummyFormat());
         })
    );
    // We can not register the same format twice
    CHECK_THROWS_AS(FormatFactory::register_format("DummyFormat",[](){
                return unique_ptr<Format>(new DummyFormat());
         }),
         FormatError
    );
}

TEST_CASE("Geting registered format", "[format factory]"){
    auto dummy = DummyFormat();
    auto format = FormatFactory::by_extension(".dummy");
    REQUIRE(typeid(dummy) == typeid(*format));
    format = FormatFactory::format("Dummy");
    REQUIRE(typeid(dummy) == typeid(*format));

    auto XYZ = XYZFormat();
    format = FormatFactory::by_extension(".xyz");
    REQUIRE(typeid(XYZ) == typeid(*format));
    format = FormatFactory::format("XYZ");
    REQUIRE(typeid(XYZ) == typeid(*format));
}
