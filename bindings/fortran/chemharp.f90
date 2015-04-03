! Chemharp, an efficient IO library for chemistry file formats
! Copyright (C) 2015 Guillaume Fraux
!
! This Source Code Form is subject to the terms of the Mozilla Public
! License, v. 2.0. If a copy of the MPL was not distributed with this
! file, You can obtain one at http://mozilla.org/MPL/2.0/
!
!
! This is the main module file for the Fortran interface to Chemharp.
!

module chemharp
    use iso_c_binding
    implicit none

    include "cenums.f90"
    include "cdef.f90"
    include "ftypes.f90"
contains
    !include "fortran_interface.f90"

end module chemharp
