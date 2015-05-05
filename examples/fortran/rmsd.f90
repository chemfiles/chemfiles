!* File rmsd.f90, example for the Chemharp library
!* Any copyright is dedicated to the Public Domain.
!* http://creativecommons.org/publicdomain/zero/1.0/
program rmsd_
    use iso_fortran_env, only: real32, int64, real64
    use chemharp
    implicit none

    type(chrp_trajectory) :: traj
    type(chrp_frame)      :: frame
    real(kind=real32), dimension(:, :), allocatable :: positions
    real(kind=real64), dimension(:),    allocatable :: distances
    integer(kind=int64) :: nsteps = 0, natoms=0, i, status
    real(kind=real64) :: distance = 0, mean = 0, rmsd = 0

    call traj%open("filename.nc", "r")
    call frame%init(0)

    call traj%nsteps(nsteps)
    allocate(distances(nsteps))

    ! Accumulate the distances to the origin of the 10th atom throughtout the
    ! trajectory
    do i=1,nsteps
        call traj%read(frame, status)
        if (status /= 0) stop "Error while reading the frame"

        ! Only allocate on the first iteration. This assume a constant number
        ! of particles
        if (i == 0) then
            call frame%size(natoms)
            allocate(positions(3, natoms))
        end if

        ! Position of the 10th atom
        call frame%positions(positions, natoms, status)
        if (status /= 0) stop "Error while getting the positions"
        distance = sqrt(positions(1, 10)*positions(1, 10) + &
                        positions(2, 10)*positions(2, 10) + &
                        positions(3, 10)*positions(3, 10))
        distances(i) = distance
    end do

    do i=1,nsteps
        mean = mean + distances(i)
    end do
    mean = mean / nsteps

    do i=1,nsteps
        rmsd = rmsd + (mean - distances(i))*(mean - distances(i));
    end do
    rmsd = rmsd / nsteps;
    rmsd = sqrt(rmsd);

    write(*, *) "Root-mean square displacement is: ", rmsd

    ! Free the memory
    call traj%close()
    call frame%free()
    deallocate(distances, positions)
end program
