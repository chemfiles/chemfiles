// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_MMTF_FILE_HPP
#define CHEMFILES_MMTF_FILE_HPP

#include "chemfiles/File.hpp"
#include <mmtf/structure_data.hpp>

namespace chemfiles {

/// Simple RAII capsule for `MMTF_container`, handling the creation and
/// destruction of the file as needed.
class MMTFFile final: public File {
public:
    MMTFFile(std::string filename, File::Mode mode);
    ~MMTFFile() noexcept override;
    MMTFFile(MMTFFile&&) = default;
    MMTFFile& operator=(MMTFFile&&) = delete;
    MMTFFile(MMTFFile const&) = delete;
    MMTFFile& operator=(MMTFFile const&) = delete;

    mmtf::StructureData *operator->() {return &handle_;}

private:
    /// underlying pointer to the MMTF file
    mmtf::StructureData handle_;
};

} // namespace chemfiles

#endif
