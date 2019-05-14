// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_FORMAT_CML_HPP
#define CHEMFILES_FORMAT_CML_HPP

#include <unordered_map>

#include "chemfiles/Format.hpp"
#include "chemfiles/File.hpp"

#include "pugixml.hpp"

namespace chemfiles {

/// [CML] file format reader.
///
/// [CML]: http://xml-cml.org/
class CMLFormat final: public Format {
public:
    CMLFormat(std::string path, File::Mode mode, File::Compression compression);
    ~CMLFormat() override;

    void read_step(size_t step, Frame& frame) override;
    void read(Frame& frame) override;
    void write(const Frame& frame) override;
    size_t nsteps() override;
private:
    /// Read the atoms from `atoms` into `frame`
    void read_atoms(Frame& frame, const pugi::xml_node& atoms);
    /// Read the bonds from `bonds` into `frame`
    void read_bonds(Frame& frame, const pugi::xml_node& bonds);

    /// Needed to set the format declaration
    File::Mode mode_;

    /// Text file where we read from. It needs to stay valid if we write to the file
    std::unique_ptr<TextFile> file_;

    /// XML document must be kept alive with the object
    pugi::xml_document document_;

    /// First actual node.  Could be 'cml' (multi-frame) or 'molecule'(single-frame).
    /// Others (like single atom) are not supported
    pugi::xml_node root_;

    /// If multi-frame, store the current location
    pugi::xml_named_node_iterator current_;

    /// Number of frames added to the file
    size_t num_added_ = 0;

    /// Atomic reference to atomic id in the current frame
    std::unordered_map<std::string, size_t> ref_to_id_;
};

template<> FormatInfo format_information<CMLFormat>();

} // namespace chemfiles

#endif
