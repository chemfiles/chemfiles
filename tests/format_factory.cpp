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
};
REGISTER_FORMAT(DummyFormat, ".dummy");

TEST_CASE("Get registered format", "[format factory]"){
    auto format = FormatFactory::format(".dummy");
    auto dummy = DummyFormat();
    REQUIRE(typeid(dummy) == typeid(*format));

    auto format2 = FormatFactory::format(".xyz");
    auto XYZ = XYZFormat();
    REQUIRE(typeid(XYZ) == typeid(*format2));
}
