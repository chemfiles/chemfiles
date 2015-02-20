#include <string>

#include "catch.hpp"

#include "Harp.hpp"
#include "FormatFactory.hpp"
#include "Frame.hpp"
#include "formats/XYZ.hpp"

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

TEST_CASE("Get registered format", "[format factory]"){
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
