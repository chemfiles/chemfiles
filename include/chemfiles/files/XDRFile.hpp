// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_XDR_FILE_HPP
#define CHEMFILES_XDR_FILE_HPP

#include <cstdint>
#include <string>

#include "chemfiles/File.hpp"

struct XDRFILE;

namespace chemfiles {

/// Simple RAII capsule for `XDRFILE*`, handling the creation and
/// destruction of the file as needed.
/// Reads the file header and stores the offsets for individual frames.
class XDRFile final : public File {
public:
    /// Possible variants of the XDR file
    enum Variants {
        /// XTC trajectory
        XTC,
        /// TRR trajectory
        TRR,
    };

    XDRFile(Variants variant, std::string path, File::Mode mode);
    ~XDRFile() override;

    XDRFile(XDRFile&& other) noexcept : File("", File::READ, File::DEFAULT) {
        *this = std::move(other);
    }
    XDRFile& operator=(XDRFile&& other) noexcept;

    XDRFile(XDRFile const&) = delete;
    XDRFile& operator=(XDRFile const&) = delete;

    /// get the number of frames/steps in the file, as indicated in the file header
    unsigned long nframes() const;
    /// get the offset corresponding to a specific frame/step
    int64_t offset(size_t step) const;
    /// get the number of atoms, as indicated in the file header
    int natoms() const;
    /// set the number of atoms
    void set_natoms(int natoms);

    operator XDRFILE*() { return handle_; }

private:
    /// underlying pointer to the xdr file
    XDRFILE* handle_;
    /// The number of frames in the trajectory
    unsigned long nframes_ = 0;
    /// Offsets within file for fast indexing
    int64_t* offsets_ = nullptr;
    /// The number of atoms in the trajectory
    int natoms_ = 0;
};

/// Check a return code from a xdrlib function, and throw a `FileError` if the
/// status is not exdrOK.
void check_xdr_error(int status, const std::string& function);

} // namespace chemfiles

#endif
