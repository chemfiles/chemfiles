// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <cassert>
#include <cstdint>

#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "chemfiles/error_fmt.hpp"
#include "chemfiles/external/optional.hpp"
#include "chemfiles/external/span.hpp"
#include "chemfiles/periodic_table.hpp"
#include "chemfiles/types.hpp"
#include "chemfiles/warnings.hpp"

#include "chemfiles/Atom.hpp"
#include "chemfiles/File.hpp"
#include "chemfiles/FormatMetadata.hpp"
#include "chemfiles/Frame.hpp"
#include "chemfiles/Residue.hpp"

#include "chemfiles/files/XDRFile.hpp"
#include "chemfiles/formats/TPR.hpp"

// see <GMX>/src/gromacs/fileio/tpxio.cpp
#define TPR_TAG_RELEASE "release"

// GROMACS explains: Enum of values that describe the contents of a TPR file
// whose format matches a version number.
// see `tpxv` in <GMX>/src/gromacs/fileio/tpxio.cpp
enum TPRVersion {
    tprv_ComputationalElectrophysiology = 96,
    tprv_Use64BitRandomSeed,
    tprv_RestrictedBendingAndCombinedAngleTorsionPotentials,
    tprv_InteractiveMolecularDynamics,
    tprv_RemoveObsoleteParameters1,
    tprv_PullCoordTypeGeom,
    tprv_PullGeomDirRel,
    tprv_IntermolecularBondeds,
    tprv_CompElWithSwapLayerOffset,
    tprv_CompElPolyatomicIonsAndMultipleIonTypes,
    tprv_RemoveAdress,
    tprv_PullCoordNGroup,
    tprv_RemoveTwinRange,
    tprv_ReplacePullPrintCOM12,
    tprv_PullExternalPotential,
    tprv_GenericParamsForElectricField,
    tprv_AcceleratedWeightHistogram,
    tprv_RemoveImplicitSolvation,
    tprv_PullPrevStepCOMAsReference,
    tprv_MimicQMMM,
    tprv_PullAverage,
    tprv_GenericInternalParameters,
    tprv_VSite2FD,
    tprv_AddSizeField,
    tprv_StoreNonBondedInteractionExclusionGroup,
    tprv_VSite1,
    tprv_MTS,
    tprv_RemovedConstantAcceleration,
    tprv_TransformationPullCoord,
    tprv_SoftcoreGapsys,
    tprv_ReaddedConstantAcceleration,
    tprv_RemoveTholeRfac,
    tprv_RemoveAtomtypes,
    tprv_Count // This number is for the total number of versions
};

// GROMACS explains:
// Version number of the file format written to run input
// files by this version of the code.
//
// The TPR version increases whenever the file format in the main
// development branch changes, due to an extension of the `TPRVersion` enum above.
// Backward compatibility for reading old run input files is maintained
// by checking this version number against that of the file and then using
// the correct code path.
static const int TPR_VERSION = tprv_Count - 1;

// GROMACS explains:
// Value of current TPR generation to keep track of incompatible changes for older TPR versions.
//
// The generation should be incremented when editing the TOPOLOGY or HEADER of the TPR format.
// In particular, updating `FUNCTION_TYPE_UPDATES` or changing the fields of the TPR header
// often triggers such needs.
//
// This way GROMACS can maintain forward compatibility for all analysis tools and/or external
// programs that only need to know the atom/residue names, charges, and bond connectivity.
//
// For chemfiles, this means it is possible to read TPR files from future versions as long as they
// are the same generation as specified here.
// The generation is tracked by GROMACS with an enum.
// see `TpxGeneration` in in <GMX>/src/gromacs/fileio/tpxio.cpp
static const int TPR_GENERATION = 28;

// This number should be the most recent backwards incompatible version.
// I.e., if this number is 9, we cannot read TPR version 9 with this code.
static const int TPR_INCOMPATIBLE_VERSION = 57; // GMX4.0 has version 58

using namespace chemfiles;

template <> const FormatMetadata& chemfiles::format_metadata<TPRFormat>() {
    static FormatMetadata metadata;
    metadata.name = "TPR";
    metadata.extension = ".tpr";
    metadata.description = "GROMACS TPR binary format";
    metadata.reference = "http://manual.gromacs.org/current/reference-manual/file-formats.html#tpr";

    metadata.read = true;
    metadata.write = false;
    metadata.memory = false;

    metadata.positions = false;
    metadata.velocities = false;
    metadata.unit_cell = true;
    metadata.atoms = false;
    metadata.bonds = false;
    metadata.residues = false;
    return metadata;
}

TPRFormat::TPRFormat(std::string path, File::Mode mode, File::Compression compression)
    : file_(std::move(path), mode) {
    if (compression != File::DEFAULT) {
        throw format_error("TPR format does not support compression");
    }
    if (mode != File::READ) {
        throw format_error("TPR format does not support write & append");
    }
}

size_t TPRFormat::nsteps() { return 1; }

void TPRFormat::read_step(size_t step, Frame& frame) {
    step_ = step;
    this->read(frame);
}

void TPRFormat::read(Frame& frame) {
    const TprHeader header = read_header();
    frame.resize(header.natoms);

    // Now read the body of the TPR file
    // see `do_tpx_body` in <GMX>/src/gromacs/fileio/tpxio.cpp

    if (header.has_box) {
        read_box(frame, header);
    }

    if (header.ngroups_temperature_coupling > 0) {
        const size_t ngtc_size = header.ngroups_temperature_coupling * header.sizeof_real;
        if (header.file_version < 69) {
            // Skip some legacy entries
            file_.skip(ngtc_size);
        }
        // GROMACS: These used to be the Berendsen tcoupl_lambda's
        file_.skip(ngtc_size);
    }

    step_++;
}

TPRFormat::TprHeader TPRFormat::read_header() {
    TprHeader header;

    const std::string version = file_.read_gmx_string();
    if (version.compare(0, 7, "VERSION") != 0) {
        throw format_error("unsupported file from a GROMACS version which is older than 2.0");
    }

    size_t precision = file_.read_single_size_as_i32();
    header.use_double = (precision == sizeof(double));
    if (precision != sizeof(float) && precision != sizeof(double)) {
        throw format_error("invalid precision {}, expected {} or {}", precision, sizeof(float),
                           sizeof(double));
    }

    header.sizeof_real = header.use_double ? sizeof(double) : sizeof(float);

    header.file_version = file_.read_single_i32();

    // GROMACS explains:
    // This is for backward compatibility with development versions 77-79
    // where the tag was, mistakenly, placed before the generation,
    // which would cause a segv instead of a proper error message
    // when reading the topology only from tpx with <77 code.
    std::string fileTag;
    if (header.file_version >= 77 && header.file_version <= 79) {
        fileTag = file_.read_gmx_string();
    }

    header.file_generation = file_.read_single_i32();

    if (header.file_version >= 81) {
        fileTag = file_.read_gmx_string();
    }
    if (header.file_version < 77 || header.file_version == 80) {
        // GROMACS: Versions before 77 don't have the tag, set it to release.
        // Version 80 is not handled by the current GROMACS implementation
        // but MDAnalysis sets the tag to release as well for version 80.
        // Version 80 was used by both 5.0-dev and 4.6-dev.
        fileTag = TPR_TAG_RELEASE;
    }

    // GROMACS explains:
    // We only support reading tpx files with the same tag as the code
    // or tpx files with the release tag and with lower version number.
    if (fileTag != TPR_TAG_RELEASE && header.file_version < TPR_VERSION) {
        throw format_error("TPR tag/version mismatch: reading file with version {}, tag '{}' with "
                           "program for version {}, tag '{}'",
                           header.file_version, fileTag, TPR_VERSION, TPR_TAG_RELEASE);
    }

    if (header.file_version > TPR_VERSION) {
        warning("TPR",
                "file version is from the future: implementation uses version {}, but got {}",
                TPR_VERSION, header.file_version);
    }

    // Assume `TopOnlyOK` is true, i.e. we need only the topology and not the input record.
    // This allows reading of future versions of the same generation.
    if ((header.file_version <= TPR_INCOMPATIBLE_VERSION) ||
        (header.file_generation > TPR_GENERATION)) {
        throw format_error("unable to read version {} with version {} program", header.file_version,
                           TPR_VERSION);
    }

    header.natoms = file_.read_single_size_as_i32();
    header.ngroups_temperature_coupling = file_.read_single_size_as_i32();

    if (header.file_version < 62) {
        // Skip some legacy entries
        file_.skip(sizeof(int) + header.sizeof_real);
    }
    if (header.file_version >= 79) {
        // Skip current value of the alchemical state
        file_.read_single_i32();
    }
    if (!header.use_double) {
        header.lambda = static_cast<double>(file_.read_single_f32());
    } else {
        header.lambda = file_.read_single_f64();
    }
    header.has_input_record = read_gmx_bool();
    header.has_topology = read_gmx_bool();
    header.has_positions = read_gmx_bool();
    header.has_velocities = read_gmx_bool();
    header.has_forces = read_gmx_bool();
    header.has_box = read_gmx_bool();

    if (header.file_version >= tprv_AddSizeField && header.file_generation >= 27) {
        // Skip size of the TPR body in bytes
        file_.read_single_i64();
    }

    if (header.file_generation > TPR_GENERATION && header.has_input_record) {
        // Trying to read a file from the future with an input record.
        // At this point in time, it's unknown what will be in the record.
        header.has_input_record = false;
    }

    if (header.file_version >= tprv_AddSizeField && header.file_generation >= 27) {
        header.body_convention = InMemory;
    } else {
        header.body_convention = FileIOXdr;
    }

    return header;
}

void TPRFormat::read_box(Frame& frame, const TprHeader& header) {
    const auto box = file_.read_gmx_box(header.use_double);
    frame.set_cell(box);

    if (header.file_version >= 51) {
        // Relative box vectors characteristic of the box shape
        // Skip unused 3*3 real matrix
        file_.skip(header.sizeof_real * 9);
    }

    // Box velocities for Parrinello-Rahman barostat
    // Skip unused 3*3 real matrix
    file_.skip(header.sizeof_real * 9);

    if (header.file_version < 56) {
        // Skip some legacy entries
        file_.skip(header.sizeof_real * 9);
    }
}

std::string TPRFormat::read_gmx_string(TprBodyConvention body_convention) {
    if (body_convention == FileIOXdr) {
        return file_.read_gmx_string();
    } else {
        assert(body_convention == InMemory);
        // Read a non-XDR-compliant *long* GROMACS string
        // A long GROMACS string stores the length of the string as uint64 before
        // the string contents. The contents are *not* padded to make the size a
        // multiple of four bytes.

        // lenght without null terminator
        const uint64_t len = file_.read_single_u64();
        // next comes the contents of the string without padding
        std::vector<char> buf;
        buf.resize(static_cast<size_t>(len));
        file_.read_char(buf);
        return std::string(buf.begin(), buf.end());
    }
}

bool TPRFormat::read_gmx_bool(TprBodyConvention body_convention) {
    if (body_convention == FileIOXdr) {
        return file_.read_single_i32() != 0;
    } else {
        assert(body_convention == InMemory);
        return file_.read_single_u8() != 0;
    }
}
