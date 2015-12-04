/* File rmsd.cpp, example for the chemfiles library
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/ */
#include <iostream>
#include <vector>
#include <numeric>
#include <cmath>

#include "chemfiles.hpp"

int main() {
    chemfiles::Trajectory traj("filename.nc");
    chemfiles::Frame frame;

    std::vector<double> distances;

    // Accumulate the distances to the origin of the 10th atom throughtout the
    // trajectory
    while (!traj.done()) {
        traj >> frame;
        // Position of the 10th atom
        auto pos = frame.positions()[9];
        double distance = sqrt(pos[0]*pos[0] + pos[1]*pos[1] + pos[2]*pos[2]);
        distances.push_back(distance);
    }

    double mean = std::accumulate(std::begin(distances), std::end(distances), 0.0) / static_cast<double>(distances.size());
    double rmsd = 0.0;
    for (auto dist : distances) {
        rmsd += (mean - dist)*(mean - dist);
    }
    rmsd /= static_cast<double>(distances.size());
    rmsd = sqrt(rmsd);

    std::cout << "Root-mean square displacement is: " << rmsd << std::endl;
    return 0;
}
