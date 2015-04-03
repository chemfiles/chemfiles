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


type trajectory
    private
    type(c_ptr) :: ptr
contains
    procedure :: open => chrp_open
    procedure :: read_step => chrp_read_step
    procedure :: read_next_step => chrp_read_next_step
    procedure :: write_step => chrp_write_step
    procedure :: close => chrp_close
end type

type cell
    private
    type(c_ptr) :: ptr
contains
    procedure :: lengths => chrp_lengths
    procedure :: lengths_set => chrp_lengths_set
    procedure :: angles => chrp_angles
    procedure :: angles_set => chrp_angles_set
    procedure :: matrix => chrp_matrix
    procedure :: type => chrp_type
    procedure :: type_set => chrp_type_set
    procedure :: periodicity => chrp_periodicity
    procedure :: periodicity_set => chrp_periodicity_set
    procedure :: free => chrp_free
end type

type frame
    private
    type(c_ptr) :: ptr
contains
    procedure :: size => chrp_size
    procedure :: positions => chrp_positions
    procedure :: positions_set => chrp_positions_set
    procedure :: velocities => chrp_velocities
    procedure :: velocities_set => chrp_velocities_set
    procedure :: free => chrp_free
end type

type atom
    private
    type(c_ptr) :: ptr
contains
    procedure :: mass => chrp_mass
    procedure :: mass_set => chrp_mass_set
    procedure :: charge => chrp_charge
    procedure :: charge_set => chrp_charge_set
    procedure :: name => chrp_name
    procedure :: name_set => chrp_name_set
    procedure :: free => chrp_free
end type

type topology
    private
    type(c_ptr) :: ptr
contains
    procedure :: append => chrp_append
    procedure :: remove => chrp_remove
    procedure :: isbond => chrp_isbond
    procedure :: isangle => chrp_isangle
    procedure :: isdihedral => chrp_isdihedral
    procedure :: bonds_count => chrp_bonds_count
    procedure :: angles_count => chrp_angles_count
    procedure :: dihedrals_count => chrp_dihedrals_count
    procedure :: bonds => chrp_bonds
    procedure :: angles => chrp_angles
    procedure :: dihedrals => chrp_dihedrals
    procedure :: add_bond => chrp_add_bond
    procedure :: remove_bond => chrp_remove_bond
    procedure :: free => chrp_free
    procedure :: atom => chrp_atom
end type
