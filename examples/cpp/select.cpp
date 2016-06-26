/* File select.cpp, example for the chemfiles library
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/ */
#include <iostream>
#include "chemfiles.hpp"

int main() {
    // Read a frame from a given file
    auto frame = chemfiles::Trajectory("filename.xyz").read();

    // Create a selection for all atoms with "Zn" name
    auto selection = chemfiles::Selection("name Zn");
    // Get the list of matching atoms from the frame
    auto zinc = selection.list(frame);

    std::cout << "We have " << zinc.size() << "zinc in the frame" << std::endl;
    for (auto i: zinc) {
        std::cout << i << " is a zinc" << std::endl;
    }

    // Create a selection for multiple atoms
    selection = chemfiles::Selection(
        "angles: name($1) H and name($2) O and name($3) H"
    );
    // Get the list of matching atoms in the frame
    auto water = selection.evaluate(frame);
    std::cout << "We have " << water.size() << "water molecules" << std::endl;
    for (auto match: water) {
        // 'match' contains the indexes of the matching atoms
        std::cout << match[0] << " - ";
        std::cout << match[1] << " - ";
        std::cout << match[2] << " - ";
        std::cout << " is a water molecule" << std::endl;
    }

    return 0;
}
