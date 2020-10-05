// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_PDB_CONNECTIVITY_HPP
#define CHEMFILES_PDB_CONNECTIVITY_HPP

#include <vector>
#include <string>
#include <unordered_map>

#include "chemfiles/external/optional.hpp"

namespace chemfiles {

/// A string interner for PDB connectivity. This convert an index to a string,
/// allow to save memory in the PDB connectivity table.
///
/// The set of known strings is set at compile time by the pdb_connectivity.py
/// script.
class InternedName {
public:
    /// `InternedName` can be converted implicitly to string
    operator const std::string&() const {
        return INTERNER_.at(index_);
    }

    /// Get the string corresponding to this interned name
    const std::string& string() const {
        return INTERNER_.at(index_);
    }
private:
    InternedName(size_t index): index_(index) {}
    size_t index_;

    friend bool operator==(const InternedName&, const InternedName&);
    friend bool operator!=(const InternedName& lhs, const InternedName& rhs);
    friend struct std::hash<InternedName>;
    friend class PDBConnectivity;

    /// The whole list of known strings. This is generated at compile time
    /// with the pdb_connectivity.py script.
    static const std::vector<std::string> INTERNER_;
};

inline bool operator==(const InternedName& lhs, const InternedName& rhs) {
    return lhs.index_ == rhs.index_;
}

inline bool operator!=(const InternedName& lhs, const InternedName& rhs) {
    return lhs.index_ != rhs.index_;
}

inline bool operator<(const InternedName& lhs, const InternedName& rhs) {
    return lhs.string() < rhs.string();
}

inline bool operator<=(const InternedName& lhs, const InternedName& rhs) {
    return lhs.string() <= rhs.string();
}

inline bool operator>(const InternedName& lhs, const InternedName& rhs) {
    return lhs.string() > rhs.string();
}

inline bool operator>=(const InternedName& lhs, const InternedName& rhs) {
    return lhs.string() >= rhs.string();
}

}  // namespace chemfiles

namespace std {
    template<> struct hash<chemfiles::InternedName> {
        typedef chemfiles::InternedName argument_type;
        typedef ::size_t result_type;
        result_type operator()(argument_type const& s) const noexcept {
            return std::hash<::size_t>{}(s.index_);
        }
    };
}

namespace chemfiles {

class PDBConnectivity {
public:
    using ResidueConnectMap = std::unordered_multimap<InternedName, InternedName>;
    using PDBConnectMap = std::unordered_map<std::string, ResidueConnectMap>;

    static optional<const ResidueConnectMap&> find(const std::string& name) {
        const auto& it = PDB_CONNECTIVITY_MAP_.find(name);
        if (it == PDB_CONNECTIVITY_MAP_.end()) {
            return nullopt;
        } else {
            return {it->second};
        }
    }

private:
    static const PDBConnectMap PDB_CONNECTIVITY_MAP_;
};

}  // namespace chemfiles



#endif
