#!/usr/bin/env python
# -*- coding: utf8 -*-
"""
Check that all error messages are formatted the same way.
"""
import os
import sys
import glob
import re

ROOT = os.path.join(os.path.dirname(__file__), "..", "..")
ERRORS = 0

# Path, line of messages that do not follow the current format, but
# are allowed
ALLOWED = [
    (os.path.join(ROOT, "src", "Configuration.cpp"), 34)
]


def error(message):
    global ERRORS
    ERRORS += 1
    print(message)


def extract_messages(lines, i):
    start = lines[i].find('"')
    if start == -1:
        # look in the next line
        i = i + 1
        start = lines[i].find('"')
        if start == -1:
            print("warning: could not get the message at {}:{}".format(
                path, i - 1
            ))
            return ""

    stop = lines[i].find('"', start + 1)
    if stop == -1:
        error("could not get the message end at {}:{}".format(
            path, i
        ))

    return lines[i][start + 1:stop]


def check_message(path, line, message):
    if not message:
        return

    for allowed_path, allowed_line in ALLOWED:
        if allowed_path == path and allowed_line == line:
            return

    # check for upper case at the start, allow abbreviations (LAMMPS, HELIX)
    if message[0].isupper() and not message[1].isupper():
        error(
            "message starts in upper case\n"
            "    => '{}' at {}:{}".format(message, path, line)
        )

    # check for full stops in sentences
    if re.search("[a-zA-Z][\\.!?]", message):
        error(
            "found punctuation (., ! or ?) in message\n"
            "    => '{}' at {}:{}".format(message, path, line)
        )


def check_file(path):
    with open(path) as fd:
        lines = [l for l in fd]

    for (i, line) in enumerate(lines):
        if " warning(" in line:
            check_message(path, i, extract_messages(lines, i))

        if "throw" in line:
            check_message(path, i, extract_messages(lines, i))


if __name__ == '__main__':
    for path in glob.glob(os.path.join(ROOT, "src/*.cpp")):
        check_file(path)

    for path in glob.glob(os.path.join(ROOT, "src/*/*.cpp")):
        check_file(path)

    if ERRORS != 0:
        sys.exit(1)
