# -* coding: utf-8 -*

"""
Various constants for fortran API generation
"""

# Maximum string length
STRING_LENGTH = 1024

# List of type which have to be wrapped to fortran
FTYPES = [
    "chrp_trajectory", "chrp_cell", "chrp_atom", "chrp_frame", "chrp_topology"
]

LICENCE = """! Chemharp, an efficient IO library for chemistry file formats
! Copyright (C) 2015 Guillaume Fraux
!
! This Source Code Form is subject to the terms of the Mozilla Public
! License, v. 2.0. If a copy of the MPL was not distributed with this
! file, You can obtain one at http://mozilla.org/MPL/2.0/
"""

BEGINING = LICENCE + """!
! =========================================================================== !
! !!!! AUTO-GENERATED FILE !!!! Do not edit. See scripts/fortran for the code.
! This file contains Fortran 2003 ISO C Binding interface to the C API
!
! This file is not compilable on his own, but should be 'include'd in another
! fortran compilation unit.
! =========================================================================== !

"""
