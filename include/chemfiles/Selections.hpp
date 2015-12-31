/* Chemfiles, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
 */

#ifndef CHFL_SELECTION_HPP
#define CHFL_SELECTION_HPP
#include <memory>
#include <vector>
#include <string>

#include "chemfiles/Frame.hpp"

namespace chemfiles {
    namespace selections {
        class Expr;
        typedef std::unique_ptr<Expr> Ast;
    }

/*!
* @class Selection Selection.hpp Selection.cpp
* @brief This class allow to select some atoms in a `Frame`, using a specific
* selection language.
*
* The selection language is built by combining basic operations. Each basic
* operation follows the `<selector> <operator> <value>` structure, where
* `<operator>` can be a comparison operator in `== != < <= > >=`.
*/
class Selection {
public:
    //! Create a selection using the given string.
    //!
    //! @throws ParserError if there is a semantic error in the selection
    //! @throws LexerError if there is a syntaxic error in the selection
    explicit Selection(const std::string& selection);

    Selection(Selection&& other) = default;
    Selection& operator=(Selection&& other) = default;

    Selection(const Selection& other) = delete;
    Selection& operator=(const Selection& other) = delete;

    //! Evaluate the selection on a given `frame`. This function returns a
    //! vector of size `Frame::natoms()`, containing `true` at the index `i` if
    //! the atom at index `i` matches the selection, and `false` otherwise.
    std::vector<bool> evaluate(const Frame& frame);
private:
    std::string selection_;
    selections::Ast ast_;
};
}

#endif
