module testing
    implicit none
    private
    public :: check
contains
    !> Check if \c condition is false, and display the \c message in case of failure
    subroutine check(condition, message)
        implicit none
        logical, intent(in) :: condition
        character(len=*), intent(in), optional :: message

        if (.not. condition) then
            if (present(message)) then
                write(*, *) message
            end if
            stop 1
        end if
    end subroutine
end module
