// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_XTC_FILE_HPP
#define CHEMFILES_XTC_FILE_HPP

#include "chemfiles/File.hpp"
#include "xdrfile.h"
#include "xdrfile_xtc.h"

namespace chemfiles {

/// Simple RAII capsule for `XDRFILE*`, handling the creation and
/// destruction of the file as needed.
/// Reads the file header and stores the offsets for individual frames.
class XTCFile final : public File {
  public:
    XTCFile(std::string path, File::Mode mode);
    ~XTCFile() override;
    XTCFile(XTCFile&&) noexcept = default;
    XTCFile& operator=(XTCFile&&) = default;
    XTCFile(XTCFile const&) = delete;
    XTCFile& operator=(XTCFile const&) = delete;

    /// get the number of frames/steps in the file, as indicated in the file header
    unsigned long nframes() const;
    /// get the offset corresponding to a specific frame/step
    int64_t offset(int64_t step) const;
    /// get the number of atoms, as indicated in the file header
    int natoms() const;

    operator XDRFILE*() { return handle_; }

  private:
    /// underlying pointer to the xtc file
    XDRFILE* handle_;
    /// The number of frames in the trajectory
    unsigned long nframes_ = 0;
    /// Offsets within file for fast indexing
    int64_t* offsets_ = nullptr;
    /// The number of atoms in the trajectory
    int natoms_ = 0;
};

/// Check a return code from a XTC function, and throw a `FileError` if the
/// status is not exdrOK.
void check_xtc_error(int status, const std::string& function);

} // namespace chemfiles

#endif
