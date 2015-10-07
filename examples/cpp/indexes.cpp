/* File indexes.cpp, example for the chemfiles library
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/ */
#include <iostream>
#include <vector>

#include "chemfiles.hpp"

int main() {
    chemfiles::Trajectory traj("filename.xyz");
    chemfiles::Frame frame;

    traj >> frame;
    auto positions = frame.positions();
    std::vector<size_t> indexes;

    for (size_t i=0; i<frame.natoms(); i++) {
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
