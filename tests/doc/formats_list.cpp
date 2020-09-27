#include <iostream>
#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

TEST_CASE() {
    // [example]
    auto list = chemfiles::formats_list();
    for (auto format: list) {
        auto& metadata = format.get();
        std::cout << metadata.name << " (" << metadata.extension.value_or("<no extension>") << ")" << std::endl;
    }
    // [example]
}
