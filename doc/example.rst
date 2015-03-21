Usage examples
==============

This example will read a file and fill a vector called ``indexes`` with the
indexes of all the atom obeying the :math:`x < 5` relation, in the first frame
of a trajectory.

C++ example
-----------

.. code-block:: cpp

    #include <iostream>
    #include "Chemharp.cpp"

    int main() {
        harp::Trajectory traj("filename.xyz");
        harp::Frame frame;

        traj >> frame;
        auto positions = frame.positions();
        vector<size_t> indexes;

        for (size_t i=0; i<frame.natoms(); i++) {
            if (positions[i][0] < 5) {
                indexes.push_back(i);
            }
        }

        std::cout << "Atoms with x < 5: " << std::endl;
        for (auto i : indexes) {
            std::cout << "  - " << i << std::endl;
        }
    }

C example
---------

.. code-block:: c

    #include <stdio.h>
    #include "chemharp.h"

    int main() {
        // TODO
    }

Fortran example
---------------

.. code-block:: fortran

    program chemtest
        use chemharp
        implicit none

        ! TODO

    end program

Python example
--------------

.. code-block:: python

    from chemharp import Trajectory

    traj = Trajectory("filename.xyz")
    frame = traj.read_next_step()
    positions = frame.positions()

    indexes = []

    for i in range(len(frame)):
        # positions is a numpy ndarray
        if positions[i, 0] < 5:
            indexes.append(i);

    print("Atoms with x < 5: ")
    for i in indexes:
        print("  - {}".format(i))


TODO: add example with more than one frame
