/* Chemfiles, an efficient IO library for chemistry file formats
* Copyright (C) 2015 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/
*/
// clang-format off

#include "chemfiles.h"
#include "chemfiles/Selections.hpp"
#include "chemfiles/capi.hpp"
using namespace chemfiles;

static_assert(
    CHFL_MAX_SELECTION_SIZE == Match::MAX_MATCH_SIZE,
    "CHFL_MAX_SELECTION_SIZE should match Match::MAX_MATCH_SIZE"
);

struct CAPISelection {
    CAPISelection(Selection&& select): selection(std::move(select)), matches() {}
    Selection selection;
    std::vector<Match> matches;
};

CHFL_SELECTION* chfl_selection(const char* string) {
    CHFL_SELECTION* c_selection = nullptr;
    CHFL_ERROR_GOTO(
        c_selection = new CAPISelection(Selection(std::string(string)));
    )
    return c_selection;
error:
    delete c_selection;
    return nullptr;
}

chfl_status chfl_selection_size(const CHFL_SELECTION* c_selection, size_t* size) {
    assert(c_selection != nullptr);
    CHFL_ERROR_CATCH(
        *size = c_selection->selection.size();
    )
}

chfl_status chfl_selection_evalutate(CHFL_SELECTION* c_selection, const CHFL_FRAME* frame, size_t* n_matches) {
    assert(c_selection != nullptr);
    CHFL_ERROR_CATCH(
        c_selection->matches = c_selection->selection.evaluate(*frame);
        *n_matches = c_selection->matches.size();
    )
}

chfl_status chfl_selection_matches(const CHFL_SELECTION* c_selection, chfl_match_t* matches, size_t n_matches) {
    assert(c_selection != nullptr);
    assert(n_matches == c_selection->matches.size());
    CHFL_ERROR_CATCH(
        auto size = c_selection->selection.size();
        for (size_t i=0; i<n_matches; i++) {
            matches[i].size = static_cast<unsigned char>(size);
            for (size_t j=0; j<size; j++) {
                matches[i].atoms[j] = c_selection->matches[i][j];
            }

            for (size_t j=size; j<CHFL_MAX_SELECTION_SIZE; j++) {
                matches[i].atoms[j] = static_cast<size_t>(-1);
            }
        }
    )
}

chfl_status chfl_selection_free(CHFL_SELECTION* c_selection) {
    delete c_selection;
    c_selection = nullptr;
    return CHFL_SUCCESS;
}
