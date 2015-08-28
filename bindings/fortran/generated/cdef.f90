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
! Function "chrp_strerror", at chemharp.h:60
function chrp_strerror_c(status) bind(C, name="chrp_strerror")
    use iso_c_binding
    implicit none
    type(c_ptr) :: chrp_strerror_c
    integer(kind=c_int), value :: status
end function

! Function "chrp_last_error", at chemharp.h:66
function chrp_last_error_c() bind(C, name="chrp_last_error")
    use iso_c_binding
    implicit none
    type(c_ptr) :: chrp_last_error_c

end function

! Function "chrp_loglevel", at chemharp.h:87
function chrp_loglevel_c(level) bind(C, name="chrp_loglevel")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_loglevel_c
    integer(kind=c_int), value :: level
end function

! Function "chrp_logfile", at chemharp.h:94
function chrp_logfile_c(file) bind(C, name="chrp_logfile")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_logfile_c
    character(len=1, kind=c_char), dimension(*), intent(in) :: file
end function

! Function "chrp_log_stderr", at chemharp.h:100
function chrp_log_stderr_c() bind(C, name="chrp_log_stderr")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_log_stderr_c

end function

! Function "chrp_open", at chemharp.h:109
function chrp_open_c(filename, mode) bind(C, name="chrp_open")
    use iso_c_binding
    implicit none
    type(c_ptr) :: chrp_open_c
    character(len=1, kind=c_char), dimension(*), intent(in) :: filename
    character(len=1, kind=c_char), dimension(*), intent(in) :: mode
end function

! Function "chrp_trajectory_read", at chemharp.h:117
function chrp_trajectory_read_c(file, frame) bind(C, name="chrp_trajectory_read")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_trajectory_read_c
    type(c_ptr), value :: file
    type(c_ptr), value :: frame
end function

! Function "chrp_trajectory_read_step", at chemharp.h:126
function chrp_trajectory_read_step_c(file, step, frame) bind(C, name="chrp_trajectory_read_step")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_trajectory_read_step_c
    type(c_ptr), value :: file
    integer(kind=c_size_t), value :: step
    type(c_ptr), value :: frame
end function

! Function "chrp_trajectory_write", at chemharp.h:134
function chrp_trajectory_write_c(file, frame) bind(C, name="chrp_trajectory_write")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_trajectory_write_c
    type(c_ptr), value :: file
    type(c_ptr), value, intent(in) :: frame
end function

! Function "chrp_trajectory_set_topology", at chemharp.h:144
function chrp_trajectory_set_topology_c(file, topology) bind(C, name="chrp_trajectory_set_topology")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_trajectory_set_topology_c
    type(c_ptr), value :: file
    type(c_ptr), value, intent(in) :: topology
end function

! Function "chrp_trajectory_set_topology_file", at chemharp.h:153
function chrp_trajectory_set_topology_file_c(file, filename) bind(C, name="chrp_trajectory_set_topology_file")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_trajectory_set_topology_file_c
    type(c_ptr), value :: file
    character(len=1, kind=c_char), dimension(*), intent(in) :: filename
end function

! Function "chrp_trajectory_set_cell", at chemharp.h:163
function chrp_trajectory_set_cell_c(file, cell) bind(C, name="chrp_trajectory_set_cell")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_trajectory_set_cell_c
    type(c_ptr), value :: file
    type(c_ptr), value, intent(in) :: cell
end function

! Function "chrp_trajectory_nsteps", at chemharp.h:171
function chrp_trajectory_nsteps_c(file, nsteps) bind(C, name="chrp_trajectory_nsteps")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_trajectory_nsteps_c
    type(c_ptr), value :: file
    integer(kind=c_size_t) :: nsteps
end function

! Function "chrp_trajectory_close", at chemharp.h:179
function chrp_trajectory_close_c(file) bind(C, name="chrp_trajectory_close")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_trajectory_close_c
    type(c_ptr), value :: file
end function

! Function "chrp_frame", at chemharp.h:188
function chrp_frame_c(natoms) bind(C, name="chrp_frame")
    use iso_c_binding
    implicit none
    type(c_ptr) :: chrp_frame_c
    integer(kind=c_size_t), value :: natoms
end function

! Function "chrp_frame_atoms_count", at chemharp.h:196
function chrp_frame_atoms_count_c(frame, natoms) bind(C, name="chrp_frame_atoms_count")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_frame_atoms_count_c
    type(c_ptr), value, intent(in) :: frame
    integer(kind=c_size_t) :: natoms
end function

! Function "chrp_frame_positions", at chemharp.h:205
function chrp_frame_positions_c(frame, data, size) bind(C, name="chrp_frame_positions")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_frame_positions_c
    type(c_ptr), value, intent(in) :: frame
    type(c_ptr), value :: data
    integer(kind=c_size_t), value :: size
end function

! Function "chrp_frame_set_positions", at chemharp.h:214
function chrp_frame_set_positions_c(frame, data, size) bind(C, name="chrp_frame_set_positions")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_frame_set_positions_c
    type(c_ptr), value :: frame
    type(c_ptr), value :: data
    integer(kind=c_size_t), value :: size
end function

! Function "chrp_frame_velocities", at chemharp.h:223
function chrp_frame_velocities_c(frame, data, size) bind(C, name="chrp_frame_velocities")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_frame_velocities_c
    type(c_ptr), value, intent(in) :: frame
    type(c_ptr), value :: data
    integer(kind=c_size_t), value :: size
end function

! Function "chrp_frame_set_velocities", at chemharp.h:232
function chrp_frame_set_velocities_c(frame, data, size) bind(C, name="chrp_frame_set_velocities")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_frame_set_velocities_c
    type(c_ptr), value :: frame
    type(c_ptr), value :: data
    integer(kind=c_size_t), value :: size
end function

! Function "chrp_frame_has_velocities", at chemharp.h:240
function chrp_frame_has_velocities_c(frame, has_vel) bind(C, name="chrp_frame_has_velocities")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_frame_has_velocities_c
    type(c_ptr), value, intent(in) :: frame
    logical(kind=c_bool) :: has_vel
end function

! Function "chrp_frame_set_cell", at chemharp.h:248
function chrp_frame_set_cell_c(frame, cell) bind(C, name="chrp_frame_set_cell")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_frame_set_cell_c
    type(c_ptr), value :: frame
    type(c_ptr), value, intent(in) :: cell
end function

! Function "chrp_frame_set_topology", at chemharp.h:256
function chrp_frame_set_topology_c(frame, topology) bind(C, name="chrp_frame_set_topology")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_frame_set_topology_c
    type(c_ptr), value :: frame
    type(c_ptr), value, intent(in) :: topology
end function

! Function "chrp_frame_step", at chemharp.h:264
function chrp_frame_step_c(frame, step) bind(C, name="chrp_frame_step")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_frame_step_c
    type(c_ptr), value, intent(in) :: frame
    integer(kind=c_size_t) :: step
end function

! Function "chrp_frame_set_step", at chemharp.h:272
function chrp_frame_set_step_c(frame, step) bind(C, name="chrp_frame_set_step")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_frame_set_step_c
    type(c_ptr), value :: frame
    integer(kind=c_size_t), value :: step
end function

! Function "chrp_frame_guess_topology", at chemharp.h:282
function chrp_frame_guess_topology_c(frame, bonds) bind(C, name="chrp_frame_guess_topology")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_frame_guess_topology_c
    type(c_ptr), value :: frame
    logical(kind=c_bool), value :: bonds
end function

! Function "chrp_frame_free", at chemharp.h:289
function chrp_frame_free_c(frame) bind(C, name="chrp_frame_free")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_frame_free_c
    type(c_ptr), value :: frame
end function

! Function "chrp_cell", at chemharp.h:297
function chrp_cell_c(a, b, c) bind(C, name="chrp_cell")
    use iso_c_binding
    implicit none
    type(c_ptr) :: chrp_cell_c
    real(kind=c_double), value :: a
    real(kind=c_double), value :: b
    real(kind=c_double), value :: c
end function

! Function "chrp_cell_triclinic", at chemharp.h:305
function chrp_cell_triclinic_c(a, b, c, alpha, beta, gamma) bind(C, name="chrp_cell_triclinic")
    use iso_c_binding
    implicit none
    type(c_ptr) :: chrp_cell_triclinic_c
    real(kind=c_double), value :: a
    real(kind=c_double), value :: b
    real(kind=c_double), value :: c
    real(kind=c_double), value :: alpha
    real(kind=c_double), value :: beta
    real(kind=c_double), value :: gamma
end function

! Function "chrp_cell_from_frame", at chemharp.h:312
function chrp_cell_from_frame_c(frame) bind(C, name="chrp_cell_from_frame")
    use iso_c_binding
    implicit none
    type(c_ptr) :: chrp_cell_from_frame_c
    type(c_ptr), value :: frame
end function

! Function "chrp_cell_volume", at chemharp.h:320
function chrp_cell_volume_c(cell, V) bind(C, name="chrp_cell_volume")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_cell_volume_c
    type(c_ptr), value, intent(in) :: cell
    real(kind=c_double) :: V
end function

! Function "chrp_cell_lengths", at chemharp.h:328
function chrp_cell_lengths_c(cell, a, b, c) bind(C, name="chrp_cell_lengths")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_cell_lengths_c
    type(c_ptr), value, intent(in) :: cell
    real(kind=c_double) :: a
    real(kind=c_double) :: b
    real(kind=c_double) :: c
end function

! Function "chrp_cell_set_lengths", at chemharp.h:336
function chrp_cell_set_lengths_c(cell, a, b, c) bind(C, name="chrp_cell_set_lengths")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_cell_set_lengths_c
    type(c_ptr), value :: cell
    real(kind=c_double), value :: a
    real(kind=c_double), value :: b
    real(kind=c_double), value :: c
end function

! Function "chrp_cell_angles", at chemharp.h:344
function chrp_cell_angles_c(cell, alpha, beta, gamma) bind(C, name="chrp_cell_angles")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_cell_angles_c
    type(c_ptr), value, intent(in) :: cell
    real(kind=c_double) :: alpha
    real(kind=c_double) :: beta
    real(kind=c_double) :: gamma
end function

! Function "chrp_cell_set_angles", at chemharp.h:352
function chrp_cell_set_angles_c(cell, alpha, beta, gamma) bind(C, name="chrp_cell_set_angles")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_cell_set_angles_c
    type(c_ptr), value :: cell
    real(kind=c_double), value :: alpha
    real(kind=c_double), value :: beta
    real(kind=c_double), value :: gamma
end function

! Function "chrp_cell_matrix", at chemharp.h:360
function chrp_cell_matrix_c(cell, mat) bind(C, name="chrp_cell_matrix")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_cell_matrix_c
    type(c_ptr), value, intent(in) :: cell
    type(c_ptr), value :: mat
end function

! Function "chrp_cell_type", at chemharp.h:378
function chrp_cell_type_c(cell, type) bind(C, name="chrp_cell_type")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_cell_type_c
    type(c_ptr), value, intent(in) :: cell
    integer(kind=c_int) :: type
end function

! Function "chrp_cell_set_type", at chemharp.h:386
function chrp_cell_set_type_c(cell, type) bind(C, name="chrp_cell_set_type")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_cell_set_type_c
    type(c_ptr), value :: cell
    integer(kind=c_int), value :: type
end function

! Function "chrp_cell_periodicity", at chemharp.h:394
function chrp_cell_periodicity_c(cell, x, y, z) bind(C, name="chrp_cell_periodicity")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_cell_periodicity_c
    type(c_ptr), value, intent(in) :: cell
    logical(kind=c_bool) :: x
    logical(kind=c_bool) :: y
    logical(kind=c_bool) :: z
end function

! Function "chrp_cell_set_periodicity", at chemharp.h:402
function chrp_cell_set_periodicity_c(cell, x, y, z) bind(C, name="chrp_cell_set_periodicity")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_cell_set_periodicity_c
    type(c_ptr), value :: cell
    logical(kind=c_bool), value :: x
    logical(kind=c_bool), value :: y
    logical(kind=c_bool), value :: z
end function

! Function "chrp_cell_free", at chemharp.h:409
function chrp_cell_free_c(cell) bind(C, name="chrp_cell_free")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_cell_free_c
    type(c_ptr), value :: cell
end function

! Function "chrp_topology", at chemharp.h:417
function chrp_topology_c() bind(C, name="chrp_topology")
    use iso_c_binding
    implicit none
    type(c_ptr) :: chrp_topology_c

end function

! Function "chrp_topology_from_frame", at chemharp.h:424
function chrp_topology_from_frame_c(frame) bind(C, name="chrp_topology_from_frame")
    use iso_c_binding
    implicit none
    type(c_ptr) :: chrp_topology_from_frame_c
    type(c_ptr), value :: frame
end function

! Function "chrp_topology_atoms_count", at chemharp.h:432
function chrp_topology_atoms_count_c(topology, natoms) bind(C, name="chrp_topology_atoms_count")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_topology_atoms_count_c
    type(c_ptr), value, intent(in) :: topology
    integer(kind=c_size_t) :: natoms
end function

! Function "chrp_topology_append", at chemharp.h:440
function chrp_topology_append_c(topology, atom) bind(C, name="chrp_topology_append")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_topology_append_c
    type(c_ptr), value :: topology
    type(c_ptr), value, intent(in) :: atom
end function

! Function "chrp_topology_remove", at chemharp.h:448
function chrp_topology_remove_c(topology, i) bind(C, name="chrp_topology_remove")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_topology_remove_c
    type(c_ptr), value :: topology
    integer(kind=c_size_t), value :: i
end function

! Function "chrp_topology_isbond", at chemharp.h:457
function chrp_topology_isbond_c(topology, i, j, result) bind(C, name="chrp_topology_isbond")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_topology_isbond_c
    type(c_ptr), value, intent(in) :: topology
    integer(kind=c_size_t), value :: i
    integer(kind=c_size_t), value :: j
    logical(kind=c_bool) :: result
end function

! Function "chrp_topology_isangle", at chemharp.h:466
function chrp_topology_isangle_c(topology, i, j, k, result) bind(C, name="chrp_topology_isangle")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_topology_isangle_c
    type(c_ptr), value, intent(in) :: topology
    integer(kind=c_size_t), value :: i
    integer(kind=c_size_t), value :: j
    integer(kind=c_size_t), value :: k
    logical(kind=c_bool) :: result
end function

! Function "chrp_topology_isdihedral", at chemharp.h:475
function chrp_topology_isdihedral_c(topology, i, j, k, m, result) bind(C, name="chrp_topology_isdihedral")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_topology_isdihedral_c
    type(c_ptr), value, intent(in) :: topology
    integer(kind=c_size_t), value :: i
    integer(kind=c_size_t), value :: j
    integer(kind=c_size_t), value :: k
    integer(kind=c_size_t), value :: m
    logical(kind=c_bool) :: result
end function

! Function "chrp_topology_bonds_count", at chemharp.h:483
function chrp_topology_bonds_count_c(topology, nbonds) bind(C, name="chrp_topology_bonds_count")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_topology_bonds_count_c
    type(c_ptr), value, intent(in) :: topology
    integer(kind=c_size_t) :: nbonds
end function

! Function "chrp_topology_angles_count", at chemharp.h:491
function chrp_topology_angles_count_c(topology, nangles) bind(C, name="chrp_topology_angles_count")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_topology_angles_count_c
    type(c_ptr), value, intent(in) :: topology
    integer(kind=c_size_t) :: nangles
end function

! Function "chrp_topology_dihedrals_count", at chemharp.h:499
function chrp_topology_dihedrals_count_c(topology, ndihedrals) bind(C, name="chrp_topology_dihedrals_count")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_topology_dihedrals_count_c
    type(c_ptr), value, intent(in) :: topology
    integer(kind=c_size_t) :: ndihedrals
end function

! Function "chrp_topology_bonds", at chemharp.h:509
function chrp_topology_bonds_c(topology, data, nbonds) bind(C, name="chrp_topology_bonds")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_topology_bonds_c
    type(c_ptr), value, intent(in) :: topology
    type(c_ptr), value :: data
    integer(kind=c_size_t), value :: nbonds
end function

! Function "chrp_topology_angles", at chemharp.h:519
function chrp_topology_angles_c(topology, data, nangles) bind(C, name="chrp_topology_angles")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_topology_angles_c
    type(c_ptr), value, intent(in) :: topology
    type(c_ptr), value :: data
    integer(kind=c_size_t), value :: nangles
end function

! Function "chrp_topology_dihedrals", at chemharp.h:529
function chrp_topology_dihedrals_c(topology, data, ndihedrals) bind(C, name="chrp_topology_dihedrals")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_topology_dihedrals_c
    type(c_ptr), value, intent(in) :: topology
    type(c_ptr), value :: data
    integer(kind=c_size_t), value :: ndihedrals
end function

! Function "chrp_topology_add_bond", at chemharp.h:537
function chrp_topology_add_bond_c(topology, i, j) bind(C, name="chrp_topology_add_bond")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_topology_add_bond_c
    type(c_ptr), value :: topology
    integer(kind=c_size_t), value :: i
    integer(kind=c_size_t), value :: j
end function

! Function "chrp_topology_remove_bond", at chemharp.h:545
function chrp_topology_remove_bond_c(topology, i, j) bind(C, name="chrp_topology_remove_bond")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_topology_remove_bond_c
    type(c_ptr), value :: topology
    integer(kind=c_size_t), value :: i
    integer(kind=c_size_t), value :: j
end function

! Function "chrp_topology_free", at chemharp.h:552
function chrp_topology_free_c(topology) bind(C, name="chrp_topology_free")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_topology_free_c
    type(c_ptr), value :: topology
end function

! Function "chrp_atom", at chemharp.h:561
function chrp_atom_c(name) bind(C, name="chrp_atom")
    use iso_c_binding
    implicit none
    type(c_ptr) :: chrp_atom_c
    character(len=1, kind=c_char), dimension(*), intent(in) :: name
end function

! Function "chrp_atom_from_frame", at chemharp.h:569
function chrp_atom_from_frame_c(frame, idx) bind(C, name="chrp_atom_from_frame")
    use iso_c_binding
    implicit none
    type(c_ptr) :: chrp_atom_from_frame_c
    type(c_ptr), value, intent(in) :: frame
    integer(kind=c_size_t), value :: idx
end function

! Function "chrp_atom_from_topology", at chemharp.h:577
function chrp_atom_from_topology_c(topology, idx) bind(C, name="chrp_atom_from_topology")
    use iso_c_binding
    implicit none
    type(c_ptr) :: chrp_atom_from_topology_c
    type(c_ptr), value, intent(in) :: topology
    integer(kind=c_size_t), value :: idx
end function

! Function "chrp_atom_mass", at chemharp.h:585
function chrp_atom_mass_c(atom, mass) bind(C, name="chrp_atom_mass")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_atom_mass_c
    type(c_ptr), value, intent(in) :: atom
    real(kind=c_float) :: mass
end function

! Function "chrp_atom_set_mass", at chemharp.h:593
function chrp_atom_set_mass_c(atom, mass) bind(C, name="chrp_atom_set_mass")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_atom_set_mass_c
    type(c_ptr), value :: atom
    real(kind=c_float), value :: mass
end function

! Function "chrp_atom_charge", at chemharp.h:601
function chrp_atom_charge_c(atom, charge) bind(C, name="chrp_atom_charge")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_atom_charge_c
    type(c_ptr), value, intent(in) :: atom
    real(kind=c_float) :: charge
end function

! Function "chrp_atom_set_charge", at chemharp.h:609
function chrp_atom_set_charge_c(atom, charge) bind(C, name="chrp_atom_set_charge")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_atom_set_charge_c
    type(c_ptr), value :: atom
    real(kind=c_float), value :: charge
end function

! Function "chrp_atom_name", at chemharp.h:618
function chrp_atom_name_c(atom, name, buffsize) bind(C, name="chrp_atom_name")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_atom_name_c
    type(c_ptr), value, intent(in) :: atom
    character(len=1, kind=c_char), dimension(*) :: name
    integer(kind=c_size_t), value :: buffsize
end function

! Function "chrp_atom_set_name", at chemharp.h:626
function chrp_atom_set_name_c(atom, name) bind(C, name="chrp_atom_set_name")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_atom_set_name_c
    type(c_ptr), value :: atom
    character(len=1, kind=c_char), dimension(*), intent(in) :: name
end function

! Function "chrp_atom_full_name", at chemharp.h:635
function chrp_atom_full_name_c(atom, name, buffsize) bind(C, name="chrp_atom_full_name")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_atom_full_name_c
    type(c_ptr), value, intent(in) :: atom
    character(len=1, kind=c_char), dimension(*) :: name
    integer(kind=c_size_t), value :: buffsize
end function

! Function "chrp_atom_vdw_radius", at chemharp.h:643
function chrp_atom_vdw_radius_c(atom, radius) bind(C, name="chrp_atom_vdw_radius")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_atom_vdw_radius_c
    type(c_ptr), value, intent(in) :: atom
    real(kind=c_double) :: radius
end function

! Function "chrp_atom_covalent_radius", at chemharp.h:651
function chrp_atom_covalent_radius_c(atom, radius) bind(C, name="chrp_atom_covalent_radius")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_atom_covalent_radius_c
    type(c_ptr), value, intent(in) :: atom
    real(kind=c_double) :: radius
end function

! Function "chrp_atom_atomic_number", at chemharp.h:659
function chrp_atom_atomic_number_c(atom, number) bind(C, name="chrp_atom_atomic_number")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_atom_atomic_number_c
    type(c_ptr), value, intent(in) :: atom
    integer(kind=c_int) :: number
end function

! Function "chrp_atom_free", at chemharp.h:666
function chrp_atom_free_c(atom) bind(C, name="chrp_atom_free")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_atom_free_c
    type(c_ptr), value :: atom
end function

end interface
