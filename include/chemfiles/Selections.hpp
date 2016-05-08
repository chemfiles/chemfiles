/* Chemfiles, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
 */

#ifndef CHEMFILES_SELECTION_HPP
#define CHEMFILES_SELECTION_HPP

#include <memory>
#include <string>
#include <vector>

#include "chemfiles/Frame.hpp"
#include "chemfiles/bool.hpp"

namespace chemfiles {
namespace selections {
    class Expr;
    typedef std::unique_ptr<Expr> Ast;
}

/// A match is a set of atomic indexes matching a given selection. The size of
/// a match depends on the associated selection, and can vary from 1 to 4.
class Match {
public:
    ///! Maximal number of atoms in a match
    static constexpr size_t MAX_MATCH_SIZE = 4;

    template<typename ...Args>
    Match(Args ...args): data_({{args...}}), size_(sizeof...(args)) {
        static_assert(sizeof...(args) < MAX_MATCH_SIZE,
        "`Match` size can not be bigger than MAX_MATCH_SIZE");
    }

    const size_t& operator[](size_t idx) const {
        assert(idx < size_ && "Out of bounds indexing of Match");
        return data_[idx];
    }

    size_t size() const {
        return size_;
    }

private:
    std::array<size_t, MAX_MATCH_SIZE> data_ = {{0}};
    size_t size_ = 0;
};

using Matches = std::vector<Match>;

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
* - `mass`: select atoms on their mass. `<value>` must be a number. Examples:
*   `mass 4`; `mass < 2.0`.
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
* elided and implicitly `==`. These selections are `name`, `index` and `mass`.
* So `name O or index 234` is equivalent to `name == O or index == 234`.
*
* Two other special operation are the `all` and `none` selection, matching
* respectively all and none of the atoms in the frame.
*/
class CHFL_EXPORT Selection {
public:
    //! Create a selection using the given string.
    //!
    //! @throws SelectionError if there is a error in the selection string
    explicit Selection(const std::string& selection);

    ~Selection();
    Selection(Selection&& other);
    Selection& operator=(Selection&& other);

    Selection(const Selection& other) = delete;
    Selection& operator=(const Selection& other) = delete;

    /// Evaluates the selection on a given `frame`. This function returns the
    /// list of matches in the frame for this selection.
    Matches evaluate(const Frame& frame) const;

    /// Evaluates a selection of size 1 on a given `frame`. This function
    /// returns the list of atomic indexes in the frame matching this selection.
    ///
    /// @throw SelectionError if the selection is not of size 1
    std::vector<size_t> list(const Frame& frame) const;

    ///! Size of the matches for this selection
    size_t size() const;

private:
    //! Generate all possible (unconstrained) matches for this selection and
    //! the given `frame`.
    Matches generate_matches(const Frame& frame) const;

    std::string selection_;
    selections::Ast ast_;
};
}

#endif
