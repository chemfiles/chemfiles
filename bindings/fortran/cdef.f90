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
! Function "chrp_strerror", at bindings/c/chemharp.h:56
function chrp_strerror_c(status) bind(C, name="chrp_strerror")
    use iso_c_binding
    implicit none
    type(c_ptr) :: chrp_strerror_c
    integer(kind=c_int), value :: status
end function

! Function "chrp_last_error", at bindings/c/chemharp.h:62
function chrp_last_error_c() bind(C, name="chrp_last_error")
    use iso_c_binding
    implicit none
    type(c_ptr) :: chrp_last_error_c

end function

! Function "chrp_loglevel", at bindings/c/chemharp.h:82
function chrp_loglevel_c(level) bind(C, name="chrp_loglevel")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_loglevel_c
    include "cenums.f90"
    integer(kind(log_level)), value :: level
end function

! Function "chrp_logfile", at bindings/c/chemharp.h:88
function chrp_logfile_c(file) bind(C, name="chrp_logfile")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_logfile_c
    character(len=1, kind=c_char), dimension(:), intent(in) :: file
end function

! Function "chrp_log_stderr", at bindings/c/chemharp.h:94
function chrp_log_stderr_c() bind(C, name="chrp_log_stderr")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_log_stderr_c

end function

! Function "chrp_open", at bindings/c/chemharp.h:103
function chrp_open_c(filename, mode) bind(C, name="chrp_open")
    use iso_c_binding
    implicit none
    type(c_ptr) :: chrp_open_c
    character(len=1, kind=c_char), dimension(:), intent(in) :: filename
    character(len=1, kind=c_char), dimension(:), intent(in) :: mode
end function

! Function "chrp_read_step", at bindings/c/chemharp.h:112
function chrp_read_step_c(file, step, frame) bind(C, name="chrp_read_step")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_read_step_c
    type(c_ptr) :: file
    integer(kind=c_size_t), value :: step
    type(c_ptr), intent(in) :: frame
end function

! Function "chrp_read_next_step", at bindings/c/chemharp.h:120
function chrp_read_next_step_c(file, frame) bind(C, name="chrp_read_next_step")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_read_next_step_c
    type(c_ptr) :: file
    type(c_ptr), intent(in) :: frame
end function

! Function "chrp_write_step", at bindings/c/chemharp.h:128
function chrp_write_step_c(file, frame) bind(C, name="chrp_write_step")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_write_step_c
    type(c_ptr) :: file
    type(c_ptr), intent(in) :: frame
end function

! Function "chrp_close", at bindings/c/chemharp.h:135
function chrp_close_c(file) bind(C, name="chrp_close")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_close_c
    type(c_ptr) :: file
end function

! Function "chrp_frame", at bindings/c/chemharp.h:144
function chrp_frame_c(natoms) bind(C, name="chrp_frame")
    use iso_c_binding
    implicit none
    type(c_ptr) :: chrp_frame_c
    integer(kind=c_size_t), value :: natoms
end function

! Function "chrp_frame_size", at bindings/c/chemharp.h:152
function chrp_frame_size_c(frame, natoms) bind(C, name="chrp_frame_size")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_frame_size_c
    type(c_ptr), intent(in) :: frame
    integer(kind=c_size_t) :: natoms
end function

! Function "chrp_frame_positions", at bindings/c/chemharp.h:161
function chrp_frame_positions_c(frame, data, size) bind(C, name="chrp_frame_positions")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_frame_positions_c
    type(c_ptr), intent(in) :: frame
    real(kind=c_float), dimension(:, :) :: data
    integer(kind=c_size_t), value :: size
end function

! Function "chrp_frame_positions_set", at bindings/c/chemharp.h:170
function chrp_frame_positions_set_c(frame, data, size) bind(C, name="chrp_frame_positions_set")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_frame_positions_set_c
    type(c_ptr) :: frame
    real(kind=c_float), dimension(:, :) :: data
    integer(kind=c_size_t), value :: size
end function

! Function "chrp_frame_velocities", at bindings/c/chemharp.h:179
function chrp_frame_velocities_c(frame, data, size) bind(C, name="chrp_frame_velocities")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_frame_velocities_c
    type(c_ptr), intent(in) :: frame
    real(kind=c_float), dimension(:, :) :: data
    integer(kind=c_size_t), value :: size
end function

! Function "chrp_frame_velocities_set", at bindings/c/chemharp.h:188
function chrp_frame_velocities_set_c(frame, data, size) bind(C, name="chrp_frame_velocities_set")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_frame_velocities_set_c
    type(c_ptr) :: frame
    real(kind=c_float), dimension(:, :) :: data
    integer(kind=c_size_t), value :: size
end function

! Function "chrp_frame_free", at bindings/c/chemharp.h:195
function chrp_frame_free_c(frame) bind(C, name="chrp_frame_free")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_frame_free_c
    type(c_ptr) :: frame
end function

! Function "chrp_cell", at bindings/c/chemharp.h:203
function chrp_cell_c(frame) bind(C, name="chrp_cell")
    use iso_c_binding
    implicit none
    type(c_ptr) :: chrp_cell_c
    type(c_ptr) :: frame
end function

! Function "chrp_cell_lengths", at bindings/c/chemharp.h:211
function chrp_cell_lengths_c(cell, a, b, c) bind(C, name="chrp_cell_lengths")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_cell_lengths_c
    type(c_ptr), intent(in) :: cell
    real(kind=c_double) :: a
    real(kind=c_double) :: b
    real(kind=c_double) :: c
end function

! Function "chrp_cell_lengths_set", at bindings/c/chemharp.h:219
function chrp_cell_lengths_set_c(cell, a, b, c) bind(C, name="chrp_cell_lengths_set")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_cell_lengths_set_c
    type(c_ptr) :: cell
    real(kind=c_double), value :: a
    real(kind=c_double), value :: b
    real(kind=c_double), value :: c
end function

! Function "chrp_cell_angles", at bindings/c/chemharp.h:227
function chrp_cell_angles_c(cell, alpha, beta, gamma) bind(C, name="chrp_cell_angles")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_cell_angles_c
    type(c_ptr), intent(in) :: cell
    real(kind=c_double) :: alpha
    real(kind=c_double) :: beta
    real(kind=c_double) :: gamma
end function

! Function "chrp_cell_angles_set", at bindings/c/chemharp.h:235
function chrp_cell_angles_set_c(cell, alpha, beta, gamma) bind(C, name="chrp_cell_angles_set")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_cell_angles_set_c
    type(c_ptr) :: cell
    real(kind=c_double), value :: alpha
    real(kind=c_double), value :: beta
    real(kind=c_double), value :: gamma
end function

! Function "chrp_cell_matrix", at bindings/c/chemharp.h:243
function chrp_cell_matrix_c(cell, mat) bind(C, name="chrp_cell_matrix")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_cell_matrix_c
    type(c_ptr), intent(in) :: cell
    real(kind=c_double), dimension(3, 3) :: mat
end function

! Function "chrp_cell_type", at bindings/c/chemharp.h:261
function chrp_cell_type_c(cell, type) bind(C, name="chrp_cell_type")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_cell_type_c
    type(c_ptr), intent(in) :: cell
    include "cenums.f90"
    integer(kind(cell_type)) :: type
end function

! Function "chrp_cell_type_set", at bindings/c/chemharp.h:269
function chrp_cell_type_set_c(cell, type) bind(C, name="chrp_cell_type_set")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_cell_type_set_c
    type(c_ptr) :: cell
    include "cenums.f90"
    integer(kind(cell_type)), value :: type
end function

! Function "chrp_cell_periodicity", at bindings/c/chemharp.h:277
function chrp_cell_periodicity_c(cell, x, y, z) bind(C, name="chrp_cell_periodicity")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_cell_periodicity_c
    type(c_ptr), intent(in) :: cell
    logical(kind=c_bool) :: x
    logical(kind=c_bool) :: y
    logical(kind=c_bool) :: z
end function

! Function "chrp_cell_periodicity_set", at bindings/c/chemharp.h:285
function chrp_cell_periodicity_set_c(cell, x, y, z) bind(C, name="chrp_cell_periodicity_set")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_cell_periodicity_set_c
    type(c_ptr) :: cell
    logical(kind=c_bool), value :: x
    logical(kind=c_bool), value :: y
    logical(kind=c_bool), value :: z
end function

! Function "chrp_cell_free", at bindings/c/chemharp.h:292
function chrp_cell_free_c(cell) bind(C, name="chrp_cell_free")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_cell_free_c
    type(c_ptr) :: cell
end function

! Function "chrp_topology", at bindings/c/chemharp.h:301
function chrp_topology_c(frame) bind(C, name="chrp_topology")
    use iso_c_binding
    implicit none
    type(c_ptr) :: chrp_topology_c
    type(c_ptr) :: frame
end function

! Function "chrp_topology_append", at bindings/c/chemharp.h:309
function chrp_topology_append_c(topology, atom) bind(C, name="chrp_topology_append")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_topology_append_c
    type(c_ptr) :: topology
    type(c_ptr) :: atom
end function

! Function "chrp_topology_remove", at bindings/c/chemharp.h:317
function chrp_topology_remove_c(topology, i) bind(C, name="chrp_topology_remove")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_topology_remove_c
    type(c_ptr) :: topology
    integer(kind=c_size_t), value :: i
end function

! Function "chrp_topology_isbond", at bindings/c/chemharp.h:326
function chrp_topology_isbond_c(topology, i, j, result) bind(C, name="chrp_topology_isbond")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_topology_isbond_c
    type(c_ptr) :: topology
    integer(kind=c_size_t), value :: i
    integer(kind=c_size_t), value :: j
    logical(kind=c_bool) :: result
end function

! Function "chrp_topology_isangle", at bindings/c/chemharp.h:335
function chrp_topology_isangle_c(topology, i, j, k, result) bind(C, name="chrp_topology_isangle")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_topology_isangle_c
    type(c_ptr) :: topology
    integer(kind=c_size_t), value :: i
    integer(kind=c_size_t), value :: j
    integer(kind=c_size_t), value :: k
    logical(kind=c_bool) :: result
end function

! Function "chrp_topology_isdihedral", at bindings/c/chemharp.h:345
function chrp_topology_isdihedral_c(topology, i, j, k, m, result) bind(C, name="chrp_topology_isdihedral")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_topology_isdihedral_c
    type(c_ptr) :: topology
    integer(kind=c_size_t), value :: i
    integer(kind=c_size_t), value :: j
    integer(kind=c_size_t), value :: k
    integer(kind=c_size_t), value :: m
    logical(kind=c_bool) :: result
end function

! Function "chrp_topology_bonds_count", at bindings/c/chemharp.h:354
function chrp_topology_bonds_count_c(topology, nbonds) bind(C, name="chrp_topology_bonds_count")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_topology_bonds_count_c
    type(c_ptr) :: topology
    integer(kind=c_size_t) :: nbonds
end function

! Function "chrp_topology_angles_count", at bindings/c/chemharp.h:362
function chrp_topology_angles_count_c(topology, nangles) bind(C, name="chrp_topology_angles_count")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_topology_angles_count_c
    type(c_ptr) :: topology
    integer(kind=c_size_t) :: nangles
end function

! Function "chrp_topology_dihedrals_count", at bindings/c/chemharp.h:370
function chrp_topology_dihedrals_count_c(topology, ndihedrals) bind(C, name="chrp_topology_dihedrals_count")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_topology_dihedrals_count_c
    type(c_ptr) :: topology
    integer(kind=c_size_t) :: ndihedrals
end function

! Function "chrp_topology_bonds", at bindings/c/chemharp.h:380
function chrp_topology_bonds_c(topology, data, nbonds) bind(C, name="chrp_topology_bonds")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_topology_bonds_c
    type(c_ptr) :: topology
    integer(kind=c_size_t), dimension(:, :) :: data
    integer(kind=c_size_t), value :: nbonds
end function

! Function "chrp_topology_angles", at bindings/c/chemharp.h:390
function chrp_topology_angles_c(topology, data, nangles) bind(C, name="chrp_topology_angles")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_topology_angles_c
    type(c_ptr) :: topology
    integer(kind=c_size_t), dimension(:, :) :: data
    integer(kind=c_size_t), value :: nangles
end function

! Function "chrp_topology_dihedrals", at bindings/c/chemharp.h:400
function chrp_topology_dihedrals_c(topology, data, ndihedrals) bind(C, name="chrp_topology_dihedrals")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_topology_dihedrals_c
    type(c_ptr) :: topology
    integer(kind=c_size_t), dimension(:, :) :: data
    integer(kind=c_size_t), value :: ndihedrals
end function

! Function "chrp_topology_add_bond", at bindings/c/chemharp.h:408
function chrp_topology_add_bond_c(topology, i, j) bind(C, name="chrp_topology_add_bond")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_topology_add_bond_c
    type(c_ptr) :: topology
    integer(kind=c_size_t), value :: i
    integer(kind=c_size_t), value :: j
end function

! Function "chrp_topology_remove_bond", at bindings/c/chemharp.h:416
function chrp_topology_remove_bond_c(topology, i, j) bind(C, name="chrp_topology_remove_bond")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_topology_remove_bond_c
    type(c_ptr) :: topology
    integer(kind=c_size_t), value :: i
    integer(kind=c_size_t), value :: j
end function

! Function "chrp_topology_free", at bindings/c/chemharp.h:423
function chrp_topology_free_c(topology) bind(C, name="chrp_topology_free")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_topology_free_c
    type(c_ptr) :: topology
end function

! Function "chrp_atom", at bindings/c/chemharp.h:432
function chrp_atom_c(frame, idx) bind(C, name="chrp_atom")
    use iso_c_binding
    implicit none
    type(c_ptr) :: chrp_atom_c
    type(c_ptr) :: frame
    integer(kind=c_size_t), value :: idx
end function

! Function "chrp_topology_atom", at bindings/c/chemharp.h:440
function chrp_topology_atom_c(topology, idx) bind(C, name="chrp_topology_atom")
    use iso_c_binding
    implicit none
    type(c_ptr) :: chrp_topology_atom_c
    type(c_ptr) :: topology
    integer(kind=c_size_t), value :: idx
end function

! Function "chrp_atom_mass", at bindings/c/chemharp.h:450
function chrp_atom_mass_c(atom, mass) bind(C, name="chrp_atom_mass")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_atom_mass_c
    type(c_ptr), intent(in) :: atom
    real(kind=c_float) :: mass
end function

! Function "chrp_atom_mass_set", at bindings/c/chemharp.h:457
function chrp_atom_mass_set_c(atom, mass) bind(C, name="chrp_atom_mass_set")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_atom_mass_set_c
    type(c_ptr) :: atom
    real(kind=c_float), value :: mass
end function

! Function "chrp_atom_charge", at bindings/c/chemharp.h:464
function chrp_atom_charge_c(atom, charge) bind(C, name="chrp_atom_charge")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_atom_charge_c
    type(c_ptr), intent(in) :: atom
    real(kind=c_float) :: charge
end function

! Function "chrp_atom_charge_set", at bindings/c/chemharp.h:471
function chrp_atom_charge_set_c(atom, charge) bind(C, name="chrp_atom_charge_set")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_atom_charge_set_c
    type(c_ptr) :: atom
    real(kind=c_float), value :: charge
end function

! Function "chrp_atom_name", at bindings/c/chemharp.h:479
function chrp_atom_name_c(atom, name, buffsize) bind(C, name="chrp_atom_name")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_atom_name_c
    type(c_ptr), intent(in) :: atom
    character(len=1, kind=c_char), dimension(:) :: name
    integer(kind=c_size_t), value :: buffsize
end function

! Function "chrp_atom_name_set", at bindings/c/chemharp.h:486
function chrp_atom_name_set_c(atom, name) bind(C, name="chrp_atom_name_set")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_atom_name_set_c
    type(c_ptr) :: atom
    character(len=1, kind=c_char), dimension(:), intent(in) :: name
end function

! Function "chrp_atom_free", at bindings/c/chemharp.h:493
function chrp_atom_free_c(atom) bind(C, name="chrp_atom_free")
    use iso_c_binding
    implicit none
    integer(c_int) :: chrp_atom_free_c
    type(c_ptr) :: atom
end function

end interface
