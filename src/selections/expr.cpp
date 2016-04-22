/* Chemfiles, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
 */
#include <cmath>
#include <functional>

#include "chemfiles/Error.hpp"
#include "chemfiles/selections/expr.hpp"

namespace chemfiles {
namespace selections {

std::ostream& operator<<(std::ostream& out, const std::unique_ptr<Expr>& expr) {
    out << expr->print();
    return out;
}

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
    default:
        throw std::runtime_error("Hit the default case in binop_str");
    }
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
    default:
        throw std::runtime_error("Hit the default case in binop_str");
    }
}

/****************************************************************************************/
std::string AllExpr::print(unsigned) const {
    return "all";
}

std::vector<Bool> AllExpr::evaluate(const Frame& frame) const {
    return std::vector<Bool>(frame.natoms(), true);
}

template <>
Ast parse<AllExpr>(token_iterator_t& begin, const token_iterator_t& end) {
    assert(end - begin >= 1);
    assert(begin->is_ident());
    assert(begin->ident() == "all");
    begin += 1;
    return Ast(new AllExpr());
}

/****************************************************************************************/
std::string NoneExpr::print(unsigned) const {
    return "none";
}

std::vector<Bool> NoneExpr::evaluate(const Frame& frame) const {
    return std::vector<Bool>(frame.natoms(), false);
}

template <>
Ast parse<NoneExpr>(token_iterator_t& begin, const token_iterator_t& end) {
    assert(end - begin >= 1);
    assert(begin->is_ident());
    assert(begin->ident() == "none");
    begin += 1;
    return Ast(new NoneExpr());
}

/****************************************************************************************/
std::string NameExpr::print(unsigned) const {
    if (equals_) {
        return "name == " + name_;
    } else {
        return "name != " + name_;
    }
}

std::vector<Bool> NameExpr::evaluate(const Frame& frame) const {
    auto res = std::vector<Bool>(frame.natoms(), false);
    auto topology = frame.topology();
    for (size_t i = 0; i < frame.natoms(); i++) {
        res[i] = ((topology[i].name() == name_) == equals_);
    }
    return res;
}

template <>
Ast parse<NameExpr>(token_iterator_t& begin, const token_iterator_t& end) {
    assert(end - begin >= 3);
    assert(begin[2].is_ident());
    assert(begin[2].ident() == "name");
    if (!begin[1].is_ident() ||
        !(begin[0].type() == Token::EQ || begin[0].type() == Token::NEQ)) {
        throw ParserError("Name selection must follow the pattern: 'name == "
                          "{name} | name != {name}'");
    }
    auto equals = (begin[0].type() == Token::EQ);
    auto name = begin[1].ident();
    begin += 3;
    return Ast(new NameExpr(name, equals));
}

/****************************************************************************************/
std::string PositionExpr::print(unsigned) const {
    return coord_.to_string() + " " + binop_str(op_) + " " +
           std::to_string(val_);
}

std::vector<Bool> PositionExpr::evaluate(const Frame& frame) const {
    auto res = std::vector<Bool>(frame.natoms(), false);
    auto compare = binop_comparison<double>(op_);
    auto j = coord_.as_index();
    auto& positions = frame.positions();
    for (size_t i = 0; i < frame.natoms(); i++) {
        res[i] = compare(positions[i][j], val_);
    }
    return res;
}

template <>
Ast parse<PositionExpr>(token_iterator_t& begin, const token_iterator_t& end) {
    assert(end - begin >= 3);
    assert(begin[2].is_ident());
    assert(begin[2].ident() == "x" || begin[2].ident() == "y" ||
           begin[2].ident() == "z");
    assert(begin->is_binary_op());

    auto coord = Coordinate(begin[2].ident());
    auto op = BinOp(begin[0].type());
    if (!begin[1].is_number()) {
        throw ParserError(
            "Position selection can only contain number as criterium.");
    }
    auto val = begin[1].number();
    begin += 3;
    return Ast(new PositionExpr(coord, op, val));
}

/****************************************************************************************/
std::string VelocityExpr::print(unsigned) const {
    return "v" + coord_.to_string() + " " + binop_str(op_) + " " +
           std::to_string(val_);
}

std::vector<Bool> VelocityExpr::evaluate(const Frame& frame) const {
    auto res = std::vector<Bool>(frame.natoms(), false);
    auto velocities = frame.velocities();
    if (velocities) {
        auto compare = binop_comparison<double>(op_);
        auto j = coord_.as_index();
        for (size_t i = 0; i < frame.natoms(); i++) {
            res[i] = compare((*velocities)[i][j], val_);
        }
    }
    return res;
}

template <>
Ast parse<VelocityExpr>(token_iterator_t& begin, const token_iterator_t& end) {
    assert(end - begin >= 3);
    assert(begin[2].is_ident());
    assert(begin[2].ident() == "vx" || begin[2].ident() == "vy" ||
           begin[2].ident() == "vz");
    assert(begin->is_binary_op());

    auto coord = Coordinate(begin[2].ident().substr(1));
    auto op = BinOp(begin[0].type());
    if (!begin[1].is_number()) {
        throw ParserError(
            "Veclocity selection can only contain number as criterium.");
    }
    auto val = begin[1].number();
    begin += 3;
    return Ast(new VelocityExpr(coord, op, val));
    ;
}

/****************************************************************************************/
std::string IndexExpr::print(unsigned) const {
    return "index " + binop_str(op_) + " " + std::to_string(val_);
}

std::vector<Bool> IndexExpr::evaluate(const Frame& frame) const {
    auto res = std::vector<Bool>(frame.natoms(), false);
    auto compare = binop_comparison<size_t>(op_);
    for (size_t i = 0; i < frame.natoms(); i++) {
        res[i] = compare(i, val_);
    }
    return res;
}

template <>
Ast parse<IndexExpr>(token_iterator_t& begin, const token_iterator_t& end) {
    assert(end - begin >= 3);
    assert(begin[2].is_ident());
    assert(begin[2].ident() == "index");
    assert(begin->is_binary_op());

    auto op = BinOp(begin[0].type());
    if (begin[1].is_number()) {
        auto num = begin[1].number();
        if (ceil(num) != num) {
            throw ParserError("Index selection should contain an integer");
        }
    } else {
        throw ParserError("Index selection should contain an integer");
    }
    auto val = static_cast<std::size_t>(begin[1].number());
    begin += 3;
    return Ast(new IndexExpr(op, val));
    ;
}

/****************************************************************************************/
std::string MassExpr::print(unsigned) const {
    return "mass " + binop_str(op_) + " " + std::to_string(val_);
}

std::vector<Bool> MassExpr::evaluate(const Frame& frame) const {
    auto res = std::vector<Bool>(frame.natoms(), false);
    auto compare = binop_comparison<double>(op_);
    auto topology = frame.topology();
    for (size_t i = 0; i < frame.natoms(); i++) {
        res[i] = compare(topology[i].mass(), val_);
    }
    return res;
}

template <>
Ast parse<MassExpr>(token_iterator_t& begin, const token_iterator_t& end) {
    assert(end - begin >= 3);
    assert(begin[2].is_ident());
    assert(begin[2].ident() == "mass");
    assert(begin[0].is_binary_op());

    if (!begin[1].is_number()) {
        throw ParserError("Mass selection should contain a number");
    }

    auto op = BinOp(begin[0].type());
    auto val = begin[1].number();
    begin += 3;
    return Ast(new MassExpr(op, val));
    ;
}

/****************************************************************************************/
std::string AndExpr::print(unsigned delta) const {
    auto lhs = lhs_->print(7);
    auto rhs = rhs_->print(7);
    return "and -> " + lhs + "\n" + std::string(delta, ' ') + "    -> " + rhs;
}

std::vector<Bool> AndExpr::evaluate(const Frame& frame) const {
    auto lhs = lhs_->evaluate(frame);
    auto rhs = rhs_->evaluate(frame);
    assert(lhs.size() == rhs.size());
    assert(lhs.size() == frame.natoms());
    for (size_t i = 0; i < frame.natoms(); i++) {
        lhs[i] = lhs[i] && rhs[i];
    }
    return lhs;
}

template <>
Ast parse<AndExpr>(token_iterator_t& begin, const token_iterator_t& end) {
    assert(begin[0].type() == Token::AND);
    begin += 1;
    if (begin == end)
        throw ParserError("Missing right-hand side operand to 'and'");

    Ast rhs = nullptr;
    try {
        rhs = dispatch_parsing(begin, end);
    } catch (const ParserError& e) {
        throw ParserError(
            std::string("Error in right-hand side operand to 'and': ") +
            e.what());
    }

    if (begin == end)
        throw ParserError("Missing left-hand side operand to 'and'");

    Ast lhs = nullptr;
    try {
        lhs = dispatch_parsing(begin, end);
    } catch (const ParserError& e) {
        throw ParserError(
            std::string("Error in left-hand side operand to 'and': ") +
            e.what());
    }
    return Ast(new AndExpr(std::move(lhs), std::move(rhs)));
}

/****************************************************************************************/
std::string OrExpr::print(unsigned delta) const {
    auto lhs = lhs_->print(6);
    auto rhs = rhs_->print(6);
    return "or -> " + lhs + "\n" + std::string(delta, ' ') + "   -> " + rhs;
}

std::vector<Bool> OrExpr::evaluate(const Frame& frame) const {
    auto lhs = lhs_->evaluate(frame);
    auto rhs = rhs_->evaluate(frame);
    assert(lhs.size() == rhs.size());
    assert(lhs.size() == frame.natoms());
    for (size_t i = 0; i < frame.natoms(); i++) {
        lhs[i] = lhs[i] || rhs[i];
    }
    return lhs;
}

template <>
Ast parse<OrExpr>(token_iterator_t& begin, const token_iterator_t& end) {
    assert(begin[0].type() == Token::OR);
    begin += 1;
    if (begin == end)
        throw ParserError("Missing right-hand side operand to 'or'");

    Ast rhs = nullptr;
    try {
        rhs = dispatch_parsing(begin, end);
    } catch (const ParserError& e) {
        throw ParserError(
            std::string("Error in right-hand side operand to 'or': ") +
            e.what());
    }

    if (begin == end)
        throw ParserError("Missing left-hand side operand to 'or'");

    Ast lhs = nullptr;
    try {
        lhs = dispatch_parsing(begin, end);
    } catch (const ParserError& e) {
        throw ParserError(
            std::string("Error in left-hand side operand to 'or': ") +
            e.what());
    }
    return Ast(new OrExpr(std::move(lhs), std::move(rhs)));
}

/****************************************************************************************/
std::string NotExpr::print(unsigned) const {
    auto ast = ast_->print(4);
    return "not " + ast;
}

std::vector<Bool> NotExpr::evaluate(const Frame& frame) const {
    auto res = ast_->evaluate(frame);
    assert(res.size() == frame.natoms());
    for (size_t i = 0; i < frame.natoms(); i++) {
        res[i] = !res[i];
    }
    return res;
}

template <>
Ast parse<NotExpr>(token_iterator_t& begin, const token_iterator_t& end) {
    assert(begin[0].type() == Token::NOT);
    begin += 1;
    if (begin == end)
        throw ParserError("Missing operand to 'not'");

    Ast ast = nullptr;
    try {
        ast = dispatch_parsing(begin, end);
    } catch (const ParserError& e) {
        throw ParserError(std::string("Error in operand of 'not': ") +
                          e.what());
    }

    return Ast(new NotExpr(std::move(ast)));
}
}
} // namespace chemfiles && namespace selections
