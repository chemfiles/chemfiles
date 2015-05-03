module strings
    use iso_c_binding
    implicit none

contains
    !** Convert a C string to a Fortran string
    function c_to_f_str(c_string) result(f_string)
        implicit none
        type(c_ptr), target, intent(in) :: c_string
        character(len=1024), pointer    :: tmp
        character(len=1024)             :: f_string
        integer                         :: str_len, i

        call c_f_pointer(c_loc(c_string), tmp)
        f_string = rm_c_null_in_str(tmp)
    end function

    !** Convert a C_NULL terminated-Fortran string to a Fortran string
    function rm_c_null_in_str(c_string) result(f_string)
        implicit none
        character(len=*), intent(in)      :: c_string
        character(len=len_trim(c_string)) :: f_string
        integer                           :: str_len, i

        str_len = index(c_string, c_null_char) - 1
        do i=1, str_len
            f_string(i:i) = c_string(i:i)
        end do
        do i=str_len+1, len_trim(c_string)
            f_string(i:i) = " "
        end do
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
