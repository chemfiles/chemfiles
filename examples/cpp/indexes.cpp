/* File indexes.cpp, example for the chemfiles library
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/ */
#include <iostream>
#include <vector>

#include "chemfiles.hpp"

int main() {
    chemfiles::Trajectory file("filename.xyz");

    auto frame = file.read();
    auto positions = frame.positions();
    std::vector<size_t> indexes;

    for (size_t i=0; i<frame.size(); i++) {
        if (positions[i][0] < 5) {
            indexes.push_back(i);
        }
    }

    std::cout << "Atoms with x < 5: " << std::endl;
    for (auto i : indexes) {
        std::cout << "  - " << i << std::endl;
    }

    return 0;
}
