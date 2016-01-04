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
#include "chemfiles/bool.hpp"

namespace chemfiles {
    namespace selections {
        class Expr;
        typedef std::unique_ptr<Expr> Ast;
    }

/*!
* @class Selection Selections.hpp Selections.cpp
* @brief This class allow to select some atoms in a `Frame`, using a specific
* selection language.
*
* The selection language is built by combining basic operations. Each basic
* operation follows the `<selector> <operator> <value>` structure, where
* `<operator>` can be a comparison operator in `== != < <= > >=`.
*
* Implemented `<selector>` and associated `<value>` types are given below:
*
* - `name`: select atoms on their name. `<value>` must be a string, and only the
*   `==` and `!=` operators are allowed. Examples: `name == C`; `name != Hw`.
* - `index`: select atoms on their index in the frame. `<value>` must be an
*    integer. Examples: `index == 4`; `index > 304`.
* - `x|y|z`: select atoms on their position. <value>` must be a number.
*    Examples: `x <= 7.3`; `z != 4.2`; `y > 2`.
* - `vx|vy|vz`: select atoms on their velocity. <value>` must be a number.
*    Examples: `vx <= 7.3`; `vz != 4.2`; `vy > 2`.
*
* These basic operations can be combined by three logical operators: `and`, `or`
* and `not`. Parentheses can be used to remove ambiguity when using multiple
* operators.
*
* 	`name == H and (x < 45.9 or vz >= 67) and (not index == 67)`
*
* Some selections also accept a short form, where the comparison operator is
* elided and implicitly `==`. These selections are `name` and `index`, as in
* `name O or index 234` which is equivalent to `name == O or index == 234`.
*
* Another special basic operation is the "all" selection, matching all the atoms
* in the frame. 
*/
class CHFL_EXPORT Selection {
public:
    //! Create a selection using the given string.
    //!
    //! @throws ParserError if there is a semantic error in the selection
    //! @throws LexerError if there is a syntaxic error in the selection
    explicit Selection(const std::string& selection);

    ~Selection();
    Selection(Selection&& other);
    Selection& operator=(Selection&& other);

    Selection(const Selection& other) = delete;
    Selection& operator=(const Selection& other) = delete;

    //! Evaluate the selection on a given `frame`. This function returns a
    //! vector of size `Frame::natoms()`, containing `true` at the index `i` if
    //! the atom at index `i` matches the selection, and `false` otherwise.
    std::vector<Bool> evaluate(const Frame& frame) const;
private:
    std::string selection_;
    selections::Ast ast_;
};
}

#endif
