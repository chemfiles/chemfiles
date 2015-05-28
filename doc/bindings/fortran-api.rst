Fortran interface
=================

The Fortran interface to Chemharp is built in an object-oriented fashion, using
the Fortran 2003 standard. It make use of the C interface and the ``iso_c_binding``
intrisic module to call the needed functions.

All the functionalities are in the ``chemharp`` module, which should be used in
all the programs using Chemharp. The ``iso_fortran_env`` instrisic module can also
be usefull to set the kind of real and doubles where needed.

The ``chemharp`` module is built around the 5 main types of Chemharp: ``chrp_trajectory``,
``chrp_frame``, ``chrp_cell``, ``chrp_topology``, and ``chrp_atom``. For more
information about these types, please see the :ref:`overview`.

Naming conventions and call conventions
---------------------------------------

All the functions and types have the ``chrp_`` prefix. Except for the ``chrp_strerror``
and ``chrp_last_error`` functions, all the functions take a ``status`` parameter,
which will indicate the status of the operation. It should be 0 if everything
was OK, and can be any other number in case of error.

When creating a variable of one of the Chemharp types, the first routine to be
called should be an initialization routine. It can be either the ``init`` routine
for default initialization, or another routine documented as initializing.

.. code-block:: fortran

    implicit none
    type(chrp_cell) :: cell
    type(chrp_frame) :: frame

    call cell%init(20, 20, 20, 90, 90, 90)
    call frame%init(3)

These initialization function should only be called once. In order to free the
memory asssociated with any Chemharp variable, the ``free`` subroutine should
be called. After a call the the ``free`` subroutine, the ``init`` subroutine
can be called again whithout any memory leak risk. Not initializing Chemharp
variables will lead to segmentations faults.

Error and logging functions
---------------------------

.. f:function:: chrp_strerror(status)

    Get the error message corresponding to an error code.

    :argument integer status: The status code
    :return string strerror: The error message corresponding to the status code

.. f:function:: chrp_last_error()

    Get the last error message.

    :return string strerror: The error message corresponding to the status code

.. f:subroutine:: chrp_loglevel(level[, status])

    Set the current log level to ``level``.

    :paramter integer(kind=kind(CHRP_LOG_LEVEL)): The new logging level
    :optional integer status [optional]: The status code

    The following loggin level are available:

    .. cpp:enum:: CHRP_LOG_LEVEL

        .. cpp:enumerator:: CHRP_LOG_NONE

            Do not log anything

        .. cpp:enumerator:: CHRP_LOG_ERROR

            Only log errors

        .. cpp:enumerator:: CHRP_LOG_WARNING

            Log warnings and erors. This is the default.

        .. cpp:enumerator:: CHRP_LOG_INFO

           Log infos, warnings and errors

        .. cpp:enumerator:: CHRP_LOG_DEBUG

           Log everything


.. f:subroutine:: chrp_logfile(file[, status])

    Redirect the logs to ``file``, overwriting the file if it exists.

    :parameter string file: The path to the log file
    :optional integer status [optional]: The status code

.. f:subroutine:: chrp_log_stderr(status)

    Redirect the logs to the standard error output. This is enabled by default.

    :optional integer status [optional]: The status code

``chrp_trajectory`` type
------------------------

.. f:currentmodule:: chrp_trajectory

.. f:type:: chrp_trajectory

    Wrapping around a C pointer of type ``CHRP_TRAJECTORY*``. The following
    subroutine are available:

    :field subroutine open:
    :field subroutine read:
    :field subroutine read_at:
    :field subroutine write:
    :field subroutine topology:
    :field subroutine topology_file:
    :field subroutine cell:
    :field subroutine nstep:
    :field subroutine close:

    The initialization routine is the ``open`` one, and the memory liberation
    routine is the ``close`` one.

.. f:subroutine:: open(filename, mode, [, status])

    Open a trajectory file.

    :parameter string filename: The path to the trajectory file
    :parameter string mode: The opening ("r" for read or "w" for write) mode for the file.
    :optional integer status [optional]: The status code

.. f:subroutine:: read(frame[, status])

    Read the next step of the trajectory into a frame

    :parameter chrp_frame frame: A frame to fill with the data
    :optional integer status [optional]: The status code

.. f:subroutine:: read_at(step, frame[, status])

    Read a specific step of the trajectory in a frame

    :parameter integer step: The step to read
    :parameter chrp_frame frame: A frame to fill with the data
    :optional integer status [optional]: The status code

.. f:subroutine:: write(frame[, status])

    Write a frame to the trajectory.

    :parameter chrp_frame frame: the frame which will be writen to the file
    :optional integer status [optional]: The status code

.. f:subroutine:: topology(topology[, status])

    Set the topology associated with a trajectory. This topology will be
    used when reading and writing the files, replacing any topology in the
    frames or files.

    :parameter chrp_topology topology: The new topology to use
    :optional integer status [optional]: The status code

.. f:subroutine:: topology_file(filename[, status])

    Set the topology associated with a trajectory by reading the first
    frame of \c filename; and extracting the topology of this frame.

    :parameter string filename: The file to read in order to get the new topology
    :optional integer status [optional]: The status code

.. f:subroutine:: cell(cell[, status])

    Set the unit cell associated with a trajectory. This cell will be
    used when reading and writing the files, replacing any unit cell in the
    frames or files.

    :parameter chrp_cell cell: The new cell to use
    :optional integer status [optional]: The status code

.. f:subroutine:: nsteps(nsteps[, status])

    Get the number of steps (the number of frames) in a trajectory.

    :parameter integer nsteps: This will contain the number of steps
    :optional integer status [optional]: The status code

.. f:subroutine:: close(status)

    Close a trajectory file, flush any buffer content to the hard drive, and
    free the associated memory

    :optional integer status [optional]: The status code

``chrp_frame`` type
-------------------

.. f:currentmodule:: chrp_frame

.. f:type:: chrp_frame

    Wrapping around a C pointer of type ``CHRP_FRAME*``. The following
    subroutine are available:

    :field subroutine init:
    :field subroutine size:
    :field subroutine positions:
    :field subroutine positions_set:
    :field subroutine velocities:
    :field subroutine velocities_set:
    :field subroutine has_velocities:
    :field subroutine cell_set:
    :field subroutine topology_set:
    :field subroutine step:
    :field subroutine step_set:
    :field subroutine free:

.. f:subroutine:: init(natoms[, status])

    Create an empty frame with initial capacity of ``natoms``. It will be
    resized by the library as needed.

    :parameter integer natoms: the size of the wanted frame
    :optional integer status [optional]: The status code

.. f:subroutine:: size(natoms[, status])

    Get the frame size, i.e. the current number of atoms

    :parameter integer natoms: the number of atoms in the frame
    :optional integer status [optional]: The status code

.. f:subroutine:: positions(data, size[, status])

    Get the positions from a frame

    :parameter real data [dimension(3, N)]: A 3xN float array to be filled with the data
    :parameter integer size: The array size (N).
    :optional integer status [optional]: The status code

.. f:subroutine:: positions_set(data, size[, status])

    Set the positions of a frame

    :parameter real data [dimension(3, N)]: A 3xN float array containing the positions in column-major order.
    :parameter integer size: The array size (N).

    :optional integer status [optional]: The status code

.. f:subroutine:: velocities(data, size[, status])

    Get the velocities from a frame, if they exists

    :parameter real data [dimension(3, N)]: A 3xN float array to be filled with the data
    :parameter integer size: The array size (N).
    :optional integer status [optional]: The status code

.. f:subroutine:: velocities_set(data, size[, status])

    Set the velocities of a frame.

    :parameter real data [dimension(3, N)]: A 3xN float array containing the velocities in column-major order.
    :parameter integer size: The array size (N).
    :optional integer status [optional]: The status code

.. f:subroutine:: has_velocities(has_vel[, status])

    Check if a frame has velocity information.

    :parameter logical has_vel: ``.true.`` if the frame has velocities, ``.false.`` otherwise.
    :optional integer status [optional]: The status code

.. f:subroutine:: cell_set(cell[, status])

    Set the UnitCell of a Frame.

    :parameter chrp_cell cell: The new unit cell
    :optional integer status [optional]: The status code

.. f:subroutine:: topology_set(topology[, status])

    Set the Topology of a Frame.

    :parameter chrp_topology topology: The new topology
    :optional integer status [optional]: The status code

.. f:subroutine:: step(step[, status])

    Get the Frame step, i.e. the frame number in the trajectory

    :parameter integer step: This will contains the step number
    :optional integer status [optional]: The status code

.. f:subroutine:: step_set(step[, status])

    Set the Frame step.

    :parameter integer step: The new frame step
    :optional integer status [optional]: The status code

.. f:subroutine:: guess_topology(bonds[, status])

    Try to guess the bonds, angles and dihedrals in the system. If ``bonds``
    is ``.true.``, guess everything; else only guess the angles and dihedrals from
    the bond list.

    :parameter logical bonds: Should we recompute the bonds from the positions or not ?
    :optional integer status [optional]: The status code

.. f:subroutine:: free(status)

    Destroy a frame, and free the associated memory

    :optional integer status [optional]: The status code

``chrp_cell`` type
------------------

.. f:currentmodule:: chrp_cell

.. f:type:: chrp_cell

    Wrapping around a C pointer of type ``CHRP_CELL*``. The following
    subroutine are available:

    :field subroutine init:
    :field subroutine from_frame:
    :field subroutine lengths:
    :field subroutine lengths_set:
    :field subroutine angles:
    :field subroutine angles_set:
    :field subroutine matrix:
    :field subroutine type:
    :field subroutine type_set:
    :field subroutine periodicity:
    :field subroutine periodicity_set:
    :field subroutine free:

    The initialization routine are ``init`` and ``from_frame``.


.. f:subroutine:: init(a, b, c, alpha, beta, gamma[, status])

    Create an UnitCell from the three lenghts and the three angles

    :parameter real a: the a cell length, in angstroms
    :parameter real b: the b cell length, in angstroms
    :parameter real c: the c cell length, in angstroms
    :parameter real alpha: the alpha angles, in degrees
    :parameter real beta: the beta angles, in degrees
    :parameter real gamma: the gamma angles, in degrees
    :optional integer status [optional]: The status code

.. f:subroutine:: from_frame_init_(frame[, status])

    Get the UnitCell from a frame

    :parameter chrp_frame frame: the frame
    :optional integer status [optional]: The status code

.. f:subroutine:: lengths(a, b, c[, status])

    Get the cell lenghts.

    :parameter real a: the a cell length, in angstroms
    :parameter real b: the b cell length, in angstroms
    :parameter real c: the c cell length, in angstroms
    :optional integer status [optional]: The status code

.. f:subroutine:: lengths_set(a, b, c[, status])

    Set the unit cell lenghts.

    :parameter real a: the new a cell length, in angstroms
    :parameter real b: the new b cell length, in angstroms
    :parameter real c: the new c cell length, in angstroms
    :optional integer status [optional]: The status code

.. f:subroutine:: angles(alpha, beta, gamma[, status])

    Get the cell angles, in degrees.

    :parameter real alpha: the alpha angles, in degrees
    :parameter real beta: the beta angles, in degrees
    :parameter real gamma: the gamma angles, in degrees
    :optional integer status [optional]: The status code

.. f:subroutine:: angles_set(alpha, beta, gamma[, status])

    Set the cell angles, in degrees

    :parameter real alpha: the new alpha angles, in degrees
    :parameter real beta: the new beta angles, in degrees
    :parameter real gamma: the new gamma angles, in degrees
    :optional integer status [optional]: The status code

.. f:subroutine:: matrix(mat[, status])

    Get the unit cell matricial representation.

    :parameter real mat [dimension(3, 3)]: the matrix to fill.
    :optional integer status [optional]: The status code

.. f:subroutine:: type(type[, status])

    Get the cell type

    :parameter integer type [kind=kind(CHRP_CELL_TYPES)]: the type of the cell
    :optional integer status [optional]: The status code

    .. cpp:enum:: CHRP_CELL_TYPES

        .. cpp:enumerator:: CHRP_CELL_ORTHOROMBIC

            The three angles are 90°

        .. cpp:enumerator:: CHRP_CELL_TRICLINIC

            The three angles may not be 90°

        .. cpp:enumerator:: CHRP_CELL_INFINITE

            Cell type when there is no periodic boundary conditions

.. f:subroutine:: type_set(type[, status])

    Set the cell type

    :parameter integer type [kind=kind(CHRP_CELL_TYPES)]: the new type of the cell
    :optional integer status [optional]: The status code

.. f:subroutine:: periodicity(x, y, z[, status])

    Get the cell periodic boundary conditions along the three axis

    :parameter logical x: the periodicity of the cell along the x axis.
    :parameter logical y: the periodicity of the cell along the y axis.
    :parameter logical z: the periodicity of the cell along the z axis.
    :optional integer status [optional]: The status code

.. f:subroutine:: periodicity_set(x, y, z[, status])

    Set the cell periodic boundary conditions along the three axis

    :parameter logical x: the new periodicity of the cell along the x axis.
    :parameter logical y: the new periodicity of the cell along the y axis.
    :parameter logical z: the new periodicity of the cell along the z axis.
    :optional integer status [optional]: The status code

.. f:subroutine:: free(status)

    Destroy an unit cell, and free the associated memory

    :optional integer status [optional]: The status code

``chrp_topology`` type
----------------------

.. f:currentmodule:: chrp_topology

.. f:type:: chrp_topology

    Wrapping around a C pointer of type ``CHRP_TOPOLOGY*``. The following
    subroutine are available:

    :field subroutine init:
    :field subroutine from_frame:
    :field subroutine size:
    :field subroutine guess:
    :field subroutine append:
    :field subroutine remove:
    :field subroutine isbond:
    :field subroutine isangle:
    :field subroutine isdihedral:
    :field subroutine bonds_count:
    :field subroutine angles_count:
    :field subroutine dihedrals_count:
    :field subroutine bonds:
    :field subroutine angles:
    :field subroutine dihedrals:
    :field subroutine add_bond:
    :field subroutine remove_bond:
    :field subroutine free:

    The initialization routine are ``init`` and ``from_frame``.

.. f:subroutine:: init(status)

    Create a new empty topology

    :optional integer status [optional]: The status code

.. f:subroutine:: from_frame(frame[, status])

    Extract the topology from a frame

    :parameter chrp_frame frame: The frame
    :optional integer status [optional]: The status code

.. f:subroutine:: size(natoms[, status])

    Get the topology size, i.e. the current number of atoms

    :parameter integer natoms: Will contain the number of atoms in the frame
    :optional integer status [optional]: The status code

.. f:subroutine:: append(atom[, status])

    Add an atom to a topology

    :parameter chrp_atom atom: The atom to be added
    :optional integer status [optional]: The status code


.. f:subroutine:: remove(i[, status])

    Remove an atom from a topology by index

    :parameter integer i: The atomic index
    :optional integer status [optional]: The status code

.. f:subroutine:: isbond(i, j, result[, status])

    Tell if the atoms ``i`` and ``j`` are bonded together

    :parameter integer i: The atomic index of the first atom
    :parameter integer j: The atomic index of the second atom
    :parameter logical result: ``.true.`` if the atoms are bonded, ``.false.`` otherwise
    :optional integer status [optional]: The status code

.. f:subroutine:: isangle(i, j, k, result[, status])

    Tell if the atoms ``i``, ``j`` and ``k`` constitues an angle

    :parameter integer i: The atomic index of the first atom
    :parameter integer j: The atomic index of the second atom
    :parameter integer k: The atomic index of the third atom
    :parameter logical result: ``.true.`` if the atoms constitues an angle, ``.false.`` otherwise
    :optional integer status [optional]: The status code

.. f:subroutine:: isdihedral(i, j, k, m, result[, status])

    Tell if the atoms ``i``, ``j``, ``k`` and ``m`` constitues a dihedral angle

    :parameter integer i: The atomic index of the first atom
    :parameter integer j: The atomic index of the second atom
    :parameter integer k: The atomic index of the third atom
    :parameter integer m: The atomic index of the fourth atom
    :parameter logical result: ``.true.`` if the atoms constitues a dihedral angle, ``.false.`` otherwise
    :optional integer status [optional]: The status code

.. f:subroutine:: bonds_count(nbonds[, status])

    Get the number of bonds in the system

    :parameter integer nbonds: After the call, contains the number of bond
    :optional integer status [optional]: The status code

.. f:subroutine:: angles_count(nangles[, status])

    Get the number of angles in the system

    :parameter integer nangles: After the call, contains the number of angles
    :optional integer status [optional]: The status code

.. f:subroutine:: dihedrals_count(ndihedrals[, status])

    Get the number of dihedral angles in the system

    :parameter integer ndihedrals: After the call, contains the number of dihedral angles
    :optional integer status [optional]: The status code

.. f:subroutine:: bonds(data, nbonds[, status])

    Get the bonds in the system

    :parameter integer data [dimension(2, nbonds)]: A 2x ``nbonds`` array to be
                                            filled with the bonds in the system
    :parameter integer nbonds: The size of the array. This should equal the value
                                given by the ``chrp_topology%bonds_count`` function
    :optional integer status [optional]: The status code

.. f:subroutine:: angles(data, nangles[, status])

    Get the angles in the system

    :parameter integer data [dimension(3, nangles)]: A 3x ``nangles`` array to be
                                            filled with the angles in the system
    :parameter integer nangles: The size of the array. This should equal the
                        value give by the ``chrp_topology%angles_count`` function
    :optional integer status [optional]: The status code

.. f:subroutine:: dihedrals(data, ndihedrals[, status])

    Get the dihedral angles in the system

    :parameter integer data [dimension(4, ndihedrals)]: A 4x ``ndihedrals`` array
                            to be filled with the dihedral angles in the system
    :parameter integer ndihedrals: The size of the array. This should equal the
                    value give by the ``chrp_topology%dihedrals_count`` function
    :optional integer status [optional]: The status code

.. f:subroutine:: add_bond(i, j[, status])

    Add a bond between the atoms ``i`` and ``j`` in the system

    :parameter integer i: The atomic index of the first atom
    :parameter integer j: The atomic index of the second atom
    :optional integer status [optional]: The status code

.. f:subroutine:: remove_bond(i, j[, status])

    Remove any existing bond between the atoms ``i`` and ``j`` in the system

    :parameter integer i: The atomic index of the first atom
    :parameter integer j: The atomic index of the second atom
    :optional integer status [optional]: The status code

.. f:subroutine:: free(status)

    Destroy a topology, and free the associated memory

    :optional integer status [optional]: The status code

``chrp_atom`` type
------------------

.. f:currentmodule:: chrp_atom

.. f:type:: chrp_atom

    Wrapping around a C pointer of type ``CHRP_ATOM*``. The following
    subroutine are available:

    :field subroutine init:
    :field subroutine from_frame:
    :field subroutine from_topology:
    :field subroutine mass:
    :field subroutine mass_set:
    :field subroutine charge:
    :field subroutine charge_set:
    :field subroutine name:
    :field subroutine name_set:
    :field subroutine full_name:
    :field subroutine vdw_radius:
    :field subroutine covalent_radius:
    :field subroutine atomic_number:
    :field subroutine free:

    The initialization routine are ``init``, ``from_frame`` and ``from_topology``.

.. f:subroutine:: init(name[, status])

    Create an atom from an atomic name

    :parameter string name: The new atom name
    :optional integer status [optional]: The status code

.. f:subroutine:: from_frame(frame, idx[, status])

    Get a specific atom from a frame

    :parameter chrp_frame frame: The frame
    :parameter integer idx: The atom index in the frame
    :optional integer status [optional]: The status code

.. f:subroutine:: from_topology(topology, idx[, status])

    Get a specific atom from a topology

    :parameter chrp_topology topology: The topology
    :parameter integer idx: The atom index in the topology
    :optional integer status [optional]: The status code

.. f:subroutine:: mass(mass[, status])

    Get the mass of an atom, in atomic mass units

    :parameter real mass: The atom mass
    :optional integer status [optional]: The status code

.. f:subroutine:: mass_set(mass[, status])

    Set the mass of an atom, in atomic mass units

    :parameter real mass: The new atom mass
    :optional integer status [optional]: The status code

.. f:subroutine:: charge(charge[, status])

    Get the charge of an atom, in number of the electron charge e

    :parameter real charge: The atom charge
    :optional integer status [optional]: The status code

.. f:subroutine:: charge_set(charge[, status])

    Set the charge of an atom, in number of the electron charge e

    :parameter real charge: The new atom charge
    :optional integer status [optional]: The status code

.. f:subroutine:: name(name, buffsize[, status])

    Get the name of an atom

    :parameter string name: A string buffer to be filled with the name
    :parameter buffsize: The lenght of the string ``name``
    :optional integer status [optional]: The status code

.. f:subroutine:: name_set(name[, status])

    Set the name of an atom

    :parameter string name: A string containing the new name
    :optional integer status [optional]: The status code

.. f:subroutine:: full_name(name, buffsize[, status])

    Try to get the full name of an atom from the short name

    :parameter string name: A string buffer to be filled with the name
    :parameter buffsize: The lenght of the string ``name``
    :optional integer status [optional]: The status code

.. f:subroutine:: vdw_radius(radius[, status])

    Try to get the Van der Waals radius of an atom from the short name

    :parameter real radius: The Van der Waals radius of the atom or -1 if no value could be found.
    :optional integer status [optional]: The status code

.. f:subroutine:: covalent_radius(radius[, status])

    Try to get the covalent radius of an atom from the short name

    :parameter real radius: The covalent radius of the atom or -1 if no value could be found.
    :optional integer status [optional]: The status code

.. f:subroutine:: atomic_number(number[, status])

    Try to get the atomic number of an atom from the short name

    :parameter integer number: The atomic number, or -1 if no value could be found.
    :optional integer status [optional]: The status code

.. f:subroutine:: free(status)

    Destroy an atom, and free the associated memory

    :optional integer status [optional]: The status code
