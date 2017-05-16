// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_CONFIGURATION_HPP
#define CHEMFILES_CONFIGURATION_HPP

#include <string>
#include <unordered_map>

namespace chemfiles {

/// Configuration for chemfiles.
///
/// The configuration is stored in TOML files, that are accessed and read on the
/// first access to the global Configuration instance.
///
/// The configuration contains type renaming information in the `[rename]` toml
/// table. For example,
///
/// ```toml
/// [rename]
/// Ow = "O"
/// Ht = "H"
/// ```
///
/// Will use `O` as type for all atoms with `Ow` type, and `H` for all atoms
/// with `Ht`.
class Configuration {
public:
    /// Get the renamed atomic type for `type`. If their is no renaming to
    /// perform for this atomic type, the initial atomic type is returned.
    static const std::string& rename(const std::string& type) {
        auto it = instance().types_rename_.find(type);
        if (it != instance().types_rename_.end()) {
            return it->second;
        } else {
            return type;
        }
    }

private:
    Configuration();
    Configuration& operator=(const Configuration&) = delete;
    Configuration(const Configuration&) = delete;
    Configuration(Configuration&&) = delete;
    Configuration& operator=(Configuration&&) = delete;

    /// Read the configuration file at the given path
    void read_configuration(std::string path);
    /// Get a lazyly-created Configuration. The configuration will be
    /// initialized on the first call to this function.
    static const Configuration& instance();

    /// Map for old-type => new-type renaming
    std::unordered_map<std::string, std::string> types_rename_;
};

} // namespace chemfiles

#endif
