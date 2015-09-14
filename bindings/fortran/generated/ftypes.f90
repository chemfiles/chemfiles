! Chemharp, an efficient IO library for chemistry file formats
! Copyright (C) 2015 Guillaume Fraux
!
! This Source Code Form is subject to the terms of the Mozilla Public
! License, v. 2.0. If a copy of the MPL was not distributed with this
! file, You can obtain one at http://mozilla.org/MPL/2.0/
!
! =========================================================================== !
! !!!! AUTO-GENERATED FILE !!!! Do not edit. See scripts/generate for the code.
! This file contains Fortran 2003 ISO C Binding interface to the C API
!
! This file is not compilable on his own, but should be 'include'd in another
! fortran compilation unit.
! =========================================================================== !


type chrp_topology
    private
    type(c_ptr) :: ptr
contains
    procedure :: init => chrp_topology_init_
    procedure :: from_frame => chrp_topology_from_frame_init_
    procedure :: atoms_count => chrp_topology_atoms_count
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
end type

type chrp_trajectory
    private
    type(c_ptr) :: ptr
contains
    procedure :: open => chrp_trajectory_open_init_
    procedure :: with_format => chrp_trajectory_with_format_init_
    procedure :: read => chrp_trajectory_read
    procedure :: read_step => chrp_trajectory_read_step
    procedure :: write => chrp_trajectory_write
    procedure :: set_topology => chrp_trajectory_set_topology
    procedure :: set_topology_file => chrp_trajectory_set_topology_file
    procedure :: set_cell => chrp_trajectory_set_cell
    procedure :: nsteps => chrp_trajectory_nsteps
    procedure :: close => chrp_trajectory_close
end type

type chrp_frame
    private
    type(c_ptr) :: ptr
contains
    procedure :: init => chrp_frame_init_
    procedure :: atoms_count => chrp_frame_atoms_count
    procedure :: positions => chrp_frame_positions
    procedure :: set_positions => chrp_frame_set_positions
    procedure :: velocities => chrp_frame_velocities
    procedure :: set_velocities => chrp_frame_set_velocities
    procedure :: has_velocities => chrp_frame_has_velocities
    procedure :: set_cell => chrp_frame_set_cell
    procedure :: set_topology => chrp_frame_set_topology
    procedure :: step => chrp_frame_step
    procedure :: set_step => chrp_frame_set_step
    procedure :: guess_topology => chrp_frame_guess_topology
    procedure :: free => chrp_frame_free
end type

type chrp_atom
    private
    type(c_ptr) :: ptr
contains
    procedure :: init => chrp_atom_init_
    procedure :: from_frame => chrp_atom_from_frame_init_
    procedure :: from_topology => chrp_atom_from_topology_init_
    procedure :: mass => chrp_atom_mass
    procedure :: set_mass => chrp_atom_set_mass
    procedure :: charge => chrp_atom_charge
    procedure :: set_charge => chrp_atom_set_charge
    procedure :: name => chrp_atom_name
    procedure :: set_name => chrp_atom_set_name
    procedure :: full_name => chrp_atom_full_name
    procedure :: vdw_radius => chrp_atom_vdw_radius
    procedure :: covalent_radius => chrp_atom_covalent_radius
    procedure :: atomic_number => chrp_atom_atomic_number
    procedure :: type => chrp_atom_type
    procedure :: set_type => chrp_atom_set_type
    procedure :: free => chrp_atom_free
end type

type chrp_cell
    private
    type(c_ptr) :: ptr
contains
    procedure :: init => chrp_cell_init_
    procedure :: triclinic => chrp_cell_triclinic_init_
    procedure :: from_frame => chrp_cell_from_frame_init_
    procedure :: volume => chrp_cell_volume
    procedure :: lengths => chrp_cell_lengths
    procedure :: set_lengths => chrp_cell_set_lengths
    procedure :: angles => chrp_cell_angles
    procedure :: set_angles => chrp_cell_set_angles
    procedure :: matrix => chrp_cell_matrix
    procedure :: type => chrp_cell_type
    procedure :: set_type => chrp_cell_set_type
    procedure :: periodicity => chrp_cell_periodicity
    procedure :: set_periodicity => chrp_cell_set_periodicity
    procedure :: free => chrp_cell_free
end type
