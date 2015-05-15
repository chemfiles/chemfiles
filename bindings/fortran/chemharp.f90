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
    use strings
    implicit none
    private
    ! Export types
    public :: chrp_trajectory, chrp_frame, chrp_cell, chrp_topology, chrp_atom
    ! Export enums
    public:: CHRP_LOG_LEVEL, CHRP_LOG_NONE, CHRP_LOG_ERROR, CHRP_LOG_WARNING, CHRP_LOG_INFO, CHRP_LOG_DEBUG
    public:: CHRP_CELL_TYPES, CHRP_CELL_ORTHOROMBIC, CHRP_CELL_TRICLINIC, CHRP_CELL_INFINITE
    ! Export free functions
    public :: chrp_log_stderr, chrp_logfile, chrp_loglevel, chrp_last_error, chrp_strerror

    include "generated/cenums.f90"
    include "generated/cdef.f90"
    include "generated/ftypes.f90"
contains
    include "generated/interface.f90"
end module chemharp
