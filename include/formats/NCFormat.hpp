/*
 * Chemharp, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#include "config.hpp"
#if HAVE_NETCDF

#ifndef HARP_FORMAT_NC_HPP
#define HARP_FORMAT_NC_HPP

#include "Vector3D.hpp"
#include "Format.hpp"
#include "TrajectoryFactory.hpp"

namespace harp {

class UnitCell;
class Topology;
class NCFile;

/*!
 * @class NCFormat formats/AmberNetCDF.hpp formats/AmberNetCDF.cpp
 * @brief Amber NetCDF file format reader.
 *
 * http://ambermd.org/netcdf/nctraj.xhtml
 */
class NCFormat : public Format {
public:
    NCFormat();
    ~NCFormat() = default;

    void read_at_step(File* file, const size_t step, Frame& frame);
    void read_next_step(File* file, Frame& frame);
    std::string description() const;
private:
    //! Reserve size for \c natoms on the internal cache.
    void reserve(size_t natoms) const;
    //! Read the unit cell at the current internal step, assumed to be valid.
    UnitCell read_cell(NCFile* file) const;
    //! Read the positions at the current internal step, assumed to be valid.
    void read_positions(NCFile* file, Array3D& pos) const;
    //! Read the velocities at the current internal step, assumed to be valid.
    void read_velocities(NCFile* file, Array3D& vel) const;
    //! Generic function to read an Array3D at the current internal step,
    //! assumed to be valid.
    void read_array3D(NCFile* file, Array3D& arr, const string& name) const;

    //! Last read step
    size_t step;
    //! Store the adress of the last read file, and its validity
    size_t last_file_hash;
    //! Was the last seen file valid ?
    bool last_file_was_valid;
    // Temporary cache for read and write operations.
    mutable std::vector<float> cache;
    // Let's register the format
    REGISTER_FORMAT;
};

} // namespace harp

#endif

#endif // HAVE_NETCDF
