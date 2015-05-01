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

interface
! Function "chrp_strerror", at bindings/c/chemharp.h:60
function chrp_strerror_c(status) bind(C, name="chrp_strerror")
    use iso_c_binding
    implicit none
    type(c_ptr) :: chrp_strerror_c
    integer(kind=c_int), value :: status
end function

! Function "chrp_last_error", at bindings/c/chemharp.h:66
function chrp_last_error_c() bind(C, name="chrp_last_error")
    use iso_c_binding
    implicit none
    type(c_ptr) :: chrp_last_error_c

end function

! Function "chrp_loglevel", at bindings/c/chemharp.h:86
function chrp_loglevel_c(level) bind(C, name="chrp_loglevel")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_loglevel_c
    include "generated/cenums.f90"
    integer(kind(log_level)), value :: level
end function

! Function "chrp_logfile", at bindings/c/chemharp.h:92
function chrp_logfile_c(file) bind(C, name="chrp_logfile")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_logfile_c
    character(len=1, kind=c_char), dimension(*), intent(in) :: file
end function

! Function "chrp_log_stderr", at bindings/c/chemharp.h:98
function chrp_log_stderr_c() bind(C, name="chrp_log_stderr")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_log_stderr_c

end function

! Function "chrp_open", at bindings/c/chemharp.h:107
function chrp_open_c(filename, mode) bind(C, name="chrp_open")
    use iso_c_binding
    implicit none
    type(c_ptr) :: chrp_open_c
    character(len=1, kind=c_char), dimension(*), intent(in) :: filename
    character(len=1, kind=c_char), dimension(*), intent(in) :: mode
end function

! Function "chrp_trajectory_read_at", at bindings/c/chemharp.h:116
function chrp_trajectory_read_at_c(file, step, frame) bind(C, name="chrp_trajectory_read_at")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_trajectory_read_at_c
    type(c_ptr), value :: file
    integer(kind=c_size_t), value :: step
    type(c_ptr), value :: frame
end function

! Function "chrp_trajectory_read", at bindings/c/chemharp.h:124
function chrp_trajectory_read_c(file, frame) bind(C, name="chrp_trajectory_read")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_trajectory_read_c
    type(c_ptr), value :: file
    type(c_ptr), value :: frame
end function

! Function "chrp_trajectory_write", at bindings/c/chemharp.h:132
function chrp_trajectory_write_c(file, frame) bind(C, name="chrp_trajectory_write")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_trajectory_write_c
    type(c_ptr), value :: file
    type(c_ptr), value, intent(in) :: frame
end function

! Function "chrp_trajectory_topology", at bindings/c/chemharp.h:142
function chrp_trajectory_topology_c(file, topology) bind(C, name="chrp_trajectory_topology")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_trajectory_topology_c
    type(c_ptr), value :: file
    type(c_ptr), value :: topology
end function

! Function "chrp_trajectory_topology_file", at bindings/c/chemharp.h:151
function chrp_trajectory_topology_file_c(file, filename) bind(C, name="chrp_trajectory_topology_file")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_trajectory_topology_file_c
    type(c_ptr), value :: file
    character(len=1, kind=c_char), dimension(*), intent(in) :: filename
end function

! Function "chrp_trajectory_nsteps", at bindings/c/chemharp.h:159
function chrp_trajectory_nsteps_c(file, nsteps) bind(C, name="chrp_trajectory_nsteps")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_trajectory_nsteps_c
    type(c_ptr), value :: file
    integer(kind=c_size_t) :: nsteps
end function

! Function "chrp_trajectory_close", at bindings/c/chemharp.h:166
function chrp_trajectory_close_c(file) bind(C, name="chrp_trajectory_close")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_trajectory_close_c
    type(c_ptr), value :: file
end function

! Function "chrp_frame", at bindings/c/chemharp.h:175
function chrp_frame_c(natoms) bind(C, name="chrp_frame")
    use iso_c_binding
    implicit none
    type(c_ptr) :: chrp_frame_c
    integer(kind=c_size_t), value :: natoms
end function

! Function "chrp_frame_size", at bindings/c/chemharp.h:183
function chrp_frame_size_c(frame, natoms) bind(C, name="chrp_frame_size")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_frame_size_c
    type(c_ptr), value, intent(in) :: frame
    integer(kind=c_size_t) :: natoms
end function

! Function "chrp_frame_positions", at bindings/c/chemharp.h:192
function chrp_frame_positions_c(frame, data, size) bind(C, name="chrp_frame_positions")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_frame_positions_c
    type(c_ptr), value, intent(in) :: frame
    real(kind=c_float), dimension(:, :) :: data
    integer(kind=c_size_t), value :: size
end function

! Function "chrp_frame_positions_set", at bindings/c/chemharp.h:201
function chrp_frame_positions_set_c(frame, data, size) bind(C, name="chrp_frame_positions_set")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_frame_positions_set_c
    type(c_ptr), value :: frame
    real(kind=c_float), dimension(:, :) :: data
    integer(kind=c_size_t), value :: size
end function

! Function "chrp_frame_velocities", at bindings/c/chemharp.h:210
function chrp_frame_velocities_c(frame, data, size) bind(C, name="chrp_frame_velocities")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_frame_velocities_c
    type(c_ptr), value, intent(in) :: frame
    real(kind=c_float), dimension(:, :) :: data
    integer(kind=c_size_t), value :: size
end function

! Function "chrp_frame_velocities_set", at bindings/c/chemharp.h:219
function chrp_frame_velocities_set_c(frame, data, size) bind(C, name="chrp_frame_velocities_set")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_frame_velocities_set_c
    type(c_ptr), value :: frame
    real(kind=c_float), dimension(:, :) :: data
    integer(kind=c_size_t), value :: size
end function

! Function "chrp_frame_has_velocities", at bindings/c/chemharp.h:227
function chrp_frame_has_velocities_c(frame, has_vel) bind(C, name="chrp_frame_has_velocities")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_frame_has_velocities_c
    type(c_ptr), value, intent(in) :: frame
    logical(kind=c_bool) :: has_vel
end function

! Function "chrp_frame_cell_set", at bindings/c/chemharp.h:235
function chrp_frame_cell_set_c(frame, cell) bind(C, name="chrp_frame_cell_set")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_frame_cell_set_c
    type(c_ptr), value :: frame
    type(c_ptr), value, intent(in) :: cell
end function

! Function "chrp_frame_topology_set", at bindings/c/chemharp.h:243
function chrp_frame_topology_set_c(frame, topology) bind(C, name="chrp_frame_topology_set")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_frame_topology_set_c
    type(c_ptr), value :: frame
    type(c_ptr), value, intent(in) :: topology
end function

! Function "chrp_frame_step", at bindings/c/chemharp.h:251
function chrp_frame_step_c(frame, step) bind(C, name="chrp_frame_step")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_frame_step_c
    type(c_ptr), value, intent(in) :: frame
    integer(kind=c_size_t) :: step
end function

! Function "chrp_frame_step_set", at bindings/c/chemharp.h:259
function chrp_frame_step_set_c(frame, step) bind(C, name="chrp_frame_step_set")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_frame_step_set_c
    type(c_ptr), value :: frame
    integer(kind=c_size_t), value :: step
end function

! Function "chrp_frame_free", at bindings/c/chemharp.h:266
function chrp_frame_free_c(frame) bind(C, name="chrp_frame_free")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_frame_free_c
    type(c_ptr), value :: frame
end function

! Function "chrp_cell", at bindings/c/chemharp.h:275
function chrp_cell_c(a, b, c, alpha, beta, gamma) bind(C, name="chrp_cell")
    use iso_c_binding
    implicit none
    type(c_ptr) :: chrp_cell_c
    real(kind=c_double), value :: a
    real(kind=c_double), value :: b
    real(kind=c_double), value :: c
    real(kind=c_double), value :: alpha
    real(kind=c_double), value :: beta
    real(kind=c_double), value :: gamma
end function

! Function "chrp_cell_from_frame", at bindings/c/chemharp.h:282
function chrp_cell_from_frame_c(frame) bind(C, name="chrp_cell_from_frame")
    use iso_c_binding
    implicit none
    type(c_ptr) :: chrp_cell_from_frame_c
    type(c_ptr), value :: frame
end function

! Function "chrp_cell_lengths", at bindings/c/chemharp.h:290
function chrp_cell_lengths_c(cell, a, b, c) bind(C, name="chrp_cell_lengths")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_cell_lengths_c
    type(c_ptr), value, intent(in) :: cell
    real(kind=c_double) :: a
    real(kind=c_double) :: b
    real(kind=c_double) :: c
end function

! Function "chrp_cell_lengths_set", at bindings/c/chemharp.h:298
function chrp_cell_lengths_set_c(cell, a, b, c) bind(C, name="chrp_cell_lengths_set")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_cell_lengths_set_c
    type(c_ptr), value :: cell
    real(kind=c_double), value :: a
    real(kind=c_double), value :: b
    real(kind=c_double), value :: c
end function

! Function "chrp_cell_angles", at bindings/c/chemharp.h:306
function chrp_cell_angles_c(cell, alpha, beta, gamma) bind(C, name="chrp_cell_angles")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_cell_angles_c
    type(c_ptr), value, intent(in) :: cell
    real(kind=c_double) :: alpha
    real(kind=c_double) :: beta
    real(kind=c_double) :: gamma
end function

! Function "chrp_cell_angles_set", at bindings/c/chemharp.h:314
function chrp_cell_angles_set_c(cell, alpha, beta, gamma) bind(C, name="chrp_cell_angles_set")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_cell_angles_set_c
    type(c_ptr), value :: cell
    real(kind=c_double), value :: alpha
    real(kind=c_double), value :: beta
    real(kind=c_double), value :: gamma
end function

! Function "chrp_cell_matrix", at bindings/c/chemharp.h:322
function chrp_cell_matrix_c(cell, mat) bind(C, name="chrp_cell_matrix")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_cell_matrix_c
    type(c_ptr), value, intent(in) :: cell
    real(kind=c_double), dimension(3, 3) :: mat
end function

! Function "chrp_cell_type", at bindings/c/chemharp.h:340
function chrp_cell_type_c(cell, type) bind(C, name="chrp_cell_type")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_cell_type_c
    type(c_ptr), value, intent(in) :: cell
    include "generated/cenums.f90"
    integer(kind(cell_type)) :: type
end function

! Function "chrp_cell_type_set", at bindings/c/chemharp.h:348
function chrp_cell_type_set_c(cell, type) bind(C, name="chrp_cell_type_set")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_cell_type_set_c
    type(c_ptr), value :: cell
    include "generated/cenums.f90"
    integer(kind(cell_type)), value :: type
end function

! Function "chrp_cell_periodicity", at bindings/c/chemharp.h:356
function chrp_cell_periodicity_c(cell, x, y, z) bind(C, name="chrp_cell_periodicity")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_cell_periodicity_c
    type(c_ptr), value, intent(in) :: cell
    logical(kind=c_bool) :: x
    logical(kind=c_bool) :: y
    logical(kind=c_bool) :: z
end function

! Function "chrp_cell_periodicity_set", at bindings/c/chemharp.h:364
function chrp_cell_periodicity_set_c(cell, x, y, z) bind(C, name="chrp_cell_periodicity_set")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_cell_periodicity_set_c
    type(c_ptr), value :: cell
    logical(kind=c_bool), value :: x
    logical(kind=c_bool), value :: y
    logical(kind=c_bool), value :: z
end function

! Function "chrp_cell_free", at bindings/c/chemharp.h:371
function chrp_cell_free_c(cell) bind(C, name="chrp_cell_free")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_cell_free_c
    type(c_ptr), value :: cell
end function

! Function "chrp_topology", at bindings/c/chemharp.h:380
function chrp_topology_c(frame) bind(C, name="chrp_topology")
    use iso_c_binding
    implicit none
    type(c_ptr) :: chrp_topology_c
    type(c_ptr), value :: frame
end function

! Function "chrp_empty_topology", at bindings/c/chemharp.h:386
function chrp_empty_topology_c() bind(C, name="chrp_empty_topology")
    use iso_c_binding
    implicit none
    type(c_ptr) :: chrp_empty_topology_c

end function

! Function "chrp_topology_size", at bindings/c/chemharp.h:394
function chrp_topology_size_c(topology, natoms) bind(C, name="chrp_topology_size")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_topology_size_c
    type(c_ptr), value, intent(in) :: topology
    integer(kind=c_size_t) :: natoms
end function

! Function "chrp_topology_guess", at bindings/c/chemharp.h:404
function chrp_topology_guess_c(topology, bonds) bind(C, name="chrp_topology_guess")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_topology_guess_c
    type(c_ptr), value :: topology
    logical(kind=c_bool), value :: bonds
end function

! Function "chrp_topology_append", at bindings/c/chemharp.h:412
function chrp_topology_append_c(topology, atom) bind(C, name="chrp_topology_append")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_topology_append_c
    type(c_ptr), value :: topology
    type(c_ptr), value :: atom
end function

! Function "chrp_topology_remove", at bindings/c/chemharp.h:420
function chrp_topology_remove_c(topology, i) bind(C, name="chrp_topology_remove")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_topology_remove_c
    type(c_ptr), value :: topology
    integer(kind=c_size_t), value :: i
end function

! Function "chrp_topology_isbond", at bindings/c/chemharp.h:429
function chrp_topology_isbond_c(topology, i, j, result) bind(C, name="chrp_topology_isbond")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_topology_isbond_c
    type(c_ptr), value :: topology
    integer(kind=c_size_t), value :: i
    integer(kind=c_size_t), value :: j
    logical(kind=c_bool) :: result
end function

! Function "chrp_topology_isangle", at bindings/c/chemharp.h:438
function chrp_topology_isangle_c(topology, i, j, k, result) bind(C, name="chrp_topology_isangle")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_topology_isangle_c
    type(c_ptr), value :: topology
    integer(kind=c_size_t), value :: i
    integer(kind=c_size_t), value :: j
    integer(kind=c_size_t), value :: k
    logical(kind=c_bool) :: result
end function

! Function "chrp_topology_isdihedral", at bindings/c/chemharp.h:448
function chrp_topology_isdihedral_c(topology, i, j, k, m, result) bind(C, name="chrp_topology_isdihedral")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_topology_isdihedral_c
    type(c_ptr), value :: topology
    integer(kind=c_size_t), value :: i
    integer(kind=c_size_t), value :: j
    integer(kind=c_size_t), value :: k
    integer(kind=c_size_t), value :: m
    logical(kind=c_bool) :: result
end function

! Function "chrp_topology_bonds_count", at bindings/c/chemharp.h:457
function chrp_topology_bonds_count_c(topology, nbonds) bind(C, name="chrp_topology_bonds_count")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_topology_bonds_count_c
    type(c_ptr), value :: topology
    integer(kind=c_size_t) :: nbonds
end function

! Function "chrp_topology_angles_count", at bindings/c/chemharp.h:465
function chrp_topology_angles_count_c(topology, nangles) bind(C, name="chrp_topology_angles_count")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_topology_angles_count_c
    type(c_ptr), value :: topology
    integer(kind=c_size_t) :: nangles
end function

! Function "chrp_topology_dihedrals_count", at bindings/c/chemharp.h:473
function chrp_topology_dihedrals_count_c(topology, ndihedrals) bind(C, name="chrp_topology_dihedrals_count")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_topology_dihedrals_count_c
    type(c_ptr), value :: topology
    integer(kind=c_size_t) :: ndihedrals
end function

! Function "chrp_topology_bonds", at bindings/c/chemharp.h:483
function chrp_topology_bonds_c(topology, data, nbonds) bind(C, name="chrp_topology_bonds")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_topology_bonds_c
    type(c_ptr), value :: topology
    integer(kind=c_size_t), dimension(:, :) :: data
    integer(kind=c_size_t), value :: nbonds
end function

! Function "chrp_topology_angles", at bindings/c/chemharp.h:493
function chrp_topology_angles_c(topology, data, nangles) bind(C, name="chrp_topology_angles")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_topology_angles_c
    type(c_ptr), value :: topology
    integer(kind=c_size_t), dimension(:, :) :: data
    integer(kind=c_size_t), value :: nangles
end function

! Function "chrp_topology_dihedrals", at bindings/c/chemharp.h:503
function chrp_topology_dihedrals_c(topology, data, ndihedrals) bind(C, name="chrp_topology_dihedrals")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_topology_dihedrals_c
    type(c_ptr), value :: topology
    integer(kind=c_size_t), dimension(:, :) :: data
    integer(kind=c_size_t), value :: ndihedrals
end function

! Function "chrp_topology_add_bond", at bindings/c/chemharp.h:511
function chrp_topology_add_bond_c(topology, i, j) bind(C, name="chrp_topology_add_bond")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_topology_add_bond_c
    type(c_ptr), value :: topology
    integer(kind=c_size_t), value :: i
    integer(kind=c_size_t), value :: j
end function

! Function "chrp_topology_remove_bond", at bindings/c/chemharp.h:519
function chrp_topology_remove_bond_c(topology, i, j) bind(C, name="chrp_topology_remove_bond")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_topology_remove_bond_c
    type(c_ptr), value :: topology
    integer(kind=c_size_t), value :: i
    integer(kind=c_size_t), value :: j
end function

! Function "chrp_topology_free", at bindings/c/chemharp.h:526
function chrp_topology_free_c(topology) bind(C, name="chrp_topology_free")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_topology_free_c
    type(c_ptr), value :: topology
end function

! Function "chrp_atom", at bindings/c/chemharp.h:535
function chrp_atom_c(frame, idx) bind(C, name="chrp_atom")
    use iso_c_binding
    implicit none
    type(c_ptr) :: chrp_atom_c
    type(c_ptr), value :: frame
    integer(kind=c_size_t), value :: idx
end function

! Function "chrp_atom_from_name", at bindings/c/chemharp.h:542
function chrp_atom_from_name_c(name) bind(C, name="chrp_atom_from_name")
    use iso_c_binding
    implicit none
    type(c_ptr) :: chrp_atom_from_name_c
    character(len=1, kind=c_char), dimension(*), intent(in) :: name
end function

! Function "chrp_atom_from_topology", at bindings/c/chemharp.h:550
function chrp_atom_from_topology_c(topology, idx) bind(C, name="chrp_atom_from_topology")
    use iso_c_binding
    implicit none
    type(c_ptr) :: chrp_atom_from_topology_c
    type(c_ptr), value :: topology
    integer(kind=c_size_t), value :: idx
end function

! Function "chrp_atom_mass", at bindings/c/chemharp.h:557
function chrp_atom_mass_c(atom, mass) bind(C, name="chrp_atom_mass")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_atom_mass_c
    type(c_ptr), value, intent(in) :: atom
    real(kind=c_float) :: mass
end function

! Function "chrp_atom_mass_set", at bindings/c/chemharp.h:564
function chrp_atom_mass_set_c(atom, mass) bind(C, name="chrp_atom_mass_set")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_atom_mass_set_c
    type(c_ptr), value :: atom
    real(kind=c_float), value :: mass
end function

! Function "chrp_atom_charge", at bindings/c/chemharp.h:571
function chrp_atom_charge_c(atom, charge) bind(C, name="chrp_atom_charge")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_atom_charge_c
    type(c_ptr), value, intent(in) :: atom
    real(kind=c_float) :: charge
end function

! Function "chrp_atom_charge_set", at bindings/c/chemharp.h:578
function chrp_atom_charge_set_c(atom, charge) bind(C, name="chrp_atom_charge_set")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_atom_charge_set_c
    type(c_ptr), value :: atom
    real(kind=c_float), value :: charge
end function

! Function "chrp_atom_name", at bindings/c/chemharp.h:586
function chrp_atom_name_c(atom, name, buffsize) bind(C, name="chrp_atom_name")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_atom_name_c
    type(c_ptr), value, intent(in) :: atom
    character(len=1, kind=c_char), dimension(*) :: name
    integer(kind=c_size_t), value :: buffsize
end function

! Function "chrp_atom_name_set", at bindings/c/chemharp.h:593
function chrp_atom_name_set_c(atom, name) bind(C, name="chrp_atom_name_set")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_atom_name_set_c
    type(c_ptr), value :: atom
    character(len=1, kind=c_char), dimension(*), intent(in) :: name
end function

! Function "chrp_atom_free", at bindings/c/chemharp.h:600
function chrp_atom_free_c(atom) bind(C, name="chrp_atom_free")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_atom_free_c
    type(c_ptr), value :: atom
end function

end interface
