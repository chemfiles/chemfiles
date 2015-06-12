C interface
===========

The C interface is define in the ``chemharp.h`` header, which should be included in
all the programs using Chemharp. All the functions and enums have a ``chrp_``
prefix indicating the provenance of the functions. The types are defined as
opaque pointer types, in all caps. The following types are defined:

* :ref:`CHRP_TRAJECTORY <capi-trajectory>` maps to the :ref:`Trajectory <overview-trajectory>` class;
* :ref:`CHRP_FRAME <capi-frame>` maps to the :ref:`Frame  <overview-frame>` class;
* :ref:`CHRP_ATOM <capi-atom>` maps to the :ref:`Atom  <overview-atom>` class;
* :ref:`CHRP_CELL <capi-cell>` maps to the :ref:`UnitCell  <overview-cell>` class;
* :ref:`CHRP_TOPOLOGY <capi-topology>` maps to the :ref:`Topology  <overview-topology>` class.

The user is reponsible for memory management when using these types. Constructors
functions (functions returning pointers to types defined above) return freshly
allocated memory, and calling the ``chrp_*_free`` functions return the corresponding
memory to the operating system.

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

.. _capi-trajectory:

Function manipulating ``CHRP_TRAJECTORY``
-----------------------------------------

The Trajectory type is the main entry point when using Chemharp. A trajectory
behave a bit like a ``FILE*`` pointer, and the ``chrp_close`` free the memory
associated with the file.

.. doxygenfunction:: chrp_open

.. doxygenfunction:: chrp_trajectory_read

.. doxygenfunction:: chrp_trajectory_read_step

.. doxygenfunction:: chrp_trajectory_write

.. doxygenfunction:: chrp_trajectory_set_topology

.. doxygenfunction:: chrp_trajectory_set_topology_file

.. doxygenfunction:: chrp_trajectory_nsteps

.. doxygenfunction:: chrp_trajectory_close

.. _capi-frame:

Function manipulating ``CHRP_FRAME``
------------------------------------

.. doxygenfunction:: chrp_frame

.. doxygenfunction:: chrp_frame_size

.. doxygenfunction:: chrp_frame_positions

.. doxygenfunction:: chrp_frame_set_positions

.. doxygenfunction:: chrp_frame_has_velocities

.. doxygenfunction:: chrp_frame_velocities

.. doxygenfunction:: chrp_frame_set_velocities

.. doxygenfunction:: chrp_frame_set_cell

.. doxygenfunction:: chrp_frame_set_topology

.. doxygenfunction:: chrp_frame_step

.. doxygenfunction:: chrp_frame_set_step

.. doxygenfunction:: chrp_frame_guess_topology

.. doxygenfunction:: chrp_frame_free

.. _capi-cell:

Function manipulating ``CHRP_CELL``
-----------------------------------

.. doxygenfunction:: chrp_cell

.. doxygenfunction:: chrp_cell_from_frame

.. doxygenfunction:: chrp_cell_lengths

.. doxygenfunction:: chrp_cell_set_lengths

.. doxygenfunction:: chrp_cell_angles

.. doxygenfunction:: chrp_cell_set_angles

.. doxygenfunction:: chrp_cell_matrix

.. doxygenenum:: CHRP_CELL_TYPES

.. doxygenfunction:: chrp_cell_type

.. doxygenfunction:: chrp_cell_set_type

.. doxygenfunction:: chrp_cell_periodicity

.. doxygenfunction:: chrp_cell_set_periodicity

.. doxygenfunction:: chrp_cell_free

.. _capi-topology:

Function manipulating ``CHRP_TOPOLOGY``
---------------------------------------

.. doxygenfunction:: chrp_topology

.. doxygenfunction:: chrp_topology_from_frame

.. doxygenfunction:: chrp_topology_size

.. doxygenfunction:: chrp_topology_append

.. doxygenfunction:: chrp_topology_remove

.. doxygenfunction:: chrp_topology_isbond

.. doxygenfunction:: chrp_topology_isangle

.. doxygenfunction:: chrp_topology_isdihedral

.. doxygenfunction:: chrp_topology_bonds_count

.. doxygenfunction:: chrp_topology_angles_count

.. doxygenfunction:: chrp_topology_dihedrals_count

.. doxygenfunction:: chrp_topology_bonds

.. doxygenfunction:: chrp_topology_angles

.. doxygenfunction:: chrp_topology_dihedrals

.. doxygenfunction:: chrp_topology_add_bond

.. doxygenfunction:: chrp_topology_remove_bond

.. doxygenfunction:: chrp_topology_free

.. _capi-atom:

Function manipulating ``CHRP_ATOM``
-----------------------------------

.. doxygenfunction:: chrp_atom

.. doxygenfunction:: chrp_atom_from_frame

.. doxygenfunction:: chrp_atom_from_topology

.. doxygenfunction:: chrp_atom_mass

.. doxygenfunction:: chrp_atom_set_mass

.. doxygenfunction:: chrp_atom_charge

.. doxygenfunction:: chrp_atom_set_charge

.. doxygenfunction:: chrp_atom_name

.. doxygenfunction:: chrp_atom_set_name

.. doxygenfunction:: chrp_atom_full_name

.. doxygenfunction:: chrp_atom_vdw_radius

.. doxygenfunction:: chrp_atom_covalent_radius

.. doxygenfunction:: chrp_atom_atomic_number

.. doxygenfunction:: chrp_atom_free
