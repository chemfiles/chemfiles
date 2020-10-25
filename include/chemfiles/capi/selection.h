// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_CHFL_SELECTION_H
#define CHEMFILES_CHFL_SELECTION_H

#include <stdint.h>

#include "chemfiles/capi/types.h"
#include "chemfiles/exports.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Create a new selection from the given `selection` string.
///
/// The caller of this function should free the associated memory using
/// `chfl_free`.
///
/// @example{capi/chfl_selection/chfl_selection.c}
/// @return A pointer to the selection, or NULL in case of error.
///         You can use `chfl_last_error` to learn about the error.
CHFL_EXPORT CHFL_SELECTION* chfl_selection(const char* selection);

/// Get a copy of a `selection`.
///
/// The copy does not contains any state, and `chfl_selection_evaluate` must be
/// called again before using `chfl_selection_matches`.
///
/// The caller of this function should free the associated memory using
/// `chfl_free`.
///
/// @example{capi/chfl_selection/copy.c}
/// @return A pointer to the new selection, or NULL in case of error.
///         You can use `chfl_last_error` to learn about the error.
CHFL_EXPORT CHFL_SELECTION* chfl_selection_copy(const CHFL_SELECTION* selection);

/// Get the size of a `selection` in `size`.
///
/// The size of a selection is the number of atoms we are selecting together.
/// This value is 1 for the 'atom' context, 2 for the 'pair' and 'bond' context,
/// 3 for the 'three' and 'angles' contexts and 4 for the 'four' and 'dihedral'
/// contexts.
///
/// @example{capi/chfl_selection/size.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_selection_size(
    const CHFL_SELECTION* selection, uint64_t* size
);

/// Get the selection string used to create a given `selection` in the `string`
/// buffer.
///
/// The buffer size must be passed in `buffsize`. This function will truncate
/// the selection string to fit in the buffer.
///
/// @example{capi/chfl_selection/string.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_selection_string(
    const CHFL_SELECTION* selection, char* string, uint64_t buffsize
);

/// Evaluate a `selection` for a given `frame`, and store the number of matches
/// in `n_matches`.
///
/// Use the `chfl_selection_matches` function to get the matches for this
/// selection.
///
/// @example{capi/chfl_selection/evaluate.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_selection_evaluate(
    CHFL_SELECTION* selection, const CHFL_FRAME* frame, uint64_t* n_matches
);

/// Get the matches for a `selection` after a call to `chfl_selection_evaluate`,
/// in `matches`.
///
/// The size of the `matches` array must be passed in `n_matches`.
///
/// @example{capi/chfl_selection/matches.c}
/// @return The operation status code. You can use `chfl_last_error` to learn
///         about the error if the status code is not `CHFL_SUCCESS`.
CHFL_EXPORT chfl_status chfl_selection_matches(
    const CHFL_SELECTION* selection, chfl_match matches[], uint64_t n_matches
);

#ifdef __cplusplus
}
#endif

#endif
