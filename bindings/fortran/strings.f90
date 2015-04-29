module strings
    use iso_c_binding
    implicit none

contains
    !** Convert a C string to a Fortran string
    function c_to_f_str(c_string) result(f_string)
        implicit none
        character(len=1, kind=c_char), dimension(*), intent(in) :: c_string
        character(len=:), allocatable :: f_string
        integer :: N, i

        i = 1
        do
           if (c_string(i) == c_null_char) exit
           i = i + 1
        end do
        N = i - 1  ! Exclude null character from Fortran string

        allocate(character(len=N) :: f_string)
        f_string = transfer(c_string(1:N), f_string)
    end function

    !** Convert a Fortran string to a C string
    function f_to_c_str(f_string) result(c_string)
        implicit none
        character(len=*), intent(in) :: f_string
        character(len=1, kind=c_char) :: c_string(len_trim(f_string)+1)
        integer :: N, i

        N = len_trim(f_string)
        do i = 1, N
            c_string(i) = f_string(i:i)
        end do
        c_string(n + 1) = c_null_char
    end function
end module
