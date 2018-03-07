// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#if false

#include <cmath>
#include <functional>

#include "chemfiles/selections/expr.hpp"

#include "chemfiles/Frame.hpp"
#include "chemfiles/ErrorFmt.hpp"
#include "chemfiles/warnings.hpp"

namespace chemfiles {
namespace selections {

//! Get the associated string to a binary operator
static std::string binop_str(BinOp op) {
    switch (op) {
    case BinOp::EQ:
        return "==";
    case BinOp::NEQ:
        return "!=";
    case BinOp::LT:
        return "<";
    case BinOp::LE:
        return "<=";
    case BinOp::GT:
        return ">";
    case BinOp::GE:
        return ">=";
    }
    unreachable();
}

//! Get the associated function to a binary operator for type `T`
template <typename T> std::function<bool(T, T)> binop_comparison(BinOp op) {
    switch (op) {
    case BinOp::EQ:
        return std::equal_to<T>();
    case BinOp::NEQ:
        return std::not_equal_to<T>();
    case BinOp::LT:
        return std::less<T>();
    case BinOp::LE:
        return std::less_equal<T>();
    case BinOp::GT:
        return std::greater<T>();
    case BinOp::GE:
        return std::greater_equal<T>();
    }
    unreachable();
}

/****************************************************************************************/
std::string All::print(unsigned /*unused*/) const {
    return "all";
}

bool All::is_match(const Frame& /*unused*/, const Match& /*unused*/) const {
    return true;
}

template <>
Ast parse<All>(token_iterator_t& begin, const token_iterator_t& end) {
    assert(end - begin >= 1);
    assert(begin->is_ident());
    assert(begin->ident() == "all");
    begin += 1;
    return Ast(new All());
}

/****************************************************************************************/
std::string None::print(unsigned /*unused*/) const {
    return "none";
}

bool None::is_match(const Frame& /*unused*/, const Match& /*unused*/) const {
    return false;
}

template <>
Ast parse<None>(token_iterator_t& begin, const token_iterator_t& end) {
    assert(end - begin >= 1);
    assert(begin->is_ident());
    assert(begin->ident() == "none");
    begin += 1;
    return Ast(new None());
}

/****************************************************************************************/
std::string Type::print(unsigned /*unused*/) const {
    auto op = equals_ ? "==" : "!=";
    return "type(#" + std::to_string(argument_ + 1) + ") " + op + " " + type_;
}

bool Type::is_match(const Frame& frame, const Match& match) const {
    auto index = match[argument_];
    return (frame.topology()[index].type() == type_) == equals_;
}

template <>
Ast parse<Type>(token_iterator_t& begin, const token_iterator_t& end) {
    assert(end - begin >= 3);
    assert(begin[2].is_ident());
    assert(begin[2].ident() == "type");
    if (!begin[1].is_ident() ||
        !(begin[0].type() == Token::EQ || begin[0].type() == Token::NEQ)) {
        throw selection_error(
            "atomic type selection must follow the pattern: 'type == <type> | type != <type>'"
        );
    }
    auto equals = (begin[0].type() == Token::EQ);
    auto element = begin[1].ident();
    if (end - begin >= 4 && begin[3].is_variable()) {
        auto argument = begin[3].variable() - 1;
        begin += 4;
        return Ast(new Type(argument, element, equals));
    } else {
        begin += 3;
        return Ast(new Type(0, element, equals));
    }
}

/****************************************************************************************/
std::string Name::print(unsigned /*unused*/) const {
    auto op = equals_ ? "==" : "!=";
    return "name(#" + std::to_string(argument_ + 1) + ") " + op + " " + name_;
}

bool Name::is_match(const Frame& frame, const Match& match) const {
    auto index = match[argument_];
    return (frame.topology()[index].name() == name_) == equals_;
}

template <>
Ast parse<Name>(token_iterator_t& begin, const token_iterator_t& end) {
    assert(end - begin >= 3);
    assert(begin[2].is_ident());
    assert(begin[2].ident() == "name");
    if (!begin[1].is_ident() ||
        !(begin[0].type() == Token::EQ || begin[0].type() == Token::NEQ)) {
        throw selection_error(
            "name selection must follow the pattern: 'name == <name> | name != <name>'"
        );
    }
    auto equals = (begin[0].type() == Token::EQ);
    auto element = begin[1].ident();
    if (end - begin >= 4 && begin[3].is_variable()) {
        auto argument = begin[3].variable() - 1;
        begin += 4;
        return Ast(new Name(argument, element, equals));
    } else {
        begin += 3;
        return Ast(new Name(0, element, equals));
    }
}

/****************************************************************************************/
std::string Resname::print(unsigned /*unused*/) const {
    auto op = equals_ ? "==" : "!=";
    return "resname(#" + std::to_string(argument_ + 1) + ") " + op + " " + name_;
}

bool Resname::is_match(const Frame& frame, const Match& match) const {
    auto index = match[argument_];
    auto residue = frame.topology().residue_for_atom(index);
    if (residue) {
        return (residue->name() == name_) == equals_;
    } else {
        // No residue for this atom
        return false;
    }
}

template <>
Ast parse<Resname>(token_iterator_t& begin, const token_iterator_t& end) {
    assert(end - begin >= 3);
    assert(begin[2].is_ident());
    assert(begin[2].ident() == "resname");
    if (!begin[1].is_ident() ||
        !(begin[0].type() == Token::EQ || begin[0].type() == Token::NEQ)) {
        throw selection_error(
            "residue name selection must follow the pattern: 'resname == <name> | resname != <name>'"
        );
    }
    auto equals = (begin[0].type() == Token::EQ);
    auto name = begin[1].ident();
    if (end - begin >= 4 && begin[3].is_variable()) {
        auto argument = begin[3].variable() - 1;
        begin += 4;
        return Ast(new Resname(argument, name, equals));
    } else {
        begin += 3;
        return Ast(new Resname(0, name, equals));
    }
}

/****************************************************************************************/
std::string Resid::print(unsigned /*unused*/) const {
    return "resid(#" + std::to_string(argument_ + 1) + ") " + binop_str(op_) +
           " " + std::to_string(id_);
}

bool Resid::is_match(const Frame& frame, const Match& match) const {
    auto compare = binop_comparison<uint64_t>(op_);
    auto index = match[argument_];
    auto residue = frame.topology().residue_for_atom(index);
    if (residue && residue->id()) {
        return compare(residue->id().value(), id_);
    } else {
        // No residue or residue id for this atom
        return false;
    }
}

template <>
Ast parse<Resid>(token_iterator_t& begin, const token_iterator_t& end) {
    assert(end - begin >= 3);
    assert(begin[2].is_ident());
    assert(begin[2].ident() == "resid");
    assert(begin->is_binary_op());

    auto op = BinOp(begin[0].type());
    if (begin[1].is_number()) {
        auto num = begin[1].number();
        if (ceil(num) != num) {
            throw selection_error("residue index selection should contain an integer");
        }
    } else {
        throw selection_error("residue index selection should contain an integer");
    }
    auto val = static_cast<std::size_t>(begin[1].number());
    if (end - begin >= 4 && begin[3].is_variable()) {
        auto argument = begin[3].variable() - 1;
        begin += 4;
        return Ast(new Resid(argument, op, val));
    } else {
        begin += 3;
        return Ast(new Resid(0, op, val));
    }
}

/****************************************************************************************/
std::string Position::print(unsigned /*unused*/) const {
    return coord_.to_string() + "(#" + std::to_string(argument_ + 1) + ") " +
           binop_str(op_) + " " + std::to_string(val_);
}

bool Position::is_match(const Frame& frame, const Match& match) const {
    auto compare = binop_comparison<double>(op_);
    auto i = match[argument_];
    auto j = coord_.as_index();
    return compare(frame.positions()[i][j], val_);
}

template <>
Ast parse<Position>(token_iterator_t& begin, const token_iterator_t& end) {
    assert(end - begin >= 3);
    assert(begin[2].is_ident());
    assert(begin[2].ident() == "x" || begin[2].ident() == "y" ||
           begin[2].ident() == "z");
    assert(begin->is_binary_op());

    auto coord = Coordinate(begin[2].ident());
    auto op = BinOp(begin[0].type());
    if (!begin[1].is_number()) {
        throw selection_error(
            "position selection can only contain number as criterium.");
    }
    auto val = begin[1].number();
    if (end - begin >= 4 && begin[3].is_variable()) {
        auto argument = begin[3].variable() - 1;
        begin += 4;
        return Ast(new Position(argument, coord, op, val));
    } else {
        begin += 3;
        return Ast(new Position(0, coord, op, val));
    }
}

/****************************************************************************************/
std::string Velocity::print(unsigned /*unused*/) const {
    return "v" + coord_.to_string() + "(#" + std::to_string(argument_ + 1) +
            ") " + binop_str(op_) + " " + std::to_string(val_);
}

bool Velocity::is_match(const Frame& frame, const Match& match) const {
    if (frame.velocities()) {
        auto compare = binop_comparison<double>(op_);
        auto i = match[argument_];
        auto j = coord_.as_index();
        auto& velocities = *frame.velocities();
        return compare(velocities[i][j], val_);
    } else {
        return false;
    }
}

template <>
Ast parse<Velocity>(token_iterator_t& begin, const token_iterator_t& end) {
    assert(end - begin >= 3);
    assert(begin[2].is_ident());
    assert(begin[2].ident() == "vx" || begin[2].ident() == "vy" ||
           begin[2].ident() == "vz");
    assert(begin->is_binary_op());

    auto coord = Coordinate(begin[2].ident().substr(1));
    auto op = BinOp(begin[0].type());
    if (!begin[1].is_number()) {
        throw selection_error(
            "veclocity selection can only contain number as criterium.");
    }
    auto val = begin[1].number();
    if (end - begin >= 4 && begin[3].is_variable()) {
        auto argument = begin[3].variable() - 1;
        begin += 4;
        return Ast(new Velocity(argument, coord, op, val));
    } else {
        begin += 3;
        return Ast(new Velocity(0, coord, op, val));
    }
}

/****************************************************************************************/
std::string Index::print(unsigned /*unused*/) const {
    return "index(#" + std::to_string(argument_ + 1) + ") " + binop_str(op_) +
           " " + std::to_string(val_);
}

bool Index::is_match(const Frame& /*unused*/, const Match& match) const {
    auto compare = binop_comparison<size_t>(op_);
    return compare(match[argument_], val_);
}

template <>
Ast parse<Index>(token_iterator_t& begin, const token_iterator_t& end) {
    assert(end - begin >= 3);
    assert(begin[2].is_ident());
    assert(begin[2].ident() == "index");
    assert(begin->is_binary_op());

    auto op = BinOp(begin[0].type());
    if (begin[1].is_number()) {
        auto num = begin[1].number();
        if (ceil(num) != num) {
            throw selection_error("index selection should contain an integer");
        }
    } else {
        throw selection_error("index selection should contain an integer");
    }
    auto val = static_cast<std::size_t>(begin[1].number());
    if (end - begin >= 4 && begin[3].is_variable()) {
        auto argument = begin[3].variable() - 1;
        begin += 4;
        return Ast(new Index(argument, op, val));
    } else {
        begin += 3;
        return Ast(new Index(0, op, val));
    }
}

/****************************************************************************************/
std::string Mass::print(unsigned /*unused*/) const {
    return "mass(#" + std::to_string(argument_ + 1) + ") " + binop_str(op_) +
           " " + std::to_string(val_);
}

bool Mass::is_match(const Frame& frame, const Match& match) const {
    auto compare = binop_comparison<double>(op_);
    auto index = match[argument_];
    return compare(frame.topology()[index].mass(), val_);
}

template <>
Ast parse<Mass>(token_iterator_t& begin, const token_iterator_t& end) {
    assert(end - begin >= 3);
    assert(begin[2].is_ident());
    assert(begin[2].ident() == "mass");
    assert(begin[0].is_binary_op());

    if (!begin[1].is_number()) {
        throw selection_error("mass selection should contain a number");
    }

    auto op = BinOp(begin[0].type());
    auto val = begin[1].number();
    if (end - begin >= 4 && begin[3].is_variable()) {
        auto argument = begin[3].variable() - 1;
        begin += 4;
        return Ast(new Mass(argument, op, val));
    } else {
        begin += 3;
        return Ast(new Mass(0, op, val));
    }
}

/****************************************************************************************/
std::string And::print(unsigned delta) const {
    auto lhs = lhs_->print(7);
    auto rhs = rhs_->print(7);
    return "and -> " + lhs + "\n" + std::string(delta, ' ') + "    -> " + rhs;
}

bool And::is_match(const Frame& frame, const Match& match) const {
    return lhs_->is_match(frame, match) && rhs_->is_match(frame, match);
}

template <>
Ast parse<And>(token_iterator_t& begin, const token_iterator_t& end) {
    assert(begin[0].type() == Token::AND);
    begin += 1;
    if (begin == end) {
        throw selection_error("missing right-hand side operand to 'and'");
    }
    auto rhs = dispatch_parsing(begin, end);

    if (begin == end) {
        throw selection_error("missing left-hand side operand to 'and'");
    }
    auto lhs = dispatch_parsing(begin, end);
    return Ast(new And(std::move(lhs), std::move(rhs)));
}

/****************************************************************************************/
std::string Or::print(unsigned delta) const {
    auto lhs = lhs_->print(6);
    auto rhs = rhs_->print(6);
    return "or -> " + lhs + "\n" + std::string(delta, ' ') + "   -> " + rhs;
}

bool Or::is_match(const Frame& frame, const Match& match) const {
    return lhs_->is_match(frame, match) || rhs_->is_match(frame, match);
}

template <>
Ast parse<Or>(token_iterator_t& begin, const token_iterator_t& end) {
    assert(begin[0].type() == Token::OR);
    begin += 1;
    if (begin == end) {
        throw selection_error("missing right-hand side operand to 'or'");
    }
    auto rhs = dispatch_parsing(begin, end);

    if (begin == end) {
        throw selection_error("missing left-hand side operand to 'or'");
    }
    auto lhs = dispatch_parsing(begin, end);
    return Ast(new Or(std::move(lhs), std::move(rhs)));
}

/****************************************************************************************/
std::string Not::print(unsigned /*unused*/) const {
    auto ast = ast_->print(4);
    return "not " + ast;
}

bool Not::is_match(const Frame& frame, const Match& match) const {
    return !ast_->is_match(frame, match);
}

template <>
Ast parse<Not>(token_iterator_t& begin, const token_iterator_t& end) {
    assert(begin[0].type() == Token::NOT);
    begin += 1;
    if (begin == end) {
        throw selection_error("missing operand to 'not'");
    }
    auto ast = dispatch_parsing(begin, end);
    return Ast(new Not(std::move(ast)));
}

}} // namespace chemfiles && namespace selections


#endif
