program read
    use iso_fortran_env, only: real32, real64, int64
    use chemharp
    use testing
    implicit none

    real(kind=real32), dimension(3, 2) :: FIRST_FRAME, LAST_FRAME, MIDLE_FRAME
    character(len=2048) :: DATAFILE
    type(chrp_frame) :: frame
    type(chrp_trajectory) :: traj
    type(chrp_topology) :: topology
    type(chrp_cell) :: cell
    integer :: status, i
    integer(kind=int64) :: nsteps


    FIRST_FRAME = reshape([0.49053, 8.41351, 0.0777257,  &
                           8.57951, 8.65712, 8.06678   ], [3, 2])
    LAST_FRAME = reshape([-1.186037, 11.439334, 0.529939,  &
                           5.208778, 12.707273, 10.940157], [3, 2])
    MIDLE_FRAME = reshape([-0.145821, 8.540648, 1.090281,  &
                            8.446093, 8.168162, 9.350953], [3, 2])

!------------------------------------------------------------------------------!
    if (command_argument_count() >= 1) then
        call get_command_argument(1, DATAFILE)
    else
        ! Use DATADIR to get the program name
        call get_command_argument(0, DATAFILE)
        DATAFILE = "Usage: " // trim(DATAFILE) // " path/to/the/datafiles"
        write(*, *) trim(DATAFILE)
        stop 1
    end if

    DATAFILE = trim(DATAFILE) // "/xyz/helium.xyz"

!------------------------------------------------------------------------------!
    call traj%open(trim(DATAFILE), "r", status=status)
    write(*, *) status
    call check((status == 0), "trajectory%open")
    call frame%init(0)

    call traj%read(frame, status)
    call check((status == 0), "trajectory%read")
    call check_frame(frame, FIRST_FRAME)

    call topology%from_frame(frame)
    call check_topology(topology)

    call cell%from_frame(frame)
    call check_cell(cell)

    call traj%nsteps(nsteps, status)
    call check((status == 0), "trajectory%nsteps")

    do i=2,nsteps
        call traj%read(frame, status)
        call check((status == 0), "trajectory%read")
    end do

    call check_frame(frame, LAST_FRAME)

    call traj%read_at(42, frame, status)
    call check_frame(frame, MIDLE_FRAME)

contains
    subroutine check_frame(frame, reference)
        use iso_c_binding, only: c_bool
        implicit none
        type(chrp_frame), intent(in) :: frame
        real(kind=real32), dimension(3, 2), intent(in) :: reference

        logical(kind=c_bool) :: has_velocities = .true.
        integer(kind=int64) :: natoms = 0
        integer :: status, i
        real(kind=real32), dimension(:, :), allocatable :: positions

        call frame%size(natoms)
        call check((natoms == 125), "frame%size")

        call frame%has_velocities(has_velocities, status)
        call check((status == 0 .and. .not. has_velocities), "frame%has_velocities")

        allocate(positions(3, natoms))

        call frame%positions(positions, natoms, status)
        call check((status == 0), "frame%positions")

        do i=1,3
            call check((abs(positions(i, 1) - reference(i, 1)) < 1e-5), "Wrong positions")
            call check((abs(positions(i, 125) - reference(i, 2)) < 1e-5), "Wrong positions")
        end do

        deallocate(positions)
    end subroutine

    subroutine check_topology(topology)
        implicit none
        type(chrp_topology), intent(in) :: topology

        integer(kind=int64) :: natoms = 0
        type(chrp_atom) :: atom
        character(len=5) :: name

        call topology%size(natoms, status)
        call check((status == 0), "topology%size")
        call check((natoms == 125), "natoms == 125")

        call atom%from_topology(topology, 3, status)
        call check((status == 0), "atom%from_topology")
        call atom%name(name, 5, status)
        call check((status == 0), "atom%name")
        call check((name == 'He'), "name == 'He'")
    end subroutine

    subroutine check_cell(cell)
        implicit none
        type(chrp_cell), intent(in) :: cell

        real(kind=real64) :: a = 0, b = 0, c = 0, alpha = 0, beta = 0, gamma = 0
        integer :: status
        integer(kind=kind(CHRP_CELL_TYPES)) :: celltype

        call cell%lengths(a, b, c, status)
        call check((status == 0), "cell%lengths")

        call check((a == 0.0), "a == 0.0")
        call check((b == 0.0), "b == 0.0")
        call check((c == 0.0), "c == 0.0")

        call cell%angles(alpha, beta, gamma, status)
        call check((status == 0), "cell%angles")

        call check((alpha == 90.0), "alpha == 90.0")
        call check((beta == 90.0), "beta == 90.0")
        call check((gamma == 90.0), "gamma == 90.0")

        call cell%type(celltype)
        call check((status == 0), "cell%type")
        call check((celltype == CHRP_CELL_INFINITE), "celltype == INFINITE")
    end subroutine
end program
