// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_STRING_VIEW_HPP
#define CHEMFILES_STRING_VIEW_HPP

#include "chemfiles/external/string_view.hpp"

// Add overloads for const char* / string_view comparison, which are not
// provided in `external/string_view.hpp` for some MSVC versions.
// They need to be defined in nonstd::sv_lite namespace, so that they can
// be found by ADL.
namespace nonstd { namespace sv_lite {
#define GENERATE_OPERATOR(__operation__)                                       \
    inline bool operator __operation__ (const char* lhs, string_view rhs ) {   \
        return string_view(lhs).compare(rhs) __operation__ 0 ;                 \
    }                                                                          \
    inline bool operator __operation__(string_view lhs, const char* rhs) {     \
        return lhs.compare(rhs) __operation__ 0 ;                              \
    }

    GENERATE_OPERATOR(==)
    GENERATE_OPERATOR(!=)
    GENERATE_OPERATOR(<)
    GENERATE_OPERATOR(<=)
    GENERATE_OPERATOR(>)
    GENERATE_OPERATOR(>=)

#undef GENERATE_OPERATOR
}} // namespace nonstd::sv_lite

namespace chemfiles {
    using nonstd::string_view;

    using nonstd::operator==;
    using nonstd::operator!=;
    using nonstd::operator<;
    using nonstd::operator>;
    using nonstd::operator<=;
    using nonstd::operator>=;
}

// compatibility with fmt
#include <fmt/core.h>
#include <fmt/format.h>
namespace fmt {

template <>
struct formatter<chemfiles::string_view>: formatter<fmt::string_view> {
    template <typename FormatContext>
    auto format(chemfiles::string_view s, FormatContext &ctx) -> decltype(ctx.out()) {
        return formatter<fmt::string_view>::format(fmt::string_view(s.data(), s.size()), ctx);
    }
};

}

#endif
