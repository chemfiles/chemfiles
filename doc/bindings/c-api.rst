C API
=====

The C API is define in the ``chemharp.h`` header, which should be included in
all the programs using Chemharp. All the functions and enums have a ``chrp_``
prefix indicating the provenance of the functions. The types are defined as
opaque pointer types, in all caps. The following types are defined:

* ``CHRP_TRAJECTORY`` maps to the ``harp::Trajectory`` class;
* ``CHRP_FRAME`` maps to the ``harp::Frame`` class;
* ``CHRP_ATOM`` maps to the ``harp::Atom`` class;
* ``CHRP_CELL`` maps to the ``harp::UnitCell`` class;
* ``CHRP_TOPOLOGY`` maps to the ``harp::Topology`` class.

Functions for errors handling
-----------------------------

Apart from the *constructor* functions (the functions returning pointers to the
types defined above); all the functions return a status code, which is 0 if
nothing went wrong, and another value in case of error. The following function
allow for error handling from the C side.

.. doxygenfunction:: chrp_strerror

.. doxygenfunction:: chrp_last_error

.. doxygenenum:: CHRP_LOG_LEVEL

.. doxygenfunction:: chrp_loglevel

.. doxygenfunction:: chrp_logfile

.. doxygenfunction:: chrp_log_stderr


Function manipulating ``CHRP_TRAJECTORY``
-----------------------------------------

.. doxygenfunction:: chrp_open

.. doxygenfunction:: chrp_read_step

.. doxygenfunction:: chrp_read_next_step

.. doxygenfunction:: chrp_write_step

.. doxygenfunction:: chrp_close

Function manipulating ``CHRP_FRAME``
------------------------------------

.. doxygenfunction:: chrp_frame

.. doxygenfunction:: chrp_frame_size

.. doxygenfunction:: chrp_frame_positions

.. doxygenfunction:: chrp_frame_positions_set

.. doxygenfunction:: chrp_frame_velocities

.. doxygenfunction:: chrp_frame_velocities_set

.. doxygenfunction:: chrp_frame_free

Function manipulating ``CHRP_CELL``
-----------------------------------

.. doxygenfunction:: chrp_cell

.. doxygenfunction:: chrp_cell_lengths

.. doxygenfunction:: chrp_cell_lengths_set

.. doxygenfunction:: chrp_cell_angles

.. doxygenfunction:: chrp_cell_angles_set

.. doxygenfunction:: chrp_cell_matrix

.. doxygenenum:: CHRP_CELL_TYPES

.. doxygenfunction:: chrp_cell_type

.. doxygenfunction:: chrp_cell_type_set

.. doxygenfunction:: chrp_cell_periodicity

.. doxygenfunction:: chrp_cell_periodicity_set

.. doxygenfunction:: chrp_cell_free


Function manipulating ``CHRP_TOPOLOGY``
---------------------------------------

.. doxygenfunction:: chrp_topology

.. doxygenfunction:: chrp_topology_is_bond

.. doxygenfunction:: chrp_topology_is_angle

.. doxygenfunction:: chrp_topology_is_dihedral

.. doxygenfunction:: chrp_topology_bonds_count

.. doxygenfunction:: chrp_topology_angles_count

.. doxygenfunction:: chrp_topology_dihedrals_count

.. doxygenfunction:: chrp_topology_bonds

.. doxygenfunction:: chrp_topology_angles

.. doxygenfunction:: chrp_topology_dihedrals

.. doxygenfunction:: chrp_topology_add_bond

.. doxygenfunction:: chrp_topology_delete_bond

.. doxygenfunction:: chrp_topology_free

Function manipulating ``CHRP_ATOM``
-----------------------------------

.. doxygenfunction:: chrp_atom

.. doxygenfunction:: chrp_topology_atom

.. doxygenfunction:: chrp_atom_mass

.. doxygenfunction:: chrp_atom_mass_set

.. doxygenfunction:: chrp_atom_charge

.. doxygenfunction:: chrp_atom_charge_set

.. doxygenfunction:: chrp_atom_name

.. doxygenfunction:: chrp_atom_name_set

.. doxygenfunction:: chrp_atom_free
