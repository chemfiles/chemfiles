#ifndef CHEMFILES_TESTS_HELPERS_HPP
#define CHEMFILES_TESTS_HELPERS_HPP

#include <array>

namespace chemfiles {
    using Vector3D = std::array<double, 3>;
}

bool approx_eq(const chemfiles::Vector3D& a, const chemfiles::Vector3D& b, double tolerance=1e-15);

#endif
