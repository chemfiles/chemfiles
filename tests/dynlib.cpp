#include "catch.hpp"
#include "chemfiles/Dynlib.hpp"
using namespace chemfiles;

TEST_CASE("Dynamic library loading", "[Dynlib]"){
    Dynlib lib("libtest.so");

    auto foo = lib.symbol<int (*)(int)>("foo");
    CHECK(foo(3) == 13);

    auto bar = lib.symbol<double (*)(void)>("bar");
    CHECK(bar() == 42);

    Dynlib lib_empty;
    CHECK_THROWS_AS(lib_empty.symbol<double (*)(void)>("HEY"), PluginError);
}
