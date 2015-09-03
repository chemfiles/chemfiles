program cell_test
    use chemharp
    use testing

    implicit none
    integer(kind=kind(CHRP_LOG_LEVEL)) :: level
    integer :: status
    logical :: fexist
    character(len=1024) :: error

    error = chrp_strerror(int(0, 4))
    call check((trim(error) == "Operation was sucessfull"), "chrp_strerror")

    error = chrp_last_error()
    call check((trim(error) == ""), "chrp_last_error")

    call chrp_loglevel(level, status=status)
    call check((status == 0), "chrp_loglevel")
    call check((level == CHRP_LOG_WARNING), "chrp_loglevel")

    call chrp_set_loglevel(CHRP_LOG_NONE, status=status)
    call check((status == 0), "chrp_set_loglevel")

    call chrp_loglevel(level, status=status)
    call check((status == 0), "chrp_loglevel")
    call check((level == CHRP_LOG_NONE), "chrp_loglevel")

    call chrp_logfile("test.log", status=status)
    call check((status == 0), "chrp_logfile")

    inquire(file="test.log", exist=fexist)
    call check(fexist, "File created by chrp_logfile")

    call chrp_log_stderr(status=status)
    call check((status == 0), "chrp_log_stderr")

    open(unit=11, iostat=status, file="test.log", status='old')
    if (status == 0) close(11, status='delete')
end program
