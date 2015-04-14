program indexes_example
    use iso_fortran_env  ! For real64
    use chemharp
    implicit none
    ! Chemharp types declaration uses the "chrp_" prefix
    type(chrp_trajectory) :: traj
    type(chrp_frame) :: frame
    type(chrp_frame) :: frame

    real(real64), dimension(:, :), allocatable :: positions
    integer, dimension(:), allocatable :: indexes
    integer :: status, i, j, natoms

    call traj%init("filename.xyz", status)
    if (status /= 0) then stop end if

    call traj%read_next_step(frame, status=status)
    if (status /= 0) then stop end if

    ! Read the number of atoms
    call frame%natoms(natoms)

    allocate(indexes(natoms), positions(3, natoms))
    call frame%positions(positions)

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
            break
        end if
        write(*,*) "  - ", indexes(i)
    end do

end program
