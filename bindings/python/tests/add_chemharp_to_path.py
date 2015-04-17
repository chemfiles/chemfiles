# -* coding: utf-8 -*

"""
Add the Chemharp module to sys.path.
"""

import sys

try:
    import chemharp
except ImportError:
    print (sys.argv)
    if len(sys.argv) > 1:
        sys.path.append(sys.argv[1])
    else:
        print("Could not import chemharp. Please provide the path to " +
              "'chemharp.so' by running python " +
              "{} path/to/chemharp".format(sys.argv[0]))
        sys.exit(1)
