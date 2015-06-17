program trajectory_read
    use iso_fortran_env, only: real32, real64, int64
    use chemharp
    use testing

    implicit none
    type(chrp_frame) :: frame
    type(chrp_trajectory) :: file
    type(chrp_topology) :: topology
    type(chrp_atom) :: atom
    type(chrp_cell) :: cell

    character(len=2048) :: DATADIR
    character(len=32) :: name
    integer :: status, i
    integer(kind=int64) :: natoms, n
    real(kind=real32) :: pos_1(3), pos_125(3), pos(3, 297)
    real(kind=real64) :: a, b, c

    ! ================================================================================== !
    if (command_argument_count() >= 1) then
        call get_command_argument(1, DATADIR)
    else
        ! Use DATADIR as a temporary to get the program name
        call get_command_argument(0, DATADIR)
        DATADIR = "Usage: " // trim(DATADIR) // " path/to/the/DATADIRs"
        write(*, *) trim(DATADIR)
        stop 1
    end if
    ! ================================================================================== !

    call file%open(trim(DATADIR) // "/xyz/water.xyz", "r", status=status)
    call check((status == 0), "file%open")

    call frame%init(0, status=status)
    call check((status == 0), "frame%init")

    ! Read the first frame
    call file%read(frame, status=status)
    call check((status == 0), "file%read")

    call frame%size(natoms, status=status)
    call check((status == 0), "frame%size")
    call check((natoms == 297), "frame%size")

    ! Check positions in the first frame
    pos_1 = [0.417219, 8.303366, 11.737172]
    pos_125 = [5.099554, -0.045104, 14.153846]
    call frame%positions(pos, natoms, status=status)
    call check((status == 0), "frame%positions")

    do i=1,3
        call check((pos_1(i) == pos(i, 1)), "frame%positions")
        call check((pos_125(i) == pos(i, 125)), "frame%positions")
    end do

    ! Check topology in the first frame
    call topology%from_frame(frame, status=status)
    call check((status == 0), "topology%from_frame")

    call topology%size(natoms, status=status)
    call check((status == 0), "topology%size")
    call check((natoms == 297), "topology%size")

    call topology%bonds_count(n, status=status)
    call check((status == 0), "topology%bonds_count")
    call check((n == 0), "topology%bonds_count")

    call atom%from_topology(topology, 0, status=status)
    call check((status == 0), "atom%from_topology")

    call atom%name(name, len(name), status=status)
    call check((status == 0), "atom%name")
    call check((name == "O"), "atom%name")

    call atom%free(status=status)
    call check((status == 0), "atom%free")
    call topology%free(status=status)
    call check((status == 0), "topology%free")

    ! Set the cell associated with a trajectory
    call cell%init(30d0, 30d0, 30d0, status=status)
    call check((status == 0), "cell%init")

    call file%set_cell(cell, status=status)
    call check((status == 0), "trajectory%set_cell")

    call cell%free(status=status)
    call check((status == 0), "cell%free")

    ! Check reading a specific step
    call file%read_step(41, frame, status=status)
    call check((status == 0), "file%read_step")

    ! Check that the cell was set
    call cell%from_frame(frame, status=status)
    call check((status == 0), "cell%from_frame")

    call cell%lengths(a, b, c, status=status)
    call check((status == 0), "cell%lengths")
    call check((a == 30d0), "cell%lengths")
    call check((b == 30d0), "cell%lengths")
    call check((c == 30d0), "cell%lengths")

    call cell%free(status=status)
    call check((status == 0), "cell%free")

    pos_1(1) = 0.761277;  pos_1(2) = 8.106125;   pos_1(3) = 10.622949;
    pos_125(1) = 5.13242; pos_125(2) = 0.079862; pos_125(3) = 14.194161;

    call frame%positions(pos, natoms, status=status)
    call check((status == 0), "frame%positions")

    do i=1,3
        call check((pos_1(i) == pos(i, 1)), "frame%positions")
        call check((pos_125(i) == pos(i, 125)), "frame%positions")
    end do

    ! Get the atom from a frame
    call atom%from_frame(frame, 1);
    call atom%name(name, len(name), status=status)
    call check((status == 0), "atom%name")
    call check((name == "H"), "atom%name")

    call atom%free(status=status)
    call check((status == 0), "atom%free")

    ! Guess the system topology
    call frame%guess_topology(logical(.true., kind=1), status=status)
    call check((status == 0), "frame%guess_topology")

    call topology%from_frame(frame, status=status)
    call check((status == 0), "topology%from_frame")

    call topology%bonds_count(n, status=status)
    call check((status == 0), "topology%bonds_count")
    call check((n == 181), "topology%bonds_count")

    call topology%angles_count(n, status=status)
    call check((status == 0), "topology%angles_count")
    call check((n == 87), "topology%angles_count")

    call topology%free(status=status)
    call check((status == 0), "topology%free")

    ! Set the topology associated with a trajectory by hand
    call topology%init()
    call atom%init("Cs")

    do i=1,3
        call topology%append(atom, status=status)
        call check((status == 0), "topology%append")
    end do

    call file%set_topology(topology, status=status)
    call check((status == 0), "file%set_topology")

    call topology%free(status=status)
    call check((status == 0), "topology%free")

    call file%read_step(10, frame, status=status)
    call check((status == 0), "file%read_step")

    call atom%from_frame(frame, 1);
    call atom%name(name, len(name), status=status)
    call check((status == 0), "atom%name")
    call check((name == "Cs"), "atom%name")

    call atom%free(status=status)
    call check((status == 0), "atom%free")

    ! Set the topology associated with a trajectory from a file
    call file%set_topology_file(trim(DATADIR) // "/xyz/topology.xyz", status=status)
    call check((status == 0), "file%set_topology")
    call file%read_step(10, frame, status=status)
    call check((status == 0), "file%read_step")

    call atom%from_frame(frame, 0);
    call atom%name(name, len(name), status=status)
    call check((status == 0), "atom%name")
    call check((name == "Zn"), "atom%name")

    call atom%free(status=status)
    call check((status == 0), "atom%free")
    call frame%free(status=status)
    call check((status == 0), "file%free")
    call file%close(status=status)
    call check((status == 0), "file%close")
end program
