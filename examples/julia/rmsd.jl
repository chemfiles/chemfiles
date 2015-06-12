# File rmsd.jl, example for the Chemharp library
# Any copyright is dedicated to the Public Domain.
# http://creativecommons.org/publicdomain/zero/1.0/

#!/usr/bin/env julia
using Chemharp

traj = Trajectory("filename.nc")

distances = Float64[]

# Accumulate the distances to the origin of the 10th atom throughtout the
# trajectory
for i=1:nsteps(input_file)
    frame = read(traj)
    # Position of the 10th atom
    pos = positions(frame)[:, 10]
    dist = sqrt(dot(pos, pos))
    push!(distances, dist)
end

mean = sum(distances)/size(distances, 1)
rmsd = 0.0
for dist in distances
    rmsd += (mean - dist)*(mean - dist)
end
rmsd /= size(distances, 1)
rmsd = sqrt(rmsd)

println("Root-mean square displacement is: $rmsd")
