// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_FORMAT_FACTORY_HPP
#define CHEMFILES_FORMAT_FACTORY_HPP

#include <memory>
#include <functional>
#include <unordered_map>

#include "chemfiles/exports.hpp"
#include "chemfiles/mutex.hpp"
#include "chemfiles/File.hpp"
#include "chemfiles/Format.hpp"
#include "chemfiles/Error.hpp"

namespace chemfiles {

using format_creator_t = std::function<std::unique_ptr<Format>(const std::string& path, File::Mode mode)>;

/// This class allow to register Format with names and file extensions
class CHFL_EXPORT FormatFactory final {
private:
    FormatFactory();

public:
    /// Get the instance of the `FormatFactory`
    static FormatFactory& get();

    /// Get a `format_creator_t` from a format name.
    ///
    /// @param name the format name
    /// @throws FormatError if the format can not be found
    format_creator_t name(const std::string& name);

    /// Get a `format_creator_t` from a format extention.
    ///
    /// @param extension the format extention
    /// @throws FormatError if the format can not be found
    format_creator_t extension(const std::string& extension);

    /// Register a given `Format` in the factory.
    ///
    /// The format informations are taken from the specialization of the
    /// template function `chemfiles::format_information` for this format.
    template<class Format>
    void add_format() {
        auto info = format_information<Format>();
        register_format(info, [](const std::string& path, File::Mode mode) {
            return std::unique_ptr<Format>(new Format(path, mode));  // NOLINT no make_unique in C++11
        });
    }

    /// Get the metadata for all registered formats
    std::vector<FormatInfo> formats();

private:
    using formats_map_t = std::vector<std::pair<FormatInfo, format_creator_t>>;
    using iterator = formats_map_t::const_iterator;

    static iterator find_name(const formats_map_t& formats, const std::string& name);
    static iterator find_extension(const formats_map_t& formats, const std::string& extension);

    void register_format(FormatInfo info, format_creator_t creator);

    /// Trajectory map associating format descriptions and creators
    mutex<formats_map_t> formats_;
};

} // namespace chemfiles

#endif
