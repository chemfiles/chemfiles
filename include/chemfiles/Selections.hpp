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

namespace chemfiles {
namespace selections {
    class Expr;
    typedef std::unique_ptr<Expr> Ast;
}

/// A match is a set of atomic indexes matching a given selection. The size of
/// a match depends on the associated selection, and can vary from 1 to 4.
class Match {
public:
    /// Maximal number of atoms in a match
    static constexpr size_t MAX_MATCH_SIZE = 4;

    template<typename ...Args>
    Match(Args ...args): data_({{args...}}), size_(sizeof...(args)) {
        static_assert(sizeof...(args) <= MAX_MATCH_SIZE,
        "`Match` size can not be bigger than MAX_MATCH_SIZE");
    }

    /// Get the `i`th index in the match. `i` should be lower than the actual
    /// `size()` of the match.
    const size_t& operator[](size_t i) const {
        assert(i < size_ && "Out of bounds indexing of Match");
        return data_[i];
    }

    /// Size of the match, i.e. number of valid indexes in the match. This value
    /// is always equals to the size of the `Selection` this match comes from.
    size_t size() const {
        return size_;
    }

private:
    std::array<size_t, MAX_MATCH_SIZE> data_ = {{0}};
    size_t size_ = 0;
};

inline bool operator==(const Match& lhs, const Match& rhs) {
    if (lhs.size() != rhs.size()) {
        return false;
    }
    for (size_t i = 0; i<lhs.size(); i++) {
        if (lhs[i] != rhs[i]) {
            return false;
        };
    }
    return true;
}

//! Selection context, i.e. what we are matching
enum class Context {
    //! Matching a single atom
    ATOM,
    //! Matching a pair of atoms
    PAIR,
    //! Matching three atoms
    THREE,
    //! Matching four atoms
    FOUR,
    //! Matching a bond
    BOND,
    //! Matching an angle
    ANGLE,
    //! Matching a dihedral angle
    DIHEDRAL
};

/*!
* @class Selection Selections.hpp Selections.cpp
* @brief This class allow to select atoms in a `Frame`, from a selection language.
*
* The selection language is built by combining basic operations. Each basic
* operation follows the `<selector>[(<variable>)] <operator> <value>` structure,
* where `<operator>` is a comparison operator in `== != < <= > >=`. Refer to
* the full documentation to know the allowed selectors and how to use them.
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
    std::vector<Match> evaluate(const Frame& frame) const;

    /// Evaluates a selection of size 1 on a given `frame`. This function
    /// returns the list of atomic indexes in the frame matching this selection.
    ///
    /// @throw SelectionError if the selection size is not 1.
    std::vector<size_t> list(const Frame& frame) const;

    /// Size of the matches for this selection
    size_t size() const;

private:
    //! Generate all possible (unconstrained) matches for this selection and
    //! the given `frame`.
    std::vector<Match> generate_matches(const Frame& frame) const;

    //! Store the selection string that generated this selection
    std::string selection_;
    //! Selection kind
    Context context_;
    //! AST for evaluation of the selection
    selections::Ast ast_;
};
}

#endif
