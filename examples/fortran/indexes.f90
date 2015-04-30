program indexes_example
    use iso_fortran_env, only: real32, int64
    use chemharp
    implicit none
    ! Chemharp types declaration uses the "chrp_" prefix
    type(chrp_trajectory) :: traj
    type(chrp_frame) :: frame

    real(real32), dimension(:, :), allocatable :: positions
    integer, dimension(:), allocatable :: indexes
    integer :: i, j, status
    integer(int64) :: natoms

    call traj%open("filename.xyz", "r")

    call traj%read(frame, status=status)
    if (status /= 0) then
        stop "Error"
    end if

    ! Read the number of atoms
    call frame%size(natoms)

    allocate(indexes(natoms), positions(3, natoms))
    call frame%positions(positions, natoms)

    indexes = -1
    j = 1
    do i=1,natoms
        if (positions(1, i) < 5) then
            indexes(j) = i
            j = j + 1
        end if
    end do

    write(*,*) "Atoms with x < 5: "
    do i=1,natoms
        if (indexes(i) == -1) then
            exit
        end if
        write(*,*) "  - ", indexes(i)
    end do

end program
