// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_CONFIGURATION_HPP
#define CHEMFILES_CONFIGURATION_HPP

#include <string>
#include <unordered_map>

#include "chemfiles/mutex.hpp"

namespace chemfiles {

/// Configuration for chemfiles.
///
/// The configuration is stored in TOML files, that are accessed and read on the
/// first access to the global Configuration instance.
///
/// The configuration contains type renaming information in the `[types]` toml
/// table. For example,
///
/// ```toml
/// [types]
/// Ow = "O"
/// Ht = "H"
/// ```
///
/// Will use `O` as type for all atoms with `Ow` type, and `H` for all atoms
/// with `Ht`.
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

    /// Read configuration from the file at `path`. If the same configuration
    /// data is already present in a previouly read configuration file, the
    /// data is replaced by the one in this file.
    ///
    /// If the file at `path` can not be opened, a `ConfigurationError` is
    /// thrown.
    static void add(const std::string& path);

private:
    Configuration();
    void read(const std::string& path);

    // Get the Configuration instance
    static Configuration& instance();

    using types_map = std::unordered_map<std::string, std::string>;
    /// Map for old-type => new-type renaming
    mutex<types_map> types_;
};

} // namespace chemfiles

#endif
