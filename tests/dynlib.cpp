#include "catch.hpp"

#include "Dynlib.hpp"
using namespace harp;

TEST_CASE("Dynamic library loading", "[Dynlib]"){
    Dynlib lib(BINDIR "/libtest.so");

    auto foo = lib.symbol<int (*)(int)>("foo");
    CHECK(foo(3) == 13);

    auto bar = lib.symbol<double (*)(void)>("bar");
    CHECK(bar() == 42);

    Dynlib lib_empty;
    CHECK_THROWS_AS(lib_empty.symbol<double (*)(void)>("HEY"), PluginError);
}
