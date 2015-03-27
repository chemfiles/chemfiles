Chemharp overview
=================

This figure represent how the basic types of Chemharp are arganised and how they
interact together. The only types a Chemharp user should worry about are the
``Trajectory``, ``Frame``, ``Topology``, ``Atom`` and ``UnitCell`` types. All of
them are described in this section.

.. image:: img/chemharp.*
    :align: center

They are various way of interacting with these types, all of them being described
in the :ref:`classes-reference` section for the C++ API. All the bindings may not
provide all the functionalities, so if something is missing, just
`ask for it! <https://github.com/Luthaf/Chemharp/issues/new>`_

.. _overview-trajectory:

Trajectory: the main entry point
--------------------------------

A ``Trajectory`` uses a file and a format together to read simulation data from
the file. It can read and write one or many ``Frame`` to this file. The file
type and the format are automatically determined from the extention.

.. _overview-frame:

Frame: data from a simulation step
----------------------------------

A ``Frame`` holds data for one step of a simulation. As not all formats provides
all the types of informations, some fields may be initialized to a default value.

A ``Frame`` may contains the following data:

* Positions for all the atoms in the system;
* Velocities for all the atoms in the system;
* The ``Topology`` of the system;
* The ``UnitCell`` of the system.

.. _overview-topology:

Topology: how are the atoms organised
-------------------------------------

A ``Topology`` describes the organisation of the particles in the system.
What are they names, how are they bonded together, … A topology is mainly a list
of ``Atom`` in the system.

.. _overview-atom:

Atom: building blocks for simulations
-------------------------------------

The ``Atom`` type contains basic information about the particles in the system:
the name (if it is disponible), mass, kind of particle and so on.

.. _overview-cell:

UnitCell: wrapping the atoms together
-------------------------------------

The ``UnitCell`` type describe the boundary conditions of the system: where are
the boundaries, and what is the periodicity of theses boundaries.
