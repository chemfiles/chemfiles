// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#if false

#ifndef CHEMFILES_SELECTION_EXPR_HPP
#define CHEMFILES_SELECTION_EXPR_HPP

#include "chemfiles/selections/lexer.hpp"
#include "chemfiles/selections/parser.hpp"

#include "chemfiles/unreachable.hpp"
#include "chemfiles/Error.hpp"
#include "chemfiles/Frame.hpp"
#include "chemfiles/Selection.hpp"

namespace chemfiles {
namespace selections {

/// Combine selections by using a logical `and` operation
class And final: public Selector {
public:
    And(Ast lhs, Ast rhs): lhs_(std::move(lhs)), rhs_(std::move(rhs)) {}
    std::string print(unsigned delta) const override;
    bool is_match(const Frame& frame, const Match& match) const override;
private:
    Ast lhs_;
    Ast rhs_;
};

/// Combine selections by using a logical `or` operation
class Or final: public Selector {
public:
    Or(Ast lhs, Ast rhs): lhs_(std::move(lhs)), rhs_(std::move(rhs)) {}
    std::string print(unsigned delta) const override;
    bool is_match(const Frame& frame, const Match& match) const override;
private:
    Ast lhs_;
    Ast rhs_;
};

/// Unary negation of a selection
class Not final: public Selector {
public:
    explicit Not(Ast ast): ast_(std::move(ast)) {}
    std::string print(unsigned delta) const override;
    bool is_match(const Frame& frame, const Match& match) const override;
private:
    Ast ast_;
};

// Existing binary operators
enum class BinOp {
    /// "=="
    EQ = Token::EQ,
    /// "!="
    NEQ = Token::NEQ,
    /// "<"
    LT = Token::LT,
    /// "<="
    LE = Token::LE,
    /// ">"
    GT = Token::GT,
    /// ">="
    GE = Token::GE,
};

/// Selection matching all atoms
class All final: public Selector {
public:
    All() = default;
    std::string print(unsigned delta) const override;
    bool is_match(const Frame& frame, const Match& match) const override;
};

/// Selection matching no atoms
class None final: public Selector {
public:
    None() = default;
    std::string print(unsigned delta) const override;
    bool is_match(const Frame& frame, const Match& match) const override;
};

/// Abstract base class for selector taking a single argument
class SingleSelector: public Selector {
public:
    SingleSelector(unsigned argument): argument_(argument) {
        assert(argument <= 3 && "argument must be less than 3 in SingleSelector");
    }

protected:
    /// Index of the argument to apply the selector to
    const unsigned argument_;
};

/// Select atoms using their type
class Type final: public SingleSelector {
public:
    Type(unsigned argument, std::string type, bool equals)
        : SingleSelector(argument), type_(std::move(type)), equals_(equals) {}
    std::string print(unsigned delta) const override;
    bool is_match(const Frame& frame, const Match& match) const override;
private:
    std::string type_;
    bool equals_;
};

/// Select atoms using their name.
class Name final: public SingleSelector {
public:
    Name(unsigned argument, std::string name, bool equals)
        : SingleSelector(argument), name_(std::move(name)), equals_(equals) {}
    std::string print(unsigned delta) const override;
    bool is_match(const Frame& frame, const Match& match) const override;
private:
    std::string name_;
    bool equals_;
};

/// Select atoms using their residue name
class Resname final: public SingleSelector {
public:
    Resname(unsigned argument, std::string name, bool equals)
        : SingleSelector(argument), name_(std::move(name)), equals_(equals) {}
    std::string print(unsigned delta) const override;
    bool is_match(const Frame& frame, const Match& match) const override;
private:
    std::string name_;
    bool equals_;
};

/// Select atoms using their index in the frame.
class Index final: public SingleSelector {
public:
    Index(unsigned argument, BinOp op, std::size_t val)
        : SingleSelector(argument), op_(op), val_(val) {}
    std::string print(unsigned delta) const override;
    bool is_match(const Frame& frame, const Match& match) const override;
private:
    BinOp op_;
    std::size_t val_;
};

/// Select atoms using their residue id (residue number)
class Resid final: public SingleSelector {
public:
    Resid(unsigned argument, BinOp op, uint64_t id)
        : SingleSelector(argument), op_(op), id_(id) {}
    std::string print(unsigned delta) const override;
    bool is_match(const Frame& frame, const Match& match) const override;
private:
    BinOp op_;
    uint64_t id_;
};

/// Select atoms using their mass.
class Mass final: public SingleSelector {
public:
    Mass(unsigned argument, BinOp op, double val): SingleSelector(argument), op_(op), val_(val) {}
    std::string print(unsigned delta) const override;
    bool is_match(const Frame& frame, const Match& match) const override;
private:
    BinOp op_;
    double val_;
};

/// Helper class representing a specific component of a 3D vector. This class
/// can take three values: X, Y or Z.
class Coordinate {
public:
    ~Coordinate() = default;
    Coordinate(const Coordinate& other) = default;
    Coordinate(Coordinate&& other) = default;
    Coordinate& operator=(const Coordinate& other) = default;
    Coordinate& operator=(Coordinate&& other) = default;

    /// Create a coordinate from a string. `"x"` maps to X, `"y"` to Y and
    /// `"z"` to Z. Any other string is an error.
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

    /// Convert the coordinate to the corresponding index: 0 for X, 1 for Y and
    /// 2 for Z.
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

    /// Convert the coordinate to the corresponding string
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

/// Select atoms using their position in space. The selection can be created by
/// `x <op> <val>`, `y <op> <val>` or `z <op> <val>`, depending on the component
/// of the position to use.
class Position final: public SingleSelector {
public:
    Position(unsigned argument, Coordinate coord, BinOp op, double val)
        : SingleSelector(argument), coord_(coord), op_(op), val_(val) {}
    std::string print(unsigned delta) const override;
    bool is_match(const Frame& frame, const Match& match) const override;
private:
    Coordinate coord_;
    BinOp op_;
    double val_;
};

/// Select atoms using their velocity. The selection can be created by `vx <op>
/// <val>`, `vy <op> <val>` or `vz <op> <val>`, depending on the component of
/// the velocity to use.
class Velocity final: public SingleSelector {
public:
    Velocity(unsigned argument, Coordinate coord, BinOp op, double val)
        : SingleSelector(argument), coord_(coord), op_(op), val_(val) {}
    std::string print(unsigned delta) const override;
    bool is_match(const Frame& frame, const Match& match) const override;
private:
    Coordinate coord_;
    BinOp op_;
    double val_;
};

template<typename T>
Ast parse(token_iterator_t& begin, const token_iterator_t& end);

#define PARSE_EXISTS(T) \
template<> Ast parse<T>(token_iterator_t& begin, const token_iterator_t& end)

PARSE_EXISTS(All);
PARSE_EXISTS(None);
PARSE_EXISTS(Type);
PARSE_EXISTS(Name);
PARSE_EXISTS(Index);
PARSE_EXISTS(Resname);
PARSE_EXISTS(Resid);
PARSE_EXISTS(Position);
PARSE_EXISTS(Velocity);
PARSE_EXISTS(Mass);
PARSE_EXISTS(And);
PARSE_EXISTS(Or);
PARSE_EXISTS(Not);

#undef PARSE_EXISTS

}} // namespace chemfiles && namespace selections

#endif

#endif
