program topology_test
    use iso_fortran_env, only: real32, real64, int64
    use chemharp
    use testing

    implicit none
    type(chrp_topology) :: topology
    type(chrp_atom) :: O, H
    integer :: status, i, j
    integer(kind=int64) :: natoms=100, n=100
    integer(kind=int64), dimension(2, 3) :: bonds, top_bonds
    integer(kind=int64), dimension(3, 2) :: angles, top_angles
    integer(kind=int64), dimension(4, 1) :: dihedrals, top_dihedrals
    logical(1) :: res

    call topology%init(status=status)
    call check((status == 0), "topology%init")

    call topology%size(natoms, status=status)
    call check((status == 0), "topology%natoms")
    call check((natoms == 0), "topology%natoms")

    ! Creating some H2O2
    call O%init("O")
    call H%init("H")

    call topology%append(H, status=status)
    call check((status == 0), "topology%append")
    call topology%append(O, status=status)
    call check((status == 0), "topology%append")
    call topology%append(O, status=status)
    call check((status == 0), "topology%append")
    call topology%append(H, status=status)
    call check((status == 0), "topology%append")

    call topology%size(natoms, status=status)
    call check((status == 0), "topology%size")
    call check((natoms == 4), "topology%size")

    call O%free()
    call H%free()

    call topology%bonds_count(n, status=status)
    call check((status == 0), "topology%bonds_count")
    call check((n == 0), "topology%bonds_count")
    call topology%angles_count(n, status=status)
    call check((status == 0), "topology%angles_count")
    call check((n == 0), "topology%angles_count")
    call topology%dihedrals_count(n, status=status)
    call check((status == 0), "topology%dihedrals_count")
    call check((n == 0), "topology%dihedrals_count")


    call topology%add_bond(0, 1, status=status)
    call check((status == 0), "topology%add_bond")
    call topology%add_bond(1, 2, status=status)
    call check((status == 0), "topology%add_bond")
    call topology%add_bond(2, 3, status=status)
    call check((status == 0), "topology%add_bond")

    call topology%bonds_count(n, status=status)
    call check((status == 0), "topology%bonds_count")
    call check((n == 3), "topology%bonds_count")
    call topology%angles_count(n, status=status)
    call check((status == 0), "topology%angles_count")
    call check((n == 2), "topology%angles_count")
    call topology%dihedrals_count(n, status=status)
    call check((status == 0), "topology%dihedrals_count")
    call check((n == 1), "topology%dihedrals_count")

    call topology%isbond(0, 1, res, status=status)
    call check((status == 0), "topology%isbond")
    call check((res .eqv. .true.), "topology%isbond")
    call topology%isbond(0, 3, res, status=status)
    call check((status == 0), "topology%isbond")
    call check((res .eqv. .false.), "topology%isbond")

    call topology%isbond(0, 1, res, status=status)
    call check((status == 0), "topology%isbond")
    call check((res .eqv. .true.), "topology%isbond")
    call topology%isbond(0, 3, res, status=status)
    call check((status == 0), "topology%isbond")
    call check((res .eqv. .false.), "topology%isbond")

    call topology%isangle(0, 1, 2, res, status=status)
    call check((status == 0), "topology%isangle")
    call check((res .eqv. .true.), "topology%isangle")
    call topology%isangle(0, 1, 3, res, status=status)
    call check((status == 0), "topology%isangle")
    call check((res .eqv. .false.), "topology%isangle")

    call topology%isdihedral(0, 1, 2, 3, res, status=status)
    call check((status == 0), "topology%isdihedral")
    call check((res .eqv. .true.), "topology%isdihedral")
    call topology%isdihedral(0, 1, 3, 2, res, status=status)
    call check((status == 0), "topology%isdihedral")
    call check((res .eqv. .false.), "topology%isdihedral")


    top_bonds = reshape([2, 3, 1, 2, 0, 1], [2, 3])
    call topology%bonds(bonds, 3, status=status)
    call check((status == 0), "topology%bonds")
    do i=1,2
        do j=1,3
            call check((bonds(i, j) == top_bonds(i, j)), "topology%bonds")
        end do
    end do

    top_angles = reshape([0, 1, 2, 1, 2, 3], [3, 2])
    call topology%angles(angles, 2, status=status)
    call check((status == 0), "topology%angles")
    do i=1,3
        do j=1,2
            call check((angles(i, j) == top_angles(i, j)), "topology%angles")
        end do
    end do

    top_dihedrals = reshape([0, 1, 2, 3], [4, 1])
    call topology%dihedrals(dihedrals, 1, status=status)
    call check((status == 0), "topology%dihedrals")
    do i=1,4
        call check((dihedrals(i, 1) == top_dihedrals(i, 1)), "topology%dihedrals")
    end do

    call topology%remove_bond(2, 3, status=status)
    call check((status == 0), "topology%remove_bond")
    call topology%bonds_count(n, status=status)
    call check((status == 0), "topology%bonds_count")
    call check((n == 2), "topology%bonds_count")
    call topology%angles_count(n, status=status)
    call check((status == 0), "topology%angles_count")
    call check((n == 1), "topology%angles_count")
    call topology%dihedrals_count(n, status=status)
    call check((status == 0), "topology%dihedrals_count")
    call check((n == 0), "topology%dihedrals_count")

    call topology%remove(3, status=status)
    call topology%size(natoms, status=status)
    call check((status == 0), "topology%size")
    call check((natoms == 3), "topology%size")

    call topology%free(status=status)
    call check((status == 0), "topology%free")
end program
