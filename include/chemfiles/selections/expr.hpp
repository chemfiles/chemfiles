/* Chemfiles, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
 */

#ifndef CHEMFILES_SELECTION_EXPR_HPP
#define CHEMFILES_SELECTION_EXPR_HPP

#include "chemfiles/selections/parser.hpp"
#include "chemfiles/utils.hpp"
#include "chemfiles/Error.hpp"

namespace chemfiles {
namespace selections {

template<typename T>
Ast parse(token_iterator_t& begin, const token_iterator_t& end);

// Existing binary operators
enum class BinOp {
    EQ = Token::EQ,     //! "=="
    NEQ = Token::NEQ,   //! "!="
    LT = Token::LT,     //! "<"
    LE = Token::LE,     //! "<="
    GT = Token::GT,     //! ">"
    GE = Token::GE,     //! ">="
};

//! Helper class representing a specific component of a 3D vector. This class
//! can take three values: X, Y or Z.
class Coordinate {
public:
    Coordinate(const Coordinate& other) = default;
    Coordinate(Coordinate&& other) = default;
    Coordinate& operator=(const Coordinate& other) = default;
    Coordinate& operator=(Coordinate&& other) = default;

    //! Create a coordinate from a string. `"x"` maps to X, `"y"` to Y and
    //! `"z"` to Z. Any other string is an error.
    explicit Coordinate(const std::string& name) {
        if (name == "x") {
            coord_ = X;
        } else if (name == "y") {
            coord_ = Y;
        } else if (name == "z") {
            coord_ = Z;
        } else {
            throw SelectionError("Could not convert '" + name + "' to coordinate.");
        }
    }

    //! Convert the coordinate to the corresponding index: 0 for X, 1 for Y and
    //! 2 for Z.
    size_t as_index() const {
        switch (coord_) {
        case X:
            return 0;
        case Y:
            return 1;
        case Z:
            return 2;
        }
        unreachable();
    }

    //! Convert the coordinate to the corresponding string
    std::string to_string() const {
        switch (coord_) {
        case X:
            return "x";
        case Y:
            return "y";
        case Z:
            return "z";
        }
        unreachable();
    }

private:
    enum {
        X,
        Y,
        Z
    } coord_;
};

//! @class AllExpr selections/expr.hpp selections/expr.cpp
//! @brief Selection matching all atoms
class AllExpr final: public Expr {
public:
    AllExpr(): Expr() {}
    std::string print(unsigned delta) const override;
    std::vector<bool> evaluate(const Frame& frame, const std::vector<Match>& matches) const override;
};

//! @class NoneExpr selections/expr.hpp selections/expr.cpp
//! @brief Selection matching no atoms
class NoneExpr final: public Expr {
public:
    NoneExpr(): Expr() {}
    std::string print(unsigned delta) const override;
    std::vector<bool> evaluate(const Frame& frame, const std::vector<Match>& matches) const override;
};

//! @class SingleExpr expr.hpp
//! @brief Abstract base class for selector taking a single argument
class SingleSelector: public Expr {
public:
    SingleSelector(unsigned argument): argument_(argument) {
        assert(argument <= 3 && "argument must be less than 3 in SingleSelector");
    }
    virtual ~SingleSelector() = default;

protected:
    //! Index of the argument to apply the selector to
    const unsigned argument_;
};

//! @class NameExpr selections/expr.hpp selections/expr.cpp
//! @brief Select atoms using their name.
//!
//! Only `==` and `!=` operators are allowed. The short form `name <value>` is
//! equivalent to `name == <value>`
class NameExpr final: public SingleSelector {
public:
    NameExpr(unsigned argument, std::string name, bool equals)
        : SingleSelector(argument), name_(name), equals_(equals) {}
    std::string print(unsigned delta) const override;
    std::vector<bool> evaluate(const Frame& frame, const std::vector<Match>& matches) const override;
private:
    std::string name_;
    bool equals_;
};

//! @class IndexExpr selections/expr.hpp selections/expr.cpp
//! @brief Select atoms using their index in the frame.
class IndexExpr final: public SingleSelector {
public:
    IndexExpr(unsigned argument, BinOp op, std::size_t val)
        : SingleSelector(argument), op_(op), val_(val) {}
    std::string print(unsigned delta) const override;
    std::vector<bool> evaluate(const Frame& frame, const std::vector<Match>& matches) const override;
private:
    BinOp op_;
    std::size_t val_;
};


//! @class PositionExpr selections/expr.hpp selections/expr.cpp
//! @brief Select atoms using their position in space. The selection can be
//! created by `x <op> <val>`, `y <op> <val>` or `z <op> <val>`, depending on
//! the component of the position to use.
class PositionExpr final: public SingleSelector {
public:
    PositionExpr(unsigned argument, Coordinate coord, BinOp op, double val)
        : SingleSelector(argument), coord_(coord), op_(op), val_(val) {}
    std::string print(unsigned delta) const override;
    std::vector<bool> evaluate(const Frame& frame, const std::vector<Match>& matches) const override;
private:
    Coordinate coord_;
    BinOp op_;
    double val_;
};

//! @class VelocityExpr selections/expr.hpp selections/expr.cpp
//! @brief Select atoms using their velocity. The selection can be created by
//! `vx <op> <val>`, `vy <op> <val>` or `vz <op> <val>`, depending on the
//! component of the velocity to use.
class VelocityExpr final: public SingleSelector {
public:
    VelocityExpr(unsigned argument, Coordinate coord, BinOp op, double val)
        : SingleSelector(argument), coord_(coord), op_(op), val_(val) {}
    std::string print(unsigned delta) const override;
    std::vector<bool> evaluate(const Frame& frame, const std::vector<Match>& matches) const override;
private:
    Coordinate coord_;
    BinOp op_;
    double val_;
};

//! @class MassExpr selections/expr.hpp selections/expr.cpp
//! @brief Select atoms using their mass, in atomic mass unit.
class MassExpr final: public SingleSelector {
public:
    MassExpr(unsigned argument, BinOp op, double val): SingleSelector(argument), op_(op), val_(val) {}
    std::string print(unsigned delta) const override;
    std::vector<bool> evaluate(const Frame& frame, const std::vector<Match>& matches) const override;
private:
    BinOp op_;
    double val_;
};

/****************************************************************************************/

//! @class AndExpr selections/expr.hpp selections/expr.cpp
//! @brief Combine selections by using a logical `and` operation
class AndExpr final: public Expr {
public:
    AndExpr(Ast&& lhs, Ast&& rhs): Expr(), lhs_(std::move(lhs)), rhs_(std::move(rhs)) {}
    std::string print(unsigned delta) const override;
    std::vector<bool> evaluate(const Frame& frame, const std::vector<Match>& matches) const override;
private:
    Ast lhs_;
    Ast rhs_;
};

//! @class OrExpr selections/expr.hpp selections/expr.cpp
//! @brief Combine selections by using a logical `or` operation
class OrExpr final: public Expr {
public:
    OrExpr(Ast&& lhs, Ast&& rhs): Expr(), lhs_(std::move(lhs)), rhs_(std::move(rhs)) {}
    std::string print(unsigned delta) const override;
    std::vector<bool> evaluate(const Frame& frame, const std::vector<Match>& matches) const override;
private:
    Ast lhs_;
    Ast rhs_;
};

//! @class NotExpr selections/expr.hpp selections/expr.cpp
//! @brief Unary negation of a selection
class NotExpr final: public Expr {
public:
    explicit NotExpr(Ast&& ast): Expr(), ast_(std::move(ast)) {}
    std::string print(unsigned delta) const override;
    std::vector<bool> evaluate(const Frame& frame, const std::vector<Match>& matches) const override;
private:
    Ast ast_;
};

}} // namespace chemfiles && namespace selections

#endif
