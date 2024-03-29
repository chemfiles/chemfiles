"""
This script check for some functions we don't want to use because they are
better wrappers in chemfiles
"""
import os
import re
import sys
from glob import glob

ROOT = os.path.join(os.path.dirname(__file__), "..", "..")
ERRORS = 0

PARSING_FUNCTIONS = {
    # Match 'std::<...>',  '::<...>' and '<...>'
    r"(?P<name>\b(std)?(::)?atof\b)": "parse<double>",
    r"(?P<name>\b(std)?(::)?atoi\b)": "parse<int>",
    r"(?P<name>\b(std)?(::)?atol\b)": "parse<long>",
    r"(?P<name>\b(std)?(::)?atoll\b)": "parse<long long>",
    r"(?P<name>\b(std)?(::)?stof\b)": "parse<double>",
    r"(?P<name>\b(std)?(::)?stod\b)": "parse<double>",
    r"(?P<name>\b(std)?(::)?stold\b)": "parse<double>",
    r"(?P<name>\b(std)?(::)?stoi\b)": "parse<int>",
    r"(?P<name>\b(std)?(::)?stol\b)": "parse<long>",
    r"(?P<name>\b(std)?(::)?stoll\b)": "parse<long long>",
    r"(?P<name>\b(std)?(::)?stoul\b)": "parse<size_t>",
    r"(?P<name>\b(std)?(::)?stoull\b)": "parse<size_t>",
    r"(?P<name>\b(std)?(::)?strtol\b)": "parse<long>",
    r"(?P<name>\b(std)?(::)?strtoul\b)": "parse<size_t>",
    r"(?P<name>\b(std)?(::)?strtoull\b)": "parse<size_t>",
    r"(?P<name>\b(std)?(::)?strtof\b)": "parse<double>",
    r"(?P<name>\b(std)?(::)?strtod\b)": "parse<double>",
    r"(?P<name>\b(std)?(::)?strtold\b)": "parse<double>",
    r"(?P<name>\b(std)?(::)?sscanf\b)": "scan",
    r"(?P<name>\b(std)?(::)?isalnum\b)": "is_ascii_alphanumeric",
    r"(?P<name>\b(std)?(::)?isalpha\b)": "is_ascii_letter",
    r"(?P<name>\b(std)?(::)?islower\b)": "is_ascii_lowercase",
    r"(?P<name>\b(std)?(::)?isupper\b)": "is_ascii_uppercase",
    r"(?P<name>\b(std)?(::)?isdigit\b)": "is_ascii_digit",
    r"(?P<name>\b(std)?(::)?isspace\b)": "is_ascii_whitespace",
    r"(?P<name>\b(std)?(::)?isblank\b)": "is_ascii_whitespace",
    r"(?P<name>\b(std)?(::)?tolower\b)": "to_ascii_lowercase",
    r"(?P<name>\b(std)?(::)?toupper\b)": "to_ascii_uppercase",
}

CAPI_FUNCTIONS = {
    r"(?P<name>\bstatic_cast<size_t>\b)": "checked_cast",
    r"(?P<name>\bnew\b)": "shared_allocator::make_shared",
    r"(?P<name>\bdelete\b)": "shared_allocator::free",
    r"(?P<name>\bmalloc\b)": "shared_allocator::make_shared",
    r"(?P<name>\bfree\b)": "shared_allocator::free",
}


def error(message):
    global ERRORS
    ERRORS += 1
    print(message)


def check_code(path, replacements):
    with open(path, encoding="utf8") as fd:
        for i, line in enumerate(fd):
            for bad, replacement in replacements.items():
                match = re.search(bad, line)
                if match and "NOLINT" not in line:
                    error(
                        "Please replace '{}' by '{}' at {}:{}".format(
                            match.group("name"),
                            replacement,
                            os.path.relpath(path),
                            i + 1,
                        )
                    )


if __name__ == "__main__":
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
