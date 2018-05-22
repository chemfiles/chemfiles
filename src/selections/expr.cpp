// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "chemfiles/Frame.hpp"
#include "chemfiles/Selection.hpp"
#include "chemfiles/ErrorFmt.hpp"

#include "chemfiles/selections/expr.hpp"

using namespace chemfiles;
using namespace chemfiles::selections;

SubSelection::SubSelection(Variable variable): selection_(nullptr), variable_(variable) {}

SubSelection::SubSelection(std::string selection):
    selection_(new Selection(std::move(selection))), variable_(UINT8_MAX)
{
    if (selection_->size() != 1) {
        throw selection_error("sub-selection must have a size of 1");
    }
}

std::vector<size_t> SubSelection::eval(const Frame& frame, const Match& match) const {
    if (is_variable()) {
        return {match[variable_]};
    } else {
        return selection_->list(frame);
    }
}

std::string SubSelection::print() const {
    if (is_variable()) {
        return fmt::format("#{}", variable_ + 1);
    } else {
        return selection_->string();
    }
}

std::string And::print(unsigned delta) const {
    auto lhs = lhs_->print(7);
    auto rhs = rhs_->print(7);
    return fmt::format("and -> {}\n{:{}}    -> {}", lhs, "", delta, rhs);
}

bool And::is_match(const Frame& frame, const Match& match) const {
    return lhs_->is_match(frame, match) && rhs_->is_match(frame, match);
}

std::string Or::print(unsigned delta) const {
    auto lhs = lhs_->print(6);
    auto rhs = rhs_->print(6);
    return fmt::format("or -> {}\n{:{}}   -> {}", lhs, "", delta, rhs);
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


std::string IsBonded::print(unsigned /*unused*/) const {
    return fmt::format("is_bonded({}, {})", i_.print(), j_ .print());
}

bool IsBonded::is_match(const Frame& frame, const Match& match) const {
    auto& bonds = frame.topology().bonds();
    for (auto i: i_.eval(frame, match)) {
        for (auto j: j_.eval(frame, match)) {
            if (i == j) {
                // This is not a valid bond
                continue;
            }

            auto candidate = chemfiles::Bond(i, j);
            auto it = std::lower_bound(bonds.begin(), bonds.end(), candidate);
            if (it != bonds.end() && *it == candidate) {
                return true;
            }
        }
    }
    return false;
}

std::string IsAngle::print(unsigned /*unused*/) const {
    return fmt::format("is_angle({}, {}, {})", i_.print(), j_.print(), k_.print());
}

bool IsAngle::is_match(const Frame& frame, const Match& match) const {
    auto& angles = frame.topology().angles();
    for (auto i: i_.eval(frame, match)) {
        for (auto j: j_.eval(frame, match)) {
            for (auto k: k_.eval(frame, match)) {
                if (i == j || j == k || i == k) {
                    // This is not a valid angle
                    continue;
                }

                auto candidate = chemfiles::Angle(i, j, k);
                auto it = std::lower_bound(angles.begin(), angles.end(), candidate);
                if (it != angles.end() && *it == candidate) {
                    return true;
                }
            }
        }
    }
    return false;
}

std::string IsDihedral::print(unsigned /*unused*/) const {
    return fmt::format("is_dihedral({}, {}, {}, {})", i_.print(), j_.print(), k_.print(), m_.print());
}

bool IsDihedral::is_match(const Frame& frame, const Match& match) const {
    auto& dihedrals = frame.topology().dihedrals();
    for (auto i: i_.eval(frame, match)) {
        for (auto j: j_.eval(frame, match)) {
            for (auto k: k_.eval(frame, match)) {
                for (auto m: m_.eval(frame, match)) {
                    if (i == j || j == k || k == m || i == k || j == m || i == m) {
                        // This is not a valid angle
                        continue;
                    }

                    auto candidate = chemfiles::Dihedral(i, j, k, m);
                    auto it = std::lower_bound(dihedrals.begin(), dihedrals.end(), candidate);
                    if (it != dihedrals.end() && *it == candidate) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

std::string IsImproper::print(unsigned /*unused*/) const {
    return fmt::format("is_improper({}, {}, {}, {})", i_.print(), j_.print(), k_.print(), m_.print());
}

bool IsImproper::is_match(const Frame& frame, const Match& match) const {
    auto& impropers = frame.topology().impropers();
    for (auto i: i_.eval(frame, match)) {
        for (auto j: j_.eval(frame, match)) {
            for (auto k: k_.eval(frame, match)) {
                for (auto m: m_.eval(frame, match)) {
                    if (i == j || j == k || k == m || i == k || j == m || i == m) {
                        // This is not a valid angle
                        continue;
                    }

                    auto candidate = chemfiles::Improper(i, j, k, m);
                    auto it = std::lower_bound(impropers.begin(), impropers.end(), candidate);
                    if (it != impropers.end() && *it == candidate) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

std::string StringSelector::print(unsigned /*unused*/) const {
    auto op = equals_ ? "==" : "!=";
    return fmt::format("{}(#{}) {} {}", name(), argument_ + 1, op, value_);
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

void Math::optimize() {
    auto lhs_opt = lhs_->optimize();
    if (lhs_opt) {
        lhs_ = MathAst(new Number(lhs_opt.value()));
    }

    auto rhs_opt = rhs_->optimize();
    if (rhs_opt) {
        rhs_ = MathAst(new Number(rhs_opt.value()));
    }
}

double Add::eval(const Frame& frame, const Match& match) const {
    return lhs_->eval(frame, match) + rhs_->eval(frame, match);
}

optional<double> Add::optimize() {
    auto lhs_opt = lhs_->optimize();
    auto rhs_opt = rhs_->optimize();
    if (lhs_opt && lhs_opt) {
        return lhs_opt.value() + rhs_opt.value();
    } else if (lhs_opt) {
        lhs_ = MathAst(new Number(lhs_opt.value()));
    } else if (rhs_opt) {
        rhs_ = MathAst(new Number(rhs_opt.value()));
    }
    return nullopt;
}

std::string Add::print() const {
    return fmt::format("({} + {})", lhs_->print(), rhs_->print());
}

double Sub::eval(const Frame& frame, const Match& match) const {
    return lhs_->eval(frame, match) - rhs_->eval(frame, match);
}

optional<double> Sub::optimize() {
    auto lhs_opt = lhs_->optimize();
    auto rhs_opt = rhs_->optimize();
    if (lhs_opt && lhs_opt) {
        return lhs_opt.value() - rhs_opt.value();
    } else if (lhs_opt) {
        lhs_ = MathAst(new Number(lhs_opt.value()));
    } else if (rhs_opt) {
        rhs_ = MathAst(new Number(rhs_opt.value()));
    }
    return nullopt;
}

std::string Sub::print() const {
    return fmt::format("({} - {})", lhs_->print(), rhs_->print());
}

double Mul::eval(const Frame& frame, const Match& match) const {
    return lhs_->eval(frame, match) * rhs_->eval(frame, match);
}

optional<double> Mul::optimize() {
    auto lhs_opt = lhs_->optimize();
    auto rhs_opt = rhs_->optimize();
    if (lhs_opt && lhs_opt) {
        return lhs_opt.value() * rhs_opt.value();
    } else if (lhs_opt) {
        lhs_ = MathAst(new Number(lhs_opt.value()));
    } else if (rhs_opt) {
        rhs_ = MathAst(new Number(rhs_opt.value()));
    }
    return nullopt;
}

std::string Mul::print() const {
    return fmt::format("({} * {})", lhs_->print(), rhs_->print());
}

double Div::eval(const Frame& frame, const Match& match) const {
    return lhs_->eval(frame, match) / rhs_->eval(frame, match);
}

optional<double> Div::optimize() {
    auto lhs_opt = lhs_->optimize();
    auto rhs_opt = rhs_->optimize();
    if (lhs_opt && lhs_opt) {
        return lhs_opt.value() / rhs_opt.value();
    } else if (lhs_opt) {
        lhs_ = MathAst(new Number(lhs_opt.value()));
    } else if (rhs_opt) {
        rhs_ = MathAst(new Number(rhs_opt.value()));
    }
    return nullopt;
}

std::string Div::print() const {
    return fmt::format("({} / {})", lhs_->print(), rhs_->print());
}

double Pow::eval(const Frame& frame, const Match& match) const {
    return pow(lhs_->eval(frame, match), rhs_->eval(frame, match));
}

optional<double> Pow::optimize() {
    auto lhs_opt = lhs_->optimize();
    auto rhs_opt = rhs_->optimize();
    if (lhs_opt && lhs_opt) {
        return pow(lhs_opt.value(), rhs_opt.value());
    } else if (lhs_opt) {
        lhs_ = MathAst(new Number(lhs_opt.value()));
    } else if (rhs_opt) {
        rhs_ = MathAst(new Number(rhs_opt.value()));
    }
    return nullopt;
}

std::string Pow::print() const {
    return fmt::format("{} ^({})", lhs_->print(), rhs_->print());
}

double Neg::eval(const Frame& frame, const Match& match) const {
    return - ast_->eval(frame, match);
}

optional<double> Neg::optimize() {
    auto optimized = ast_->optimize();
    if (optimized) {
        return - optimized.value();
    } else {
        return nullopt;
    }
}

std::string Neg::print() const {
    return fmt::format("(-{})", ast_->print());
}

double Function::eval(const Frame& frame, const Match& match) const {
    return fn_(ast_->eval(frame, match));
}

optional<double> Function::optimize() {
    auto optimized = ast_->optimize();
    if (optimized) {
        return fn_(optimized.value());
    } else {
        return nullopt;
    }
}

std::string Function::print() const {
    return fmt::format("{}({})", name_, ast_->print());
}

double Number::eval(const Frame& /*unused*/, const Match& /*unused*/) const {
    return value_;
}

optional<double> Number::optimize() {
    return value_;
}

std::string Number::print() const {
    if (lround(value_) == value_) {
        return std::to_string(lround(value_));
    } else {
        return std::to_string(value_);
    }
}

double Distance::eval(const Frame& frame, const Match& match) const {
    return frame.distance(match[i_], match[j_]);
}

std::string Distance::print() const {
    return fmt::format("distance(#{}, #{})", i_ + 1, j_ + 1);
}

double selections::Angle::eval(const Frame& frame, const Match& match) const {
    return frame.angle(match[i_], match[j_], match[k_]);
}

std::string selections::Angle::print() const {
    return fmt::format("angle(#{}, #{}, #{})", i_ + 1, j_ + 1, k_ + 1);
}

double selections::Dihedral::eval(const Frame& frame, const Match& match) const {
    return frame.dihedral(match[i_], match[j_], match[k_], match[m_]);
}

std::string selections::Dihedral::print() const {
    return fmt::format("dihedral(#{}, #{}, #{}, #{})", i_ + 1, j_ + 1, k_ + 1, m_ + 1);
}

double OutOfPlane::eval(const Frame& frame, const Match& match) const {
    return frame.out_of_plane(match[i_], match[j_], match[k_], match[m_]);
}

std::string OutOfPlane::print() const {
    return fmt::format("out_of_plane(#{}, #{}, #{}, #{})", i_ + 1, j_ + 1, k_ + 1, m_ + 1);
}

double NumericProperty::eval(const Frame& frame, const Match& match) const {
    return this->value(frame, match[argument_]);
}

optional<double> NumericProperty::optimize() {
    return nullopt;
}

std::string NumericProperty::print() const {
    return fmt::format("{}(#{})", name(), argument_ + 1);
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
