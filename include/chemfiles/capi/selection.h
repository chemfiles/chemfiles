// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) 2015-2016 Guillaume Fraux and contributors
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef CHEMFILES_CHFL_SELECTION_H
#define CHEMFILES_CHFL_SELECTION_H

#include "chemfiles/capi/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/// @brief Create a new selection from the given selection string.
/// @return A pointer to the corresponding selection, or NULL in case of error
CHFL_EXPORT CHFL_SELECTION* chfl_selection(const char* selection);

/// @brief Get the size of the selection, i.e. the number of atoms we are
///        selecting together.
///
/// This value is 1 for the 'atom' context, 2 for the 'pair' and 'bond' context,
/// 3 for the 'three' and 'angles' contextes and 4 for the 'four' and 'dihedral'
/// contextes.
///
/// @param selection The selection
/// @param size The size of the selection
/// @return The status code
CHFL_EXPORT chfl_status chfl_selection_size(
    const CHFL_SELECTION* const selection, uint64_t* size
);

/// @brief Get the selection string used to create a given selection.
/// @param selection The selection
/// @param string A string buffer to be filled with the selection string
/// @param buffsize The size of the string buffer
/// @return The status code
CHFL_EXPORT chfl_status chfl_selection_string(
    const CHFL_SELECTION* const selection, char* const string, uint64_t buffsize
);

/// @brief Evaluate a selection for a given frame.
///
/// Use the `chfl_selection_get` function to get the matches for this selection.
///
/// @param selection The selection
/// @param frame The frame
/// @param n The number of matches for the selection
/// @return The status code
CHFL_EXPORT chfl_status chfl_selection_evalutate(
    CHFL_SELECTION* const selection, const CHFL_FRAME* const frame, uint64_t* n
);

/// Maximal size for a selection match
#define CHFL_MAX_SELECTION_SIZE 4

/// @brief A match from a selection evaluation
///
/// A match is a set of atomic indexes matching a given selection. The size of
/// a match depends on the associated selection, and can vary from 1 to
/// `CHFL_MAX_SELECTION_SIZE`.
typedef struct {
    /// The actual size of the match. Elements in `atoms` are significant up
    /// to this value, and filled with `(uint64_t)-1` for all the other values.
    uint64_t size;
    /// Indexes matching the associated selection
    uint64_t atoms[CHFL_MAX_SELECTION_SIZE];
} chfl_match_t;

/// @brief Get the matches for a selection after a call to `chfl_selection_evalutate`
///
/// @param selection the selection
/// @param matches a pre-allocated array of size (number of matches)
/// @param n the number of matches, as indicated by `chfl_selection_evaluate`
/// @return The status code
CHFL_EXPORT chfl_status chfl_selection_matches(
    const CHFL_SELECTION* const selection, chfl_match_t matches[], uint64_t n
);

/// @brief Destroy a selection, and free the associated memory
/// @param selection The selection to destroy
/// @return The status code
CHFL_EXPORT chfl_status chfl_selection_free(CHFL_SELECTION* selection);

#ifdef __cplusplus
}
#endif

#endif
