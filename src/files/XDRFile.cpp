// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "chemfiles/files/XDRFile.hpp"

#include "xdrfile_xtc.h"
#include "xdrfile_trr.h"

#include "chemfiles/error_fmt.hpp"
#include "chemfiles/utils.hpp"
using namespace chemfiles;

XDRFile::XDRFile(Variants variant, std::string path, File::Mode mode)
    : File(std::move(path), mode, File::DEFAULT), handle_(nullptr) {

    std::function<int(const char*, int*, unsigned long*, int64_t**)> read_header;
    if (variant == XTC) {
        read_header = read_xtc_header;
    } else {
        assert(variant == TRR);
        read_header = read_trr_header;
    }

    const char* openmode;
    if (mode == File::READ) {
        openmode = "r";
        // Do not check the return value, if the file does not exist it will
        // fail the check below and produce a more informative error message
        read_header(this->path().c_str(), &natoms_, &nframes_, &offsets_);
    } else if (mode == File::WRITE) {
        openmode = "w";
    } else {
        openmode = "a";
        // Do not check return value, because the file might not exist. If it
        // does, we need to get the number of atoms and frames for appending
        read_header(this->path().c_str(), &natoms_, &nframes_, &offsets_);
    }

    handle_ = xdrfile_open(this->path().c_str(), openmode);
    if (handle_ == nullptr) {
        throw file_error("could not open the file at '{}'", this->path());
    }
}

XDRFile& XDRFile::operator=(XDRFile&& other) noexcept {
    if (handle_ != nullptr) {
        xdrfile_close(handle_);
    }
    free(offsets_);

    // Get the data from other
    handle_ = other.handle_;
    nframes_ = other.nframes_;
    offsets_ = other.offsets_;
    natoms_ = other.natoms_;

    // reset other
    other.handle_ = nullptr;
    other.offsets_ = nullptr;
    File::operator=(std::move(other));
    return *this;
}

XDRFile::~XDRFile() {
    xdrfile_close(handle_);
    free(offsets_);
}

unsigned long XDRFile::nframes() const { return nframes_; }

int64_t XDRFile::offset(size_t step) const {
    if (step >= static_cast<size_t>(nframes_)) {
        throw file_error("step {} is out of bounds, we have only {} frames", step, nframes_);
    }
    return offsets_[step];
}

int XDRFile::natoms() const {
    return natoms_;
}

void XDRFile::set_natoms(int natoms) {
    natoms_ = natoms;
}

void chemfiles::check_xdr_error(int status, const std::string& function) {
    switch (status) {
    case exdrHEADER:
    case exdrSTRING:
    case exdrDOUBLE:
    case exdrINT:
    case exdrFLOAT:
    case exdrUINT:
    case exdr3DX:
    case exdrCLOSE:
    case exdrMAGIC:
    case exdrNOMEM:
    case exdrENDOFFILE:
    case exdrFILENOTFOUND:
    case exdrNR:
        throw file_error(
            "error while calling {} in the XDR library: {}", function, exdr_message[status]
        );
    case exdrOK:
        // Do nothing, this is good
        break;
    default:
        throw file_error("unknown status code from XDR library: {}", status);
    }
}
