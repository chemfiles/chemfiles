! Chemharp, an efficient IO library for chemistry file formats
! Copyright (C) 2015 Guillaume Fraux
!
! This Source Code Form is subject to the terms of the Mozilla Public
! License, v. 2.0. If a copy of the MPL was not distributed with this
! file, You can obtain one at http://mozilla.org/MPL/2.0/
!
! =========================================================================== !
! !!!! AUTO-GENERATED FILE !!!! Do not edit. See scripts/fortran for the code.
! This file contains Fortran 2003 ISO C Binding interface to the C API
!
! This file is not compilable on his own, but should be 'include'd in another
! fortran compilation unit.
! =========================================================================== !


enum, bind(C)
    enumerator :: CHRP_LOG_NONE
    enumerator :: CHRP_LOG_ERROR
    enumerator :: CHRP_LOG_WARNING
    enumerator :: CHRP_LOG_INFO
    enumerator :: CHRP_LOG_DEBUG
    ! Enumeration name:
    enumerator :: CHRP_LOG_LEVEL
end enum

enum, bind(C)
    enumerator :: CHRP_CELL_ORTHOROMBIC
    enumerator :: CHRP_CELL_TRICLINIC
    enumerator :: CHRP_CELL_INFINITE
    ! Enumeration name:
    enumerator :: CHRP_CELL_TYPES
end enum
