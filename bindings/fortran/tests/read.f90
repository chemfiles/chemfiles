program read
    use iso_fortran_env, only: real32, real64, int64
    use chemharp
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
    call traj%open(trim(DATAFILE), "r")
    call frame%init(0)

    call traj%read(frame, status)
    if (status /= 0) stop "trajectory%read"
    call check_frame(frame, FIRST_FRAME)

    call topology%from_frame(frame)
    call check_topology(topology)

    call cell%from_frame(frame)
    call check_cell(cell)

    call traj%nsteps(nsteps, status)
    if (status /= 0) stop "trajectory%nsteps"

    do i=2,nsteps
        call traj%read(frame, status)
        if (status /= 0) stop "trajectory%read"
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
        if (natoms /= 125) stop "frame%size"

        call frame%has_velocities(has_velocities, status)
        if (status /= 0 .or. has_velocities) stop "frame%has_velocities"

        allocate(positions(3, natoms))

        call frame%positions(positions, natoms, status)
        if (status /= 0) stop "frame%positions"

        do i=1,3
            if (abs(positions(i, 1) - reference(i, 1)) > 1e-5) stop "Wrong positions"
            if (abs(positions(i, 125) - reference(i, 2)) > 1e-5) stop "Wrong positions"
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
        if (status /= 0) stop "topology%size"

        if (natoms /= 125) stop "natoms /= 125"

        call atom%from_topology(topology, 3)
        call atom%name(name, 5)
        if (name /= 'He') stop "name /= He"
    end subroutine

    subroutine check_cell(cell)
        implicit none
        type(chrp_cell), intent(in) :: cell

        real(kind=real64) :: a = 0, b = 0, c = 0, alpha = 0, beta = 0, gamma = 0
        integer :: status
        integer(kind=kind(cell_type)) :: celltype

        call cell%lengths(a, b, c, status)
        if (status /= 0) stop "cell%lengths"

        if (a /= 0.0) stop "a /= 0.0"
        if (b /= 0.0) stop "b /= 0.0"
        if (c /= 0.0) stop "c /= 0.0"

        call cell%angles(alpha, beta, gamma, status)
        if (status /= 0) stop "cell%angles"

        if (alpha /= 90.0) stop "alpha /= 0.0"
        if (beta /= 90.0) stop "beta /= 0.0"
        if (gamma /= 90.0) stop "gamma /= 0.0"

        call cell%type(celltype)
        if (status /= 0) stop "cell%type"
        if (celltype /= INFINITE) stop "celltype /= INFINITE"
    end subroutine
end program
