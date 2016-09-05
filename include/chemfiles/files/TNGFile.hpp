/* Chemfiles, an efficient IO library for chemistry file_ formats
* Copyright (C) 2015 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#ifndef CHEMFILES_TNG_FILE_HPP
#define CHEMFILES_TNG_FILE_HPP

#include "tng/tng_io.h"
#include "chemfiles/File.hpp"

namespace chemfiles {

//! Simple RAII capsule for `tng_trajectory_t`, handling the creation and
//! destruction of the file as needed.
class TNGFile final: public BinaryFile {
public:
    TNGFile(std::string filename, File::Mode mode);
    ~TNGFile();

    TNGFile(TNGFile&&) = default;
    TNGFile& operator=(TNGFile&&) = default;

    operator tng_trajectory_t() {return handle_;}

private:
    /// underlying pointer to the tng file
    tng_trajectory_t handle_;
};

/// Check a return code from a TNG function, and throw a `FileError` if the
/// status is not TNG_SUCCESS.
void check_tng_error(tng_function_status status, std::string function);

} // namespace chemfiles

#endif
