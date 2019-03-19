#!/usr/bin/env python
# -*- coding: utf8 -*-
"""
Check that only chemfiles symbols are exported by the shared library.
"""
import os
import sys
import subprocess

ROOT = os.path.join(os.path.dirname(__file__), "..", "..")
ERRORS = 0


def error(message):
    global ERRORS
    ERRORS += 1
    print(message)


def list_symbols(path):
    list = subprocess.check_output(["nm", "-D", "-C", path], stderr=subprocess.STDOUT)
    symbols = []
    for line in list.split("\n"):
        # Only global code symbols
        if " T " in line:
            name = " ".join(line.split()[2:])
            symbols.append(name)
    return symbols


if __name__ == '__main__':
    for symbol in list_symbols(sys.argv[1]):
        if symbol.startswith("std::"):
            continue
        elif symbol.startswith("chemfiles::"):
            continue
        elif symbol.startswith("chfl_"):
            continue
        elif symbol.startswith("transaction clone"):
            continue
        elif symbol in ["_init", "_fini"]:
            continue
        else:
            error("External symbol {} is exported".format(symbol))

    if ERRORS != 0:
        sys.exit(1)
