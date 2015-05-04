program read
    use iso_fortran_env, only: real32, real64, int64
    use chemharp
    implicit none

    type(chrp_trajectory) :: traj
    type(chrp_topology)   :: topology
    type(chrp_atom)       :: atom
    type(chrp_frame)      :: frame

    character(len=512) :: FILENAME = "test-fortran.xyz"
    character(len=2048) :: CONTENT, file_content = ""
    character :: EOL = char(10)

    real(kind=real32), dimension(3, 4) :: positions
    integer :: status, i

    CONTENT = "4" // EOL // &
              "Written by Chemharp" // EOL // &
              "He 1 2 3" // EOL // &
              "He 4 5 6" // EOL // &
              "He 1 2 3" // EOL // &
              "He 4 5 6" // EOL

    positions = reshape([1.0, 2.0, 3.0, &
                         4.0, 5.0, 6.0, &
                         1.0, 2.0, 3.0, &
                         4.0, 5.0, 6.0], [3, 4])

!------------------------------------------------------------------------------!

    call traj%open(FILENAME, "w")
    call topology%empty()
    call atom%from_name("He")
    call frame%init(4)

    do i=1, 4
        call topology%append(atom, status)
        if (status /= 0) stop "topology%append"
    end do

    call frame%positions_set(positions, 4, status)
    if (status /= 0) stop "frame%positions_set"

    call frame%topology_set(topology, status)
    if (status /= 0) stop "frame%topology_set"

    call traj%write(frame)
    if (status /= 0) stop "traj%write"

    call traj%close()
    if (status /= 0) stop "traj%close"

    file_content = read_whole_file(FILENAME)
    if (trim(file_content) /= trim(CONTENT)) stop "Wrong file content"

    open(unit=11, iostat=status, file=FILENAME, status='old')
    if (status == 0) close(11, status='delete')
contains
    !** Read a whole file into a string
    function read_whole_file(name) result(content)
        implicit none
        character(len=*) :: name
        character(len=2048) :: content

        integer :: status
        character(len=512) :: tmp

        content = ""
        open(file=name, unit=11, iostat=status)
        do while (.not. is_iostat_end(status))
            read(11, '(a)', iostat=status, end=999) tmp
            tmp = trim(tmp) // char(10)
            content = trim(content) // trim(tmp)
        end do
999     close(11)
    end function
end program
