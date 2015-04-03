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
    procedure :: init => chrp_cell
    procedure :: lengths => chrp_cell_lengths
    procedure :: lengths_set => chrp_cell_lengths_set
    procedure :: angles => chrp_cell_angles
    procedure :: angles_set => chrp_cell_angles_set
    procedure :: matrix => chrp_cell_matrix
    procedure :: type => chrp_cell_type
    procedure :: type_set => chrp_cell_type_set
    procedure :: periodicity => chrp_cell_periodicity
    procedure :: periodicity_set => chrp_cell_periodicity_set
    procedure :: free => chrp_cell_free
end type

type frame
    private
    type(c_ptr) :: ptr
contains
    procedure :: init => chrp_frame
    procedure :: size => chrp_frame_size
    procedure :: positions => chrp_frame_positions
    procedure :: positions_set => chrp_frame_positions_set
    procedure :: velocities => chrp_frame_velocities
    procedure :: velocities_set => chrp_frame_velocities_set
    procedure :: free => chrp_frame_free
end type

type atom
    private
    type(c_ptr) :: ptr
contains
    procedure :: init => chrp_atom
    procedure :: mass => chrp_atom_mass
    procedure :: mass_set => chrp_atom_mass_set
    procedure :: charge => chrp_atom_charge
    procedure :: charge_set => chrp_atom_charge_set
    procedure :: name => chrp_atom_name
    procedure :: name_set => chrp_atom_name_set
    procedure :: free => chrp_atom_free
end type

type topology
    private
    type(c_ptr) :: ptr
contains
    procedure :: init => chrp_topology
    procedure :: append => chrp_topology_append
    procedure :: remove => chrp_topology_remove
    procedure :: isbond => chrp_topology_isbond
    procedure :: isangle => chrp_topology_isangle
    procedure :: isdihedral => chrp_topology_isdihedral
    procedure :: bonds_count => chrp_topology_bonds_count
    procedure :: angles_count => chrp_topology_angles_count
    procedure :: dihedrals_count => chrp_topology_dihedrals_count
    procedure :: bonds => chrp_topology_bonds
    procedure :: angles => chrp_topology_angles
    procedure :: dihedrals => chrp_topology_dihedrals
    procedure :: add_bond => chrp_topology_add_bond
    procedure :: remove_bond => chrp_topology_remove_bond
    procedure :: free => chrp_topology_free
    procedure :: atom => chrp_topology_atom
end type
