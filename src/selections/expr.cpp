// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) 2015-2016 Guillaume Fraux and contributors
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include <cmath>
#include <functional>

#include "chemfiles/Error.hpp"
#include "chemfiles/selections/expr.hpp"
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
std::string AllExpr::print(unsigned /*unused*/) const {
    return "all";
}

std::vector<bool> AllExpr::evaluate(const Frame& /*unused*/, const std::vector<Match>& matches) const {
    return std::vector<bool>(matches.size(), true);
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
std::string NoneExpr::print(unsigned /*unused*/) const {
    return "none";
}

std::vector<bool> NoneExpr::evaluate(const Frame& /*unused*/, const std::vector<Match>& matches) const {
    return std::vector<bool>(matches.size(), false);
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
std::string TypeExpr::print(unsigned /*unused*/) const {
    auto op = equals_ ? "==" : "!=";
    return "type(#" + std::to_string(argument_ + 1) + ") " + op + " " + type_;
}

std::vector<bool> TypeExpr::evaluate(const Frame& frame, const std::vector<Match>& matches) const {
    auto res = std::vector<bool>(matches.size(), false);
    auto topology = frame.topology();
    for (size_t i = 0; i < matches.size(); i++) {
        auto idx = matches[i][argument_];
        res[i] = ((topology[idx].type() == type_) == equals_);
    }
    return res;
}

template <>
Ast parse<TypeExpr>(token_iterator_t& begin, const token_iterator_t& end) {
    assert(end - begin >= 3);
    assert(begin[2].is_ident());
    assert(begin[2].ident() == "type");
    if (!begin[1].is_ident() ||
        !(begin[0].type() == Token::EQ || begin[0].type() == Token::NEQ)) {
        throw SelectionError(
            "Atomic type selection must follow the pattern: 'type == {type} | type != {type}'"
        );
    }
    auto equals = (begin[0].type() == Token::EQ);
    auto element = begin[1].ident();
    if (end - begin >= 4 && begin[3].is_variable()) {
        auto argument = begin[3].variable() - 1;
        begin += 4;
        return Ast(new TypeExpr(argument, element, equals));
    } else {
        begin += 3;
        return Ast(new TypeExpr(0, element, equals));
    }
}

/****************************************************************************************/
std::string NameExpr::print(unsigned /*unused*/) const {
    auto op = equals_ ? "==" : "!=";
    return "name(#" + std::to_string(argument_ + 1) + ") " + op + " " + name_;
}

std::vector<bool> NameExpr::evaluate(const Frame& frame, const std::vector<Match>& matches) const {
    auto res = std::vector<bool>(matches.size(), false);
    auto topology = frame.topology();
    for (size_t i = 0; i < matches.size(); i++) {
        auto idx = matches[i][argument_];
        res[i] = ((topology[idx].name() == name_) == equals_);
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
        throw SelectionError("Name selection must follow the pattern: 'name == "
                             "{name} | name != {name}'");
    }
    auto equals = (begin[0].type() == Token::EQ);
    auto element = begin[1].ident();
    if (end - begin >= 4 && begin[3].is_variable()) {
        auto argument = begin[3].variable() - 1;
        begin += 4;
        return Ast(new NameExpr(argument, element, equals));
    } else {
        begin += 3;
        return Ast(new NameExpr(0, element, equals));
    }
}

/****************************************************************************************/
std::string ResnameExpr::print(unsigned /*unused*/) const {
    auto op = equals_ ? "==" : "!=";
    return "resname(#" + std::to_string(argument_ + 1) + ") " + op + " " + name_;
}

std::vector<bool> ResnameExpr::evaluate(const Frame& frame, const std::vector<Match>& matches) const {
    auto res = std::vector<bool>(matches.size(), false);
    auto topology = frame.topology();
    for (size_t i = 0; i < matches.size(); i++) {
        auto idx = matches[i][argument_];
        auto residue = topology.residue(idx);
        if (residue) {
            res[i] = ((residue->name() == name_) == equals_);
        } else {
            // No residue for this atom
            res[i] = false;
        }
    }
    return res;
}

template <>
Ast parse<ResnameExpr>(token_iterator_t& begin, const token_iterator_t& end) {
    assert(end - begin >= 3);
    assert(begin[2].is_ident());
    assert(begin[2].ident() == "resname");
    if (!begin[1].is_ident() ||
        !(begin[0].type() == Token::EQ || begin[0].type() == Token::NEQ)) {
        throw SelectionError(
            "Residue name selection must follow the pattern: 'resname == {name} | resname != {name}'"
        );
    }
    auto equals = (begin[0].type() == Token::EQ);
    auto name = begin[1].ident();
    if (end - begin >= 4 && begin[3].is_variable()) {
        auto argument = begin[3].variable() - 1;
        begin += 4;
        return Ast(new ResnameExpr(argument, name, equals));
    } else {
        begin += 3;
        return Ast(new ResnameExpr(0, name, equals));
    }
}

/****************************************************************************************/
std::string ResidExpr::print(unsigned /*unused*/) const {
    return "resid(#" + std::to_string(argument_ + 1) + ") " + binop_str(op_) +
           " " + std::to_string(id_);
}

std::vector<bool> ResidExpr::evaluate(const Frame& frame, const std::vector<Match>& matches) const {
    auto res = std::vector<bool>(matches.size(), false);
    auto compare = binop_comparison<uint64_t>(op_);
    auto topology = frame.topology();
    for (size_t i = 0; i < matches.size(); i++) {
        auto idx = matches[i][argument_];
        auto residue = topology.residue(idx);
        if (residue && residue->id() != static_cast<size_t>(-1)) {
            res[i] = compare(residue->id(), id_);
        } else {
            // No residue or residue id for this atom
            res[i] = false;
        }
    }
    return res;
}

template <>
Ast parse<ResidExpr>(token_iterator_t& begin, const token_iterator_t& end) {
    assert(end - begin >= 3);
    assert(begin[2].is_ident());
    assert(begin[2].ident() == "resid");
    assert(begin->is_binary_op());

    auto op = BinOp(begin[0].type());
    if (begin[1].is_number()) {
        auto num = begin[1].number();
        if (ceil(num) != num) {
            throw SelectionError("Residue index selection should contain an integer");
        }
    } else {
        throw SelectionError("Residue index selection should contain an integer");
    }
    auto val = static_cast<std::size_t>(begin[1].number());
    if (end - begin >= 4 && begin[3].is_variable()) {
        auto argument = begin[3].variable() - 1;
        begin += 4;
        return Ast(new ResidExpr(argument, op, val));
    } else {
        begin += 3;
        return Ast(new ResidExpr(0, op, val));
    }
}

/****************************************************************************************/
std::string PositionExpr::print(unsigned /*unused*/) const {
    return coord_.to_string() + "(#" + std::to_string(argument_ + 1) + ") " +
           binop_str(op_) + " " + std::to_string(val_);
}

std::vector<bool> PositionExpr::evaluate(const Frame& frame, const std::vector<Match>& matches) const {
    auto res = std::vector<bool>(matches.size(), false);
    auto compare = binop_comparison<double>(op_);
    auto j = coord_.as_index();
    auto& positions = frame.positions();
    for (size_t i = 0; i < matches.size(); i++) {
        auto idx = matches[i][argument_];
        auto position = positions[idx][j];
        res[i] = compare(position, val_);
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
        throw SelectionError(
            "Position selection can only contain number as criterium.");
    }
    auto val = begin[1].number();
    if (end - begin >= 4 && begin[3].is_variable()) {
        auto argument = begin[3].variable() - 1;
        begin += 4;
        return Ast(new PositionExpr(argument, coord, op, val));
    } else {
        begin += 3;
        return Ast(new PositionExpr(0, coord, op, val));
    }
}

/****************************************************************************************/
std::string VelocityExpr::print(unsigned /*unused*/) const {
    return "v" + coord_.to_string() + "(#" + std::to_string(argument_ + 1) +
            ") " + binop_str(op_) + " " + std::to_string(val_);
}

std::vector<bool> VelocityExpr::evaluate(const Frame& frame, const std::vector<Match>& matches) const {
    auto res = std::vector<bool>(matches.size(), false);

    if (frame.velocities()) {
        auto velocities = *frame.velocities();
        auto compare = binop_comparison<double>(op_);
        auto j = coord_.as_index();
        for (size_t i = 0; i < matches.size(); i++) {
            auto idx = matches[i][argument_];
            auto velocity = velocities[idx][j];
            res[i] = compare(velocity, val_);
        }
    } else {
        warning("No velocities in frame while evaluating {}", print(0));
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
        throw SelectionError(
            "Veclocity selection can only contain number as criterium.");
    }
    auto val = begin[1].number();
    if (end - begin >= 4 && begin[3].is_variable()) {
        auto argument = begin[3].variable() - 1;
        begin += 4;
        return Ast(new VelocityExpr(argument, coord, op, val));
    } else {
        begin += 3;
        return Ast(new VelocityExpr(0, coord, op, val));
    }
}

/****************************************************************************************/
std::string IndexExpr::print(unsigned /*unused*/) const {
    return "index(#" + std::to_string(argument_ + 1) + ") " + binop_str(op_) +
           " " + std::to_string(val_);
}

std::vector<bool> IndexExpr::evaluate(const Frame& /*unused*/, const std::vector<Match>& matches) const {
    auto res = std::vector<bool>(matches.size(), false);
    auto compare = binop_comparison<size_t>(op_);
    for (size_t i = 0; i < matches.size(); i++) {
        auto idx = matches[i][argument_];
        res[i] = compare(idx, val_);
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
            throw SelectionError("Index selection should contain an integer");
        }
    } else {
        throw SelectionError("Index selection should contain an integer");
    }
    auto val = static_cast<std::size_t>(begin[1].number());
    if (end - begin >= 4 && begin[3].is_variable()) {
        auto argument = begin[3].variable() - 1;
        begin += 4;
        return Ast(new IndexExpr(argument, op, val));
    } else {
        begin += 3;
        return Ast(new IndexExpr(0, op, val));
    }
}

/****************************************************************************************/
std::string MassExpr::print(unsigned /*unused*/) const {
    return "mass(#" + std::to_string(argument_ + 1) + ") " + binop_str(op_) +
           " " + std::to_string(val_);
}

std::vector<bool> MassExpr::evaluate(const Frame& frame, const std::vector<Match>& matches) const {
    auto res = std::vector<bool>(matches.size(), false);
    auto compare = binop_comparison<double>(op_);
    auto topology = frame.topology();
    for (size_t i = 0; i < matches.size(); i++) {
        auto idx = matches[i][argument_];
        res[i] = compare(topology[idx].mass(), val_);
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
        throw SelectionError("Mass selection should contain a number");
    }

    auto op = BinOp(begin[0].type());
    auto val = begin[1].number();
    if (end - begin >= 4 && begin[3].is_variable()) {
        auto argument = begin[3].variable() - 1;
        begin += 4;
        return Ast(new MassExpr(argument, op, val));
    } else {
        begin += 3;
        return Ast(new MassExpr(0, op, val));
    }
}

/****************************************************************************************/
std::string AndExpr::print(unsigned delta) const {
    auto lhs = lhs_->print(7);
    auto rhs = rhs_->print(7);
    return "and -> " + lhs + "\n" + std::string(delta, ' ') + "    -> " + rhs;
}

std::vector<bool> AndExpr::evaluate(const Frame& frame, const std::vector<Match>& matches) const {
    auto lhs = lhs_->evaluate(frame, matches);
    auto rhs = rhs_->evaluate(frame, matches);
    assert(lhs.size() == rhs.size());
    assert(lhs.size() == matches.size());
    for (size_t i = 0; i < matches.size(); i++) {
        lhs[i] = lhs[i] && rhs[i];
    }
    return lhs;
}

template <>
Ast parse<AndExpr>(token_iterator_t& begin, const token_iterator_t& end) {
    assert(begin[0].type() == Token::AND);
    begin += 1;
    if (begin == end) {
        throw SelectionError("Missing right-hand side operand to 'and'");
    }

    Ast rhs = nullptr;
    try {
        rhs = dispatch_parsing(begin, end);
    } catch (const SelectionError& e) {
        throw SelectionError(
            std::string("Error in right-hand side operand to 'and': ") +
            e.what());
    }

    if (begin == end) {
        throw SelectionError("Missing left-hand side operand to 'and'");
    }

    Ast lhs = nullptr;
    try {
        lhs = dispatch_parsing(begin, end);
    } catch (const SelectionError& e) {
        throw SelectionError(
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

std::vector<bool> OrExpr::evaluate(const Frame& frame, const std::vector<Match>& matches) const {
    auto lhs = lhs_->evaluate(frame, matches);
    auto rhs = rhs_->evaluate(frame, matches);
    assert(lhs.size() == rhs.size());
    assert(lhs.size() == matches.size());
    for (size_t i = 0; i < matches.size(); i++) {
        lhs[i] = lhs[i] || rhs[i];
    }
    return lhs;
}

template <>
Ast parse<OrExpr>(token_iterator_t& begin, const token_iterator_t& end) {
    assert(begin[0].type() == Token::OR);
    begin += 1;
    if (begin == end) {
        throw SelectionError("Missing right-hand side operand to 'or'");
    }

    Ast rhs = nullptr;
    try {
        rhs = dispatch_parsing(begin, end);
    } catch (const SelectionError& e) {
        throw SelectionError(
            std::string("Error in right-hand side operand to 'or': ") +
            e.what());
    }

    if (begin == end) {
        throw SelectionError("Missing left-hand side operand to 'or'");
    }

    Ast lhs = nullptr;
    try {
        lhs = dispatch_parsing(begin, end);
    } catch (const SelectionError& e) {
        throw SelectionError(
            std::string("Error in left-hand side operand to 'or': ") +
            e.what()
        );
    }
    return Ast(new OrExpr(std::move(lhs), std::move(rhs)));
}

/****************************************************************************************/
std::string NotExpr::print(unsigned /*unused*/) const {
    auto ast = ast_->print(4);
    return "not " + ast;
}

std::vector<bool> NotExpr::evaluate(const Frame& frame, const std::vector<Match>& matches) const {
    auto res = ast_->evaluate(frame, matches);
    assert(res.size() == matches.size());
    for (size_t i = 0; i < matches.size(); i++) {
        res[i] = !res[i];
    }
    return res;
}

template <>
Ast parse<NotExpr>(token_iterator_t& begin, const token_iterator_t& end) {
    assert(begin[0].type() == Token::NOT);
    begin += 1;
    if (begin == end) {
        throw SelectionError("Missing operand to 'not'");
    }

    Ast ast = nullptr;
    try {
        ast = dispatch_parsing(begin, end);
    } catch (const SelectionError& e) {
        throw SelectionError(
            std::string("Error in operand of 'not': ") + e.what()
        );
    }

    return Ast(new NotExpr(std::move(ast)));
}

}} // namespace chemfiles && namespace selections
