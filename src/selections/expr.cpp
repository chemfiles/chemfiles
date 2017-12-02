// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

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
std::string AllExpr::print(unsigned /*unused*/) const {
    return "all";
}

bool AllExpr::is_match(const Frame& /*unused*/, const Match& /*unused*/) const {
    return true;
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

bool NoneExpr::is_match(const Frame& /*unused*/, const Match& /*unused*/) const {
    return false;
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

bool TypeExpr::is_match(const Frame& frame, const Match& match) const {
    auto index = match[argument_];
    return (frame.topology()[index].type() == type_) == equals_;
}

template <>
Ast parse<TypeExpr>(token_iterator_t& begin, const token_iterator_t& end) {
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

bool NameExpr::is_match(const Frame& frame, const Match& match) const {
    auto index = match[argument_];
    return (frame.topology()[index].name() == name_) == equals_;
}

template <>
Ast parse<NameExpr>(token_iterator_t& begin, const token_iterator_t& end) {
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

bool ResnameExpr::is_match(const Frame& frame, const Match& match) const {
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
Ast parse<ResnameExpr>(token_iterator_t& begin, const token_iterator_t& end) {
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

bool ResidExpr::is_match(const Frame& frame, const Match& match) const {
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
Ast parse<ResidExpr>(token_iterator_t& begin, const token_iterator_t& end) {
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

bool PositionExpr::is_match(const Frame& frame, const Match& match) const {
    auto compare = binop_comparison<double>(op_);
    auto i = match[argument_];
    auto j = coord_.as_index();
    return compare(frame.positions()[i][j], val_);
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
        throw selection_error(
            "position selection can only contain number as criterium.");
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

bool VelocityExpr::is_match(const Frame& frame, const Match& match) const {
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
Ast parse<VelocityExpr>(token_iterator_t& begin, const token_iterator_t& end) {
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

bool IndexExpr::is_match(const Frame& /*unused*/, const Match& match) const {
    auto compare = binop_comparison<size_t>(op_);
    return compare(match[argument_], val_);
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
            throw selection_error("index selection should contain an integer");
        }
    } else {
        throw selection_error("index selection should contain an integer");
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

bool MassExpr::is_match(const Frame& frame, const Match& match) const {
    auto compare = binop_comparison<double>(op_);
    auto index = match[argument_];
    return compare(frame.topology()[index].mass(), val_);
}

template <>
Ast parse<MassExpr>(token_iterator_t& begin, const token_iterator_t& end) {
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

bool AndExpr::is_match(const Frame& frame, const Match& match) const {
    return lhs_->is_match(frame, match) && rhs_->is_match(frame, match);
}

template <>
Ast parse<AndExpr>(token_iterator_t& begin, const token_iterator_t& end) {
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
    return Ast(new AndExpr(std::move(lhs), std::move(rhs)));
}

/****************************************************************************************/
std::string OrExpr::print(unsigned delta) const {
    auto lhs = lhs_->print(6);
    auto rhs = rhs_->print(6);
    return "or -> " + lhs + "\n" + std::string(delta, ' ') + "   -> " + rhs;
}

bool OrExpr::is_match(const Frame& frame, const Match& match) const {
    return lhs_->is_match(frame, match) || rhs_->is_match(frame, match);
}

template <>
Ast parse<OrExpr>(token_iterator_t& begin, const token_iterator_t& end) {
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
    return Ast(new OrExpr(std::move(lhs), std::move(rhs)));
}

/****************************************************************************************/
std::string NotExpr::print(unsigned /*unused*/) const {
    auto ast = ast_->print(4);
    return "not " + ast;
}

bool NotExpr::is_match(const Frame& frame, const Match& match) const {
    return !ast_->is_match(frame, match);
}

template <>
Ast parse<NotExpr>(token_iterator_t& begin, const token_iterator_t& end) {
    assert(begin[0].type() == Token::NOT);
    begin += 1;
    if (begin == end) {
        throw selection_error("missing operand to 'not'");
    }
    auto ast = dispatch_parsing(begin, end);
    return Ast(new NotExpr(std::move(ast)));
}

}} // namespace chemfiles && namespace selections
