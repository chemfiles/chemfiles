// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_CONFIGURATION_HPP
#define CHEMFILES_CONFIGURATION_HPP

#include <string>
#include <unordered_map>

#include "chemfiles/mutex.hpp"
#include "chemfiles/periodic_table.hpp"
#include "chemfiles/external/optional.hpp"

namespace toml {
    class value;
    using Table = std::unordered_map<std::string, value>;
}

namespace chemfiles {

/// Configuration for chemfiles.
///
/// The configuration is stored in TOML files, that are accessed and read on the
/// first access to the global Configuration instance.
///
/// The configuration contains multiple sections:
///
/// - the `[types]` section contains type renaming data.
/// - the `[atoms]` section contains atomic data.
///
/// ```toml
/// [types]
/// # All atoms named Ow should get the type `O`
/// Ow = "O"
/// # All atoms named Ht should get the type `H`
/// Ht = "H"
///
/// # Set data for all atoms with `CH3` type
/// [atoms.CH3]
/// mass = 15.035
/// charge = 0
/// ```
class Configuration final {
public:
    ~Configuration() = default;
    Configuration(const Configuration&) = delete;
    Configuration& operator=(const Configuration&) = delete;
    Configuration(Configuration&&) = delete;
    Configuration& operator=(Configuration&&) = delete;

    /// Get the renamed atomic type for `type`. If their is no renaming to
    /// perform for this atomic type, the initial atomic type is returned.
    static const std::string& rename(const std::string& type) {
        auto types = instance().types_.lock();
        auto it = types->find(type);
        if (it != types->end()) {
            return it->second;
        } else {
            return type;
        }
    }

    /// Get the atomic data for `type` if any.
    static optional<const AtomicData&> atom_data(const std::string& type) {
        auto atoms = instance().atoms_.lock();
        auto it = atoms->find(type);
        if (it != atoms->end()) {
            return it->second;
        } else {
            return {};
        }
    }

    /// Read configuration from the file at `path`. If the same configuration
    /// data is already present in a previously read configuration file, the
    /// data is replaced by the one in this file.
    ///
    /// If the file at `path` can not be opened, a `ConfigurationError` is
    /// thrown.
    static void add(const std::string& path);

private:
    Configuration();
    void read(const std::string& path);
    void read_types(const std::string& path, const toml::Table& data);
    void read_atomic_data(const std::string& path, const toml::Table& data);

    optional<std::string> atomic_data_string(
        const std::string& path,
        const toml::Table& table,
        const std::string& property,
        const std::string& atomic_type
    );
    optional<double> atomic_data_number(
        const std::string& path,
        const toml::Table& table,
        const std::string& property,
        const std::string& atomic_type
    );

    // Get the Configuration instance
    static Configuration& instance();

    using types_map = std::unordered_map<std::string, std::string>;
    /// Map for old-type => new-type renaming
    mutex<types_map> types_;
    /// Map for element type => data associations
    mutex<atomic_data_map> atoms_;
};

} // namespace chemfiles

#endif
