.. _julia-api:

Julia interface
===============

The `Julia`_ interface to Chemharp wrap around the C interface providing a Julian API.
All the functionalities are in the ``Chemharp`` module, which can be imported by the
``using Chemharp`` expression. The ``Chemharp`` module is built around the 5 main
types of Chemharp: ``Trajectory``, ``Frame``, ``UnitCell``, ``Topology``, and
``Atom``. For more information about these types, please see the :ref:`overview`.

The julia interface only support Julia 0.4, but few work would be needed to also
support the 0.3 version. You can install this interface by running

.. code-block:: julia

    julia> Pkg.clone("https://github.com/Luthaf/Chemharp.jl")

    julia> Pkg.build("Chemharp")

.. _Julia: http://julialang.org/

Error and logging functions
---------------------------

These jl:functions are not exported, and should be called by there fully qualified name:

.. code-block:: julia

    Chemharp.last_error()
    Chemharp.loglevel(Chemharp.ERROR)

.. jl:function:: Chemharp.last_error()

    Get the last error message.

.. jl:function:: Chemharp.loglevel(level)

    Set the current log level to ``level``. The following loggin level are available:

    - ``Chemharp.NONE`` : Do not log anything;
    - ``Chemharp.ERROR`` : Only log errors;
    - ``Chemharp.WARNING`` : Log warnings and erors. This is the default;
    - ``Chemharp.INFO`` : Log infos, warnings and errors;
    - ``Chemharp.DEBUG`` : Log everything.

.. jl:function:: Chemharp.logfile(file)

    Redirect the logs to ``file``, overwriting the file if it exists.

.. jl:function:: Chemharp.log_to_stderr()

    Redirect the logs to the standard error output. This is enabled by default.


``Trajectory`` type and associated functions
--------------------------------------------

A trajectory is a file containing informations about the positions of particles
during a simulation, and sometimes other quantities.

.. jl:function:: Trajectory(filename, mode)

    Open a trajectory file.

    :parameter string filename: The path to the trajectory file
    :parameter string mode: The opening mode: "r" for read, "w" for write and  "a" for append.

.. jl:function:: read(trajectory::Trajectory) -> Frame

    Read the next step of the trajectory, and return the corresponding Frame.

.. jl:function:: read!(trajectory::Trajectory, frame::Frame)

    Read the next step of the trajectory into a pre-existing frame.

.. jl:function:: read_step(trajectory::Trajectory, step) -> Frame

    Read at the specific step ``step`` of the trajectory, and return the
    corresponding Frame.

.. jl:function:: read_step(trajectory::Trajectory, step, frame::Frame)

    Read at the specific step ``step`` of the trajectory into a pre-existing frame.

.. jl:function:: write(trajectory::Trajectory, frame::Frame)

    Write a frame to the trajectory.

.. jl:function:: set_topology!(trajectory::Trajectory, topology::Topology)

    Set the topology associated with a trajectory. This topology will be
    used when reading and writing the files, replacing any topology in the
    frames or files.

.. jl:function:: set_topology!(trajectory::Trajectory, filename)

    Set the topology associated with a trajectory by reading the first
    frame of ``filename``; and extracting the topology of this frame.

.. jl:function:: set_cell!(trajectory::Trajectory, cell::UnitCell)

    Set the unit cell associated with a trajectory. This cell will be
    used when reading and writing the files, replacing any unit cell in the
    frames or files.

.. jl:function:: nsteps(trajectory::Trajectory) -> Integer

    Get the number of steps (the number of frames) in a trajectory.

.. jl:function:: close(status)

    Close a trajectory file, flush any buffer content to the hard drive, and
    free the associated memory.


``Frame`` type and associated functions
---------------------------------------

The Frame class hold all data from a simulation, and can be constructed either from a
Trajectory while reading a file, or directly.

.. jl:function:: Frame(natoms = 0)

    Create an empty frame with initial capacity of ``natoms``. It will be resized by
    the library as needed.

.. jl:function:: size(frame::Frame)

    Get the frame size, i.e. the current number of atoms

.. jl:function:: natoms(frame::Frame)

    Get the frame size, i.e. the current number of atoms

.. jl:function:: positions!(frame::Frame, data::Array{Float32, 2})

    Get the positions from a frame. Data is a pre-allocated array of size 3xN float
    array to be filled with the positions.

.. jl:function:: positions(frame::Frame) -> Array{Float32, 2}

    Get the positions from a frame. The result will be allocated on the fly.

.. jl:function:: set_positions!(frame::Frame, data::Array{Float32, 2})

    Set the positions of a frame to ``data``. ``data`` should be a 3xN float array
    containing the positions in column-major order.

.. jl:function:: velocities!(frame::Frame, data::Array{Float32, 2})

    Get the velocities from a frame, if they exists. Data is a pre-allocated array of
    size 3xN float array to be filled with the velocities.

.. jl:function:: velocities(frame::Frame) -> Array{Float32, 2}

    Get the velocities from a frame. The result will be allocated on the fly.

.. jl:function:: set_velocities!(frame::Frame, data::Array{Float32, 2})

    Set the velocities of a frame.  to ``data``. ``data`` should be a 3xN float array
    containing the positions in column-major order.

.. jl:function:: has_velocities(frame::Frame) -> Bool

    Check if a frame has velocity information.

.. jl:function:: set_cell!(frame::Frame, cell::UnitCell)

    Set the UnitCell of a Frame.

.. jl:function:: set_topology!(frame::Frame, topology::Topology)

    Set the Topology of a Frame.

.. jl:function:: step(frame::Frame) -> Integer

    Get the Frame step, i.e. the frame number in the trajectory.

.. jl:function:: set_step!(frame::Frame, step)

    Set the Frame step to ``step``.

.. jl:function:: guess_topology!(frame::Frame,  bonds::Bool=true)

    Try to guess the bonds, angles and dihedrals in the system. If ``bonds``
    is ``true``, guess everything; else only guess the angles and dihedrals from
    the bond list.

``UnitCell`` type and associated function
-----------------------------------------

An UnitCell represent the bounding box of the simulation. It is represented by three
base vectors of lengthes ``a``, ``b`` and ``c``; and the angles between these vectors
``alpha``, ``beta`` and ``gamma``.

.. jl:function:: UnitCell(a, b, c, alpha=90, beta=90, gamma=90)

    Create an UnitCell from the three lenghts and the three angles.

.. jl:function:: UnitCell(frame::Frame)

    Get a copy of the UnitCell of a frame.

.. jl:function:: lengths(cell::UnitCell) -> (a, b, c)

    Get the three cell lenghts ``a``, ``b`` and ``c``; in angstroms.

.. jl:function:: set_lengths!(cell::UnitCell, a, b, c)

    Set the unit cell lenghts to ``a``, ``b`` and ``c``; in angstroms.

.. jl:function:: angles(cell::UnitCell) -> (alpha, beta, gamma)

    Get the cell angles, in degrees.

.. jl:function:: set_angles!(cell::UnitCell, alpha, beta, gamma)

    Set the cell angles to ``alpha``, ``beta`` and ``gamma``, in degrees.

.. jl:function:: matrix(cell::UnitCell)

    Get the unit cell matricial representation, i.e. the representation of the three
    base vectors arranged as:

    .. code-block:: sh

        | a_x b_x c_x |
        |  0  b_y c_y |
        |  0   0  c_z |

.. jl:function:: type(cell::UnitCell)

    Get the cell type. The following cell types are defined:

    - ``Chemharp.ORTHOROMBIC`` : The three angles are 90°
    - ``Chemharp.TRICLINIC`` : The three angles may not be 90°
    - ``Chemharp.INFINITE`` : Cell type when there is no periodic boundary conditions

.. jl:function:: set_type!(cell::UnitCell, celltype)

    Set the cell type to ``celltype``.

.. jl:function:: periodicity(cell::UnitCell) -> (x, y, z)

    Get the cell periodic boundary conditions along the three axis.

.. jl:function:: set_periodicity!(cell::UnitCell, x, y, z)

    Set the cell periodic boundary conditions along the three axis.


``Topology`` type and associated function
-----------------------------------------

The Topology class hold data about which atoms are linked together to form bonds,
angles, *etc.* in a Frame. The atoms in a topology are represented by they index in
the associated Frame.

.. jl:function:: Topology()

    Create an empty Topology.

.. jl:function:: Topology(frame::Frame)

    Extract the topology from a frame.

.. jl:function:: size(topology::Topology)

    Get the topology size, i.e. the current number of atoms.

.. jl:function:: natoms(topology::Topology)

    Get the topology size, i.e. the current number of atoms.

.. jl:function:: push!(topology::Topology, atom::Atom)

    Add an atom at the end of a topology.

.. jl:function:: remove!(topology::Topology, i)

    Remove an atom from a topology by index.

.. jl:function:: isbond(topology::Topology, i, j) -> Bool

    Tell if the atoms ``i`` and ``j`` are bonded together.

.. jl:function:: isangle(topology::Topology, i, j, k) -> Bool

    Tell if the atoms ``i``, ``j`` and ``k`` constitues an angle.

.. jl:function:: isdihedral(topology::Topology, i, j, k, m) -> Bool

    Tell if the atoms ``i``, ``j``, ``k`` and ``m`` constitues a dihedral angle.

.. jl:function:: nbonds(topology::Topology) -> Integer

    Get the number of bonds in the system.

.. jl:function:: nangles(topology::Topology) -> Integer

    Get the number of angles in the system.

.. jl:function:: ndihedrals(topology::Topology) -> Integer

    Get the number of dihedral angles in the system.

.. jl:function:: bonds(topology::Topology) -> Array{UInt, 2}

    Get the bonds in the system, arranged in a 2x ``nbonds`` array.

.. jl:function:: angles(topology::Topology) -> Array{UInt, 2}

    Get the angles in the system, arranges as a 3x ``nangles`` array.

.. jl:function:: dihedrals(topology::Topology) -> Array{UInt, 2}

    Get the dihedral angles in the system, arranged as a 4x ``ndihedrals`` array.

.. jl:function:: add_bond!(topology::Topology, i, j)

    Add a bond between the atoms ``i`` and ``j`` in the system.

.. jl:function:: remove_bond!(topology::Topology, i, j)

    Remove any existing bond between the atoms ``i`` and ``j`` in the system.

``Atom`` type and associated function
-------------------------------------

.. jl:function:: Atom(name)

    Create an atom from an atomic name.

.. jl:function:: Atom(frame::Frame, idx)

    Get the atom at index ``idx`` from the frame.

.. jl:function:: Atom(topology::Topology, idx)

    Get the atom at index ``idx`` from the topology.

.. jl:function:: mass(atom::Atom) -> Float64

    Get the mass of an atom, in atomic mass units.

.. jl:function:: set_mass!(atom::Atom, mass)

    Set the mass of an atom to ``mass``, in atomic mass units.

.. jl:function:: charge(atom::Atom) -> Float64

    Get the charge of an atom, in number of the electron charge e.

.. jl:function:: set_charge!(atom::Atom, charge)

    Set the charge of an atom to ``charge``, in number of the electron charge e.

.. jl:function:: name(atom::Atom) -> ASCIIString

    Get the name of an atom.

.. jl:function:: set_name!(atom::Atom, name)

    Set the name of an atom to ``name``.

.. jl:function:: full_name(atom::Atom) -> ASCIIString

    Try to get the full name of an atom from the short name.

.. jl:function:: vdw_radius(atom::Atom) -> Float32

    Try to get the Van der Waals radius of an atom from the short name. Returns -1 if no
    value could be found.

.. jl:function:: covalent_radius(atom::Atom) -> Float32

    Try to get the covalent radius of an atom from the short name. Returns -1 if no
    value could be found.

.. jl:function:: atomic_number(atom::Atom) -> Integer

    Try to get the atomic number of an atom from the short name. Returns -1 if no
    value could be found.
