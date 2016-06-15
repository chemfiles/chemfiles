#define CATCH_CONFIG_RUNNER
#include "catch.hpp"
#include "crashs.h"

int main(int argc, char* argv[]) {
    silent_crash_handlers();
    return Catch::Session().run(argc, argv);
}
