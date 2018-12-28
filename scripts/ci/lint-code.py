#!/usr/bin/env python
# -*- coding: utf8 -*-
"""
This script check for some functions we don't want to use because they are
better wrappers in chemfiles
"""
import os
import sys
from glob import glob

ROOT = os.path.join(os.path.dirname(__file__), "..", "..")
ERRORS = 0

PARSING_FUNCTIONS = {
    'stof': 'parse<double>',
    'stod': 'parse<double>',
    'stold': 'parse<double>',
    'stoi': 'parse<long long>',
    'stol': 'parse<long long>',
    'stoll': 'parse<long long>',
    'stoul': 'parse<size_t>',
    'stoull': 'parse<size_t>',
}

CAPI_FUNCTIONS = {
    'static_cast<size_t>': 'checked_cast'
}


def error(message):
    global ERRORS
    ERRORS += 1
    print(message)


def check_code(path, replacements):
    with open(path) as fd:
        for i, line in enumerate(fd):
            for bad, replacement in replacements.items():
                if bad in line:
                    error("Please replace '{}' by '{}' at {}:{}".format(
                        bad, replacement, os.path.relpath(path), i
                    ))


if __name__ == '__main__':
    for file in glob(os.path.join(ROOT, "include", "chemfiles", "*.hpp")):
        if file.endswith("utils.hpp"):
            continue
        check_code(file, PARSING_FUNCTIONS)

    for file in glob(os.path.join(ROOT, "include", "chemfiles", "*", "*.hpp")):
        check_code(file, PARSING_FUNCTIONS)

    for file in glob(os.path.join(ROOT, "src", "*.cpp")):
        check_code(file, PARSING_FUNCTIONS)

    for file in glob(os.path.join(ROOT, "src", "*", "*.cpp")):
        check_code(file, PARSING_FUNCTIONS)

    for file in glob(os.path.join(ROOT, "src", "capi", "*.cpp")):
        check_code(file, CAPI_FUNCTIONS)

    if ERRORS != 0:
        sys.exit(1)
