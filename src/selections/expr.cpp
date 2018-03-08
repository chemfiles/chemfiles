// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "chemfiles/Frame.hpp"
#include "chemfiles/Selection.hpp"

#include "chemfiles/selections/expr.hpp"

using namespace chemfiles;
using namespace chemfiles::selections;

std::string And::print(unsigned delta) const {
    auto lhs = lhs_->print(7);
    auto rhs = rhs_->print(7);
    return "and -> " + lhs + "\n" + std::string(delta, ' ') + "    -> " + rhs;
}

bool And::is_match(const Frame& frame, const Match& match) const {
    return lhs_->is_match(frame, match) && rhs_->is_match(frame, match);
}

std::string Or::print(unsigned delta) const {
    auto lhs = lhs_->print(6);
    auto rhs = rhs_->print(6);
    return "or -> " + lhs + "\n" + std::string(delta, ' ') + "   -> " + rhs;
}

bool Or::is_match(const Frame& frame, const Match& match) const {
    return lhs_->is_match(frame, match) || rhs_->is_match(frame, match);
}

std::string Not::print(unsigned /*unused*/) const {
    return "not " + ast_->print(4);
}

bool Not::is_match(const Frame& frame, const Match& match) const {
    return !ast_->is_match(frame, match);
}

std::string All::print(unsigned /*unused*/) const {
    return "all";
}

bool All::is_match(const Frame& /*unused*/, const Match& /*unused*/) const {
    return true;
}

std::string None::print(unsigned /*unused*/) const {
    return "none";
}

bool None::is_match(const Frame& /*unused*/, const Match& /*unused*/) const {
    return false;
}

std::string StringSelector::print(unsigned /*unused*/) const {
    auto name = this->name();
    auto op = equals_ ? "==" : "!=";
    return name + "(#" + std::to_string(argument_ + 1) + ") " + op + " " + value_;
}

bool StringSelector::is_match(const Frame& frame, const Match& match) const {
    return (this->value(frame, match[argument_]) == value_) == equals_;
}

std::string Type::name() const {
    return "type";
}

const std::string& Type::value(const Frame& frame, size_t i) const {
    return frame[i].type();
}

std::string Name::name() const {
    return "name";
}

const std::string& Name::value(const Frame& frame, size_t i) const {
    return frame[i].name();
}

std::string Resname::name() const {
    return "resname";
}

static const std::string EMPTY_STRING = "";

const std::string& Resname::value(const Frame& frame, size_t i) const {
    auto residue = frame.topology().residue_for_atom(i);
    if (residue) {
        return residue->name();
    } else {
        return EMPTY_STRING;
    }
}


bool Math::is_match(const Frame& frame, const Match& match) const {
    auto lhs = lhs_->eval(frame, match);
    auto rhs = rhs_->eval(frame, match);
    switch (op_) {
    case Math::Operator::EQUAL:
        return lhs == rhs;
    case Math::Operator::NOT_EQUAL:
        return lhs != rhs;
    case Math::Operator::LESS:
        return lhs < rhs;
    case Math::Operator::LESS_EQUAL:
        return lhs <= rhs;
    case Math::Operator::GREATER:
        return lhs > rhs;
    case Math::Operator::GREATER_EQUAL:
        return lhs >= rhs;
    }
    unreachable();
}

std::string Math::print(unsigned /*unused*/) const {
    std::string op;
    switch (op_) {
    case Math::Operator::EQUAL:
        op = " == ";
        break;
    case Math::Operator::NOT_EQUAL:
        op = " != ";
        break;
    case Math::Operator::LESS:
        op = " < ";
        break;
    case Math::Operator::LESS_EQUAL:
        op = " <= ";
        break;
    case Math::Operator::GREATER:
        op = " > ";
        break;
    case Math::Operator::GREATER_EQUAL:
        op = " >= ";
        break;
    default:
        unreachable();
    }

    return lhs_->print() + op + rhs_->print();
}


double Add::eval(const Frame& frame, const Match& match) const {
    return lhs_->eval(frame, match) + rhs_->eval(frame, match);
}

std::string Add::print() const {
    return "(" + lhs_->print() + " + " + rhs_->print() + ")";
}

double Sub::eval(const Frame& frame, const Match& match) const {
    return lhs_->eval(frame, match) - rhs_->eval(frame, match);
}

std::string Sub::print() const {
    return "(" + lhs_->print() + " - " + rhs_->print() + ")";
}

double Mul::eval(const Frame& frame, const Match& match) const {
    return lhs_->eval(frame, match) * rhs_->eval(frame, match);
}

std::string Mul::print() const {
    return "(" + lhs_->print() + " * " + rhs_->print() + ")";
}

double Div::eval(const Frame& frame, const Match& match) const {
    return lhs_->eval(frame, match) / rhs_->eval(frame, match);
}

std::string Div::print() const {
    return "(" + lhs_->print() + " / " + rhs_->print() + ")";
}

double Pow::eval(const Frame& frame, const Match& match) const {
    return pow(lhs_->eval(frame, match), rhs_->eval(frame, match));
}

std::string Pow::print() const {
    return lhs_->print() + " ^(" + rhs_->print() + ")";
}

double Function::eval(const Frame& frame, const Match& match) const {
    return fn_(ast_->eval(frame, match));
}

std::string Function::print() const {
    return name_ + "(" + ast_->print() + ")";
}

double Number::eval(const Frame& /*unused*/, const Match& /*unused*/) const {
    return value_;
}

std::string Number::print() const {
    if (lround(value_) == value_) {
        return std::to_string(lround(value_));
    } else {
        return std::to_string(value_);
    }
}

double NumericProperty::eval(const Frame& frame, const Match& match) const {
    return this->value(frame, match[argument_]);
}

std::string NumericProperty::print() const {
    return this->name() + "(#" + std::to_string(argument_ + 1) + ")";
}

std::string Index::name() const {
    return "index";
}

double Index::value(const Frame& /*unused*/, size_t i) const {
    return static_cast<double>(i);
}

std::string Resid::name() const {
    return "resid";
}

double Resid::value(const Frame& frame, size_t i) const {
    auto residue = frame.topology().residue_for_atom(i);
    if (residue && residue->id()) {
        return static_cast<double>(*residue->id());
    } else {
        return -1;
    }
}

std::string Mass::name() const {
    return "mass";
}

double Mass::value(const Frame& frame, size_t i) const {
    return frame[i].mass();
}

std::string Position::name() const {
    switch (coordinate_) {
    case Coordinate::X:
        return "x";
    case Coordinate::Y:
        return "y";
    case Coordinate::Z:
        return "z";
    }
    unreachable();
}

double Position::value(const Frame& frame, size_t i) const {
    return frame.positions()[i][static_cast<size_t>(coordinate_)];
}

std::string Velocity::name() const {
    switch (coordinate_) {
    case Coordinate::X:
        return "vx";
    case Coordinate::Y:
        return "vy";
    case Coordinate::Z:
        return "vz";
    }
    unreachable();
}

double Velocity::value(const Frame& frame, size_t i) const {
    if (frame.velocities()) {
        auto& velocities = *frame.velocities();
        return velocities[i][static_cast<size_t>(coordinate_)];
    } else {
        return 0.0;
    }
}
