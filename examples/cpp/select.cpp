/* This file is an example for the chemfiles library
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/ */
#include <chemfiles.hpp>
#include <functional>

int main() {
    auto input = chemfiles::Trajectory("input.arc");
    auto output = chemfiles::Trajectory("output.pdb", 'w');

    auto selection = chemfiles::Selection("name Zn or name N");

    for (size_t step=0; step<input.nsteps(); step++) {
        auto frame = input.read();
        auto to_remove = selection.list(frame);
        std::sort(std::begin(to_remove), std::end(to_remove), std::greater<size_t>());
        for (auto i: to_remove) {
            frame.remove(i);
        }
        output.write(frame);
    }

    return 0;
}
