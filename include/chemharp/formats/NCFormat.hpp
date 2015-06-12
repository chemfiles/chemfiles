/*
 * Chemharp, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#include "chemharp/config.hpp"
#if HAVE_NETCDF

#ifndef HARP_FORMAT_NC_HPP
#define HARP_FORMAT_NC_HPP

#include "chemharp/Vector3D.hpp"
#include "chemharp/Format.hpp"
#include "chemharp/TrajectoryFactory.hpp"

namespace harp {

class UnitCell;
class Topology;
class NCFile;

/*!
 * @class NCFormat formats/NCFormat.hpp formats/NCFormat.cpp
 * @brief Amber NetCDF file format reader.
 *
 * http://ambermd.org/netcdf/nctraj.xhtml
 */
class NCFormat : public Format {
public:
    NCFormat();
    ~NCFormat() = default;

    virtual void read_step(File* file, const size_t step, Frame& frame) override;
    virtual void read(File* file, Frame& frame) override;
    virtual void write(File* file, const Frame& frame) override;

    virtual size_t nsteps(File* file) const override;
    virtual std::string description() const override;
private:
    //! Reserve size for \c natoms on the internal cache.
    void reserve(size_t natoms) const;
    //! Read the unit cell at the current internal step, the file is assumed to be valid.
    UnitCell read_cell(NCFile* file) const;
    //! Generic function to read an Array3D at the current internal step,
    //! the file is assumed to be valid.
    void read_array3D(NCFile* file, Array3D& arr, const string& name) const;
    //! Validate a file before reading it. If the \c natoms argument is present,
    //! we also check that the corresponding dimension match this value
    void validate(NCFile* file, size_t natoms=static_cast<size_t>(-1)) const;

    //! Write an Array3D to the file, as a variable with the name \c name, at
    //! the current internal step.
    void write_array3D(NCFile* file, const Array3D& arr, const string& name) const;
    //! Write an UnitCell to the file, at the current internal step
    void write_cell(NCFile* file, const UnitCell& cell) const;

    //! Last read step
    mutable size_t step;
    //! Store the adress of the last read file, and its validity
    mutable size_t last_file_hash;
    //! Was the last seen file valid ?
    mutable bool last_file_was_valid;
    // Temporary cache for read and write operations.
    mutable std::vector<float> cache;
    // Let's register the format
    REGISTER_FORMAT;
};

} // namespace harp

#endif

#endif // HAVE_NETCDF
