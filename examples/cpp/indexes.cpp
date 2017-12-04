/* This file is an example for the chemfiles library
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/ */
#include <chemfiles.hpp>
#include <iostream>
#include <vector>

int main() {
    chemfiles::Trajectory file("filename.xyz");
    chemfiles::Frame frame = file.read();

    std::vector<size_t> less_than_five;
    auto positions = frame.positions();

    for (size_t i=0; i<frame.size(); i++) {
        if (positions[i][0] < 5) {
            less_than_five.push_back(i);
        }
    }

    std::cout << "Atoms with x < 5: " << std::endl;
    for (auto i : less_than_five) {
        std::cout << "  - " << i << std::endl;
    }

    return 0;
}
