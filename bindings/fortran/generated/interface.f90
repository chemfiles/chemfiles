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


function chrp_strerror(status) result(string)
    implicit none
    integer(kind=c_int), value :: status
    character(len=1024) :: string
    type(c_ptr) :: c_string

    c_string = chrp_strerror_c(status)
    string = c_to_f_str(c_string)
end function

function chrp_last_error() result(string)
    implicit none

    character(len=1024) :: string
    type(c_ptr) :: c_string

    c_string = chrp_last_error_c()
    string = c_to_f_str(c_string)
end function

subroutine chrp_loglevel(level, status)
    implicit none
    include "generated/cenums.f90"
    integer(kind=kind(CHRP_LOG_LEVEL)) :: level
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_loglevel_c(level)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_set_loglevel(level, status)
    implicit none
    include "generated/cenums.f90"
    integer(kind=kind(CHRP_LOG_LEVEL)), value :: level
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_set_loglevel_c(level)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_logfile(file, status)
    implicit none
    character(len=*), intent(in) :: file
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_logfile_c(f_to_c_str(file))
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_log_stderr(status)
    implicit none

    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_log_stderr_c()
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_open_init_(this, filename, mode, status)
    implicit none
    class(chrp_trajectory) :: this
    character(len=*), intent(in) :: filename
    character(len=*), intent(in) :: mode
    integer, optional :: status
    integer :: status_tmp_

    this%ptr = chrp_open_c(f_to_c_str(filename), f_to_c_str(mode))

    if (.not. c_associated(this%ptr)) then
        status_tmp_ = -1
    else
        status_tmp_ = 0
    end if

    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_trajectory_read(this, frame, status)
    implicit none
    class(chrp_trajectory) :: this
    class(chrp_frame) :: frame
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_trajectory_read_c(this%ptr, frame%ptr)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_trajectory_read_step(this, step, frame, status)
    implicit none
    class(chrp_trajectory) :: this
    integer(kind=c_size_t), value :: step
    class(chrp_frame) :: frame
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_trajectory_read_step_c(this%ptr, step, frame%ptr)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_trajectory_write(this, frame, status)
    implicit none
    class(chrp_trajectory) :: this
    class(chrp_frame), intent(in) :: frame
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_trajectory_write_c(this%ptr, frame%ptr)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_trajectory_set_topology(this, topology, status)
    implicit none
    class(chrp_trajectory) :: this
    class(chrp_topology), intent(in) :: topology
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_trajectory_set_topology_c(this%ptr, topology%ptr)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_trajectory_set_topology_file(this, filename, status)
    implicit none
    class(chrp_trajectory) :: this
    character(len=*), intent(in) :: filename
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_trajectory_set_topology_file_c(this%ptr, f_to_c_str(filename))
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_trajectory_set_cell(this, cell, status)
    implicit none
    class(chrp_trajectory) :: this
    class(chrp_cell), intent(in) :: cell
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_trajectory_set_cell_c(this%ptr, cell%ptr)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_trajectory_nsteps(this, nsteps, status)
    implicit none
    class(chrp_trajectory) :: this
    integer(kind=c_size_t) :: nsteps
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_trajectory_nsteps_c(this%ptr, nsteps)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_trajectory_close(this, status)
    implicit none
    class(chrp_trajectory) :: this
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_trajectory_close_c(this%ptr)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_frame_init_(this, natoms, status)
    implicit none
    class(chrp_frame) :: this
    integer(kind=c_size_t), value :: natoms
    integer, optional :: status
    integer :: status_tmp_

    this%ptr = chrp_frame_c(natoms)

    if (.not. c_associated(this%ptr)) then
        status_tmp_ = -1
    else
        status_tmp_ = 0
    end if

    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_frame_atoms_count(this, natoms, status)
    implicit none
    class(chrp_frame), intent(in) :: this
    integer(kind=c_size_t) :: natoms
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_frame_atoms_count_c(this%ptr, natoms)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_frame_positions(this, data, size, status)
    implicit none
    class(chrp_frame), intent(in) :: this
    real(kind=c_float), dimension(:, :), target :: data
    integer(kind=c_size_t), value :: size
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_frame_positions_c(this%ptr, c_loc(data), size)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_frame_set_positions(this, data, size, status)
    implicit none
    class(chrp_frame) :: this
    real(kind=c_float), dimension(:, :), target :: data
    integer(kind=c_size_t), value :: size
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_frame_set_positions_c(this%ptr, c_loc(data), size)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_frame_velocities(this, data, size, status)
    implicit none
    class(chrp_frame), intent(in) :: this
    real(kind=c_float), dimension(:, :), target :: data
    integer(kind=c_size_t), value :: size
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_frame_velocities_c(this%ptr, c_loc(data), size)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_frame_set_velocities(this, data, size, status)
    implicit none
    class(chrp_frame) :: this
    real(kind=c_float), dimension(:, :), target :: data
    integer(kind=c_size_t), value :: size
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_frame_set_velocities_c(this%ptr, c_loc(data), size)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_frame_has_velocities(this, has_vel, status)
    implicit none
    class(chrp_frame), intent(in) :: this
    logical(kind=c_bool) :: has_vel
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_frame_has_velocities_c(this%ptr, has_vel)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_frame_set_cell(this, cell, status)
    implicit none
    class(chrp_frame) :: this
    class(chrp_cell), intent(in) :: cell
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_frame_set_cell_c(this%ptr, cell%ptr)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_frame_set_topology(this, topology, status)
    implicit none
    class(chrp_frame) :: this
    class(chrp_topology), intent(in) :: topology
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_frame_set_topology_c(this%ptr, topology%ptr)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_frame_step(this, step, status)
    implicit none
    class(chrp_frame), intent(in) :: this
    integer(kind=c_size_t) :: step
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_frame_step_c(this%ptr, step)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_frame_set_step(this, step, status)
    implicit none
    class(chrp_frame) :: this
    integer(kind=c_size_t), value :: step
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_frame_set_step_c(this%ptr, step)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_frame_guess_topology(this, bonds, status)
    implicit none
    class(chrp_frame) :: this
    logical(kind=c_bool), value :: bonds
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_frame_guess_topology_c(this%ptr, bonds)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_frame_free(this, status)
    implicit none
    class(chrp_frame) :: this
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_frame_free_c(this%ptr)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_cell_init_(this, a, b, c, status)
    implicit none
    class(chrp_cell) :: this
    real(kind=c_double), value :: a
    real(kind=c_double), value :: b
    real(kind=c_double), value :: c
    integer, optional :: status
    integer :: status_tmp_

    this%ptr = chrp_cell_c(a, b, c)

    if (.not. c_associated(this%ptr)) then
        status_tmp_ = -1
    else
        status_tmp_ = 0
    end if

    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_cell_triclinic_init_(this, a, b, c, alpha, beta, gamma, status)
    implicit none
    class(chrp_cell) :: this
    real(kind=c_double), value :: a
    real(kind=c_double), value :: b
    real(kind=c_double), value :: c
    real(kind=c_double), value :: alpha
    real(kind=c_double), value :: beta
    real(kind=c_double), value :: gamma
    integer, optional :: status
    integer :: status_tmp_

    this%ptr = chrp_cell_triclinic_c(a, b, c, alpha, beta, gamma)

    if (.not. c_associated(this%ptr)) then
        status_tmp_ = -1
    else
        status_tmp_ = 0
    end if

    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_cell_from_frame_init_(this, frame, status)
    implicit none
    class(chrp_cell) :: this
    class(chrp_frame) :: frame
    integer, optional :: status
    integer :: status_tmp_

    this%ptr = chrp_cell_from_frame_c(frame%ptr)

    if (.not. c_associated(this%ptr)) then
        status_tmp_ = -1
    else
        status_tmp_ = 0
    end if

    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_cell_volume(this, V, status)
    implicit none
    class(chrp_cell), intent(in) :: this
    real(kind=c_double) :: V
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_cell_volume_c(this%ptr, V)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_cell_lengths(this, a, b, c, status)
    implicit none
    class(chrp_cell), intent(in) :: this
    real(kind=c_double) :: a
    real(kind=c_double) :: b
    real(kind=c_double) :: c
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_cell_lengths_c(this%ptr, a, b, c)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_cell_set_lengths(this, a, b, c, status)
    implicit none
    class(chrp_cell) :: this
    real(kind=c_double), value :: a
    real(kind=c_double), value :: b
    real(kind=c_double), value :: c
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_cell_set_lengths_c(this%ptr, a, b, c)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_cell_angles(this, alpha, beta, gamma, status)
    implicit none
    class(chrp_cell), intent(in) :: this
    real(kind=c_double) :: alpha
    real(kind=c_double) :: beta
    real(kind=c_double) :: gamma
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_cell_angles_c(this%ptr, alpha, beta, gamma)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_cell_set_angles(this, alpha, beta, gamma, status)
    implicit none
    class(chrp_cell) :: this
    real(kind=c_double), value :: alpha
    real(kind=c_double), value :: beta
    real(kind=c_double), value :: gamma
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_cell_set_angles_c(this%ptr, alpha, beta, gamma)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_cell_matrix(this, mat, status)
    implicit none
    class(chrp_cell), intent(in) :: this
    real(kind=c_double), dimension(3, 3), target :: mat
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_cell_matrix_c(this%ptr, c_loc(mat))
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_cell_type(this, type, status)
    implicit none
    class(chrp_cell), intent(in) :: this
    include "generated/cenums.f90"
    integer(kind=kind(CHRP_CELL_TYPES)) :: type
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_cell_type_c(this%ptr, type)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_cell_set_type(this, type, status)
    implicit none
    class(chrp_cell) :: this
    include "generated/cenums.f90"
    integer(kind=kind(CHRP_CELL_TYPES)), value :: type
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_cell_set_type_c(this%ptr, type)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_cell_periodicity(this, x, y, z, status)
    implicit none
    class(chrp_cell), intent(in) :: this
    logical(kind=c_bool) :: x
    logical(kind=c_bool) :: y
    logical(kind=c_bool) :: z
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_cell_periodicity_c(this%ptr, x, y, z)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_cell_set_periodicity(this, x, y, z, status)
    implicit none
    class(chrp_cell) :: this
    logical(kind=c_bool), value :: x
    logical(kind=c_bool), value :: y
    logical(kind=c_bool), value :: z
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_cell_set_periodicity_c(this%ptr, x, y, z)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_cell_free(this, status)
    implicit none
    class(chrp_cell) :: this
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_cell_free_c(this%ptr)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_topology_init_(this, status)
    implicit none
    class(chrp_topology) :: this
    integer, optional :: status
    integer :: status_tmp_

    this%ptr = chrp_topology_c()

    if (.not. c_associated(this%ptr)) then
        status_tmp_ = -1
    else
        status_tmp_ = 0
    end if

    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_topology_from_frame_init_(this, frame, status)
    implicit none
    class(chrp_topology) :: this
    class(chrp_frame) :: frame
    integer, optional :: status
    integer :: status_tmp_

    this%ptr = chrp_topology_from_frame_c(frame%ptr)

    if (.not. c_associated(this%ptr)) then
        status_tmp_ = -1
    else
        status_tmp_ = 0
    end if

    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_topology_atoms_count(this, natoms, status)
    implicit none
    class(chrp_topology), intent(in) :: this
    integer(kind=c_size_t) :: natoms
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_topology_atoms_count_c(this%ptr, natoms)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_topology_append(this, atom, status)
    implicit none
    class(chrp_topology) :: this
    class(chrp_atom), intent(in) :: atom
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_topology_append_c(this%ptr, atom%ptr)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_topology_remove(this, i, status)
    implicit none
    class(chrp_topology) :: this
    integer(kind=c_size_t), value :: i
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_topology_remove_c(this%ptr, i)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_topology_isbond(this, i, j, result, status)
    implicit none
    class(chrp_topology), intent(in) :: this
    integer(kind=c_size_t), value :: i
    integer(kind=c_size_t), value :: j
    logical(kind=c_bool) :: result
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_topology_isbond_c(this%ptr, i, j, result)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_topology_isangle(this, i, j, k, result, status)
    implicit none
    class(chrp_topology), intent(in) :: this
    integer(kind=c_size_t), value :: i
    integer(kind=c_size_t), value :: j
    integer(kind=c_size_t), value :: k
    logical(kind=c_bool) :: result
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_topology_isangle_c(this%ptr, i, j, k, result)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_topology_isdihedral(this, i, j, k, m, result, status)
    implicit none
    class(chrp_topology), intent(in) :: this
    integer(kind=c_size_t), value :: i
    integer(kind=c_size_t), value :: j
    integer(kind=c_size_t), value :: k
    integer(kind=c_size_t), value :: m
    logical(kind=c_bool) :: result
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_topology_isdihedral_c(this%ptr, i, j, k, m, result)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_topology_bonds_count(this, nbonds, status)
    implicit none
    class(chrp_topology), intent(in) :: this
    integer(kind=c_size_t) :: nbonds
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_topology_bonds_count_c(this%ptr, nbonds)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_topology_angles_count(this, nangles, status)
    implicit none
    class(chrp_topology), intent(in) :: this
    integer(kind=c_size_t) :: nangles
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_topology_angles_count_c(this%ptr, nangles)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_topology_dihedrals_count(this, ndihedrals, status)
    implicit none
    class(chrp_topology), intent(in) :: this
    integer(kind=c_size_t) :: ndihedrals
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_topology_dihedrals_count_c(this%ptr, ndihedrals)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_topology_bonds(this, data, nbonds, status)
    implicit none
    class(chrp_topology), intent(in) :: this
    integer(kind=c_size_t), dimension(:, :), target :: data
    integer(kind=c_size_t), value :: nbonds
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_topology_bonds_c(this%ptr, c_loc(data), nbonds)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_topology_angles(this, data, nangles, status)
    implicit none
    class(chrp_topology), intent(in) :: this
    integer(kind=c_size_t), dimension(:, :), target :: data
    integer(kind=c_size_t), value :: nangles
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_topology_angles_c(this%ptr, c_loc(data), nangles)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_topology_dihedrals(this, data, ndihedrals, status)
    implicit none
    class(chrp_topology), intent(in) :: this
    integer(kind=c_size_t), dimension(:, :), target :: data
    integer(kind=c_size_t), value :: ndihedrals
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_topology_dihedrals_c(this%ptr, c_loc(data), ndihedrals)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_topology_add_bond(this, i, j, status)
    implicit none
    class(chrp_topology) :: this
    integer(kind=c_size_t), value :: i
    integer(kind=c_size_t), value :: j
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_topology_add_bond_c(this%ptr, i, j)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_topology_remove_bond(this, i, j, status)
    implicit none
    class(chrp_topology) :: this
    integer(kind=c_size_t), value :: i
    integer(kind=c_size_t), value :: j
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_topology_remove_bond_c(this%ptr, i, j)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_topology_free(this, status)
    implicit none
    class(chrp_topology) :: this
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_topology_free_c(this%ptr)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_atom_init_(this, name, status)
    implicit none
    class(chrp_atom) :: this
    character(len=*), intent(in) :: name
    integer, optional :: status
    integer :: status_tmp_

    this%ptr = chrp_atom_c(f_to_c_str(name))

    if (.not. c_associated(this%ptr)) then
        status_tmp_ = -1
    else
        status_tmp_ = 0
    end if

    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_atom_from_frame_init_(this, frame, idx, status)
    implicit none
    class(chrp_atom) :: this
    class(chrp_frame), intent(in) :: frame
    integer(kind=c_size_t), value :: idx
    integer, optional :: status
    integer :: status_tmp_

    this%ptr = chrp_atom_from_frame_c(frame%ptr, idx)

    if (.not. c_associated(this%ptr)) then
        status_tmp_ = -1
    else
        status_tmp_ = 0
    end if

    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_atom_from_topology_init_(this, topology, idx, status)
    implicit none
    class(chrp_atom) :: this
    class(chrp_topology), intent(in) :: topology
    integer(kind=c_size_t), value :: idx
    integer, optional :: status
    integer :: status_tmp_

    this%ptr = chrp_atom_from_topology_c(topology%ptr, idx)

    if (.not. c_associated(this%ptr)) then
        status_tmp_ = -1
    else
        status_tmp_ = 0
    end if

    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_atom_mass(this, mass, status)
    implicit none
    class(chrp_atom), intent(in) :: this
    real(kind=c_float) :: mass
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_atom_mass_c(this%ptr, mass)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_atom_set_mass(this, mass, status)
    implicit none
    class(chrp_atom) :: this
    real(kind=c_float), value :: mass
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_atom_set_mass_c(this%ptr, mass)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_atom_charge(this, charge, status)
    implicit none
    class(chrp_atom), intent(in) :: this
    real(kind=c_float) :: charge
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_atom_charge_c(this%ptr, charge)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_atom_set_charge(this, charge, status)
    implicit none
    class(chrp_atom) :: this
    real(kind=c_float), value :: charge
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_atom_set_charge_c(this%ptr, charge)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_atom_name(this, name, buffsize, status)
    implicit none
    class(chrp_atom), intent(in) :: this
    character(len=*) :: name
    integer(kind=c_size_t), value :: buffsize
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_atom_name_c(this%ptr, name, buffsize)
    if (present(status)) then
        status = status_tmp_
    end if
    name = rm_c_null_in_str(name)
end subroutine

subroutine chrp_atom_set_name(this, name, status)
    implicit none
    class(chrp_atom) :: this
    character(len=*), intent(in) :: name
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_atom_set_name_c(this%ptr, f_to_c_str(name))
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_atom_full_name(this, name, buffsize, status)
    implicit none
    class(chrp_atom), intent(in) :: this
    character(len=*) :: name
    integer(kind=c_size_t), value :: buffsize
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_atom_full_name_c(this%ptr, name, buffsize)
    if (present(status)) then
        status = status_tmp_
    end if
    name = rm_c_null_in_str(name)
end subroutine

subroutine chrp_atom_vdw_radius(this, radius, status)
    implicit none
    class(chrp_atom), intent(in) :: this
    real(kind=c_double) :: radius
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_atom_vdw_radius_c(this%ptr, radius)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_atom_covalent_radius(this, radius, status)
    implicit none
    class(chrp_atom), intent(in) :: this
    real(kind=c_double) :: radius
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_atom_covalent_radius_c(this%ptr, radius)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_atom_atomic_number(this, number, status)
    implicit none
    class(chrp_atom), intent(in) :: this
    integer(kind=c_int) :: number
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_atom_atomic_number_c(this%ptr, number)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine

subroutine chrp_atom_free(this, status)
    implicit none
    class(chrp_atom) :: this
    integer, optional :: status
    integer :: status_tmp_

    status_tmp_ = chrp_atom_free_c(this%ptr)
    if (present(status)) then
        status = status_tmp_
    end if
end subroutine
