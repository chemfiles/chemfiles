set(EXTERNAL_FLAGS "")
# Disable warnings about external code
if(MSVC)
    string(REGEX REPLACE "/Wall" "/w" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    string(REGEX REPLACE "/Wall" "/w" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    set(EXTERNAL_FLAGS "${EXTERNAL_FLAGS} /EHsc")
else()
    set(EXTERNAL_FLAGS "${EXTERNAL_FLAGS} -w")
endif()

if(${BUILD_SHARED_LIBS} AND NOT WIN32)
    set(EXTERNAL_FLAGS "${EXTERNAL_FLAGS} -fPIC")
endif()

if(WIN32)
    set(EXTERNAL_FLAGS "${EXTERNAL_FLAGS} -DWIN32")
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTERNAL_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTERNAL_FLAGS}")

function(external_library _name_)
    if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/${_name_}/CMakeLists.txt")
        message(STATUS "Unpacking ${_name_} sources")
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/${_name_}.tar.gz
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )
    endif()

    add_subdirectory(
        ${CMAKE_CURRENT_BINARY_DIR}/${_name_}
        ${CMAKE_CURRENT_BINARY_DIR}/${_name_}
    )
endfunction()

# Include directories for the dependencies
set(EXTERNAL_INCLUDES "")

# ==========
# fmtlib: https://github.com/chemfiles/fmt
# ==========
external_library(fmt)
list(APPEND EXTERNAL_INCLUDES ${CMAKE_CURRENT_BINARY_DIR}/fmt)

# ==========
# netcdf: https://github.com/chemfiles/netcdf-c
# ==========
if(CHFL_SYSTEM_NETCDF)
    find_package(NetCDF REQUIRED)
    set(NETCDF_OBJECTS "" PARENT_SCOPE)
else()
    external_library(netcdf)

    add_library(netcdf3-chemfiles OBJECT
        ${CMAKE_CURRENT_BINARY_DIR}/netcdf/liblib/nc_initialize.c
    )
    target_include_directories(netcdf3-chemfiles PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/netcdf/include
        ${CMAKE_CURRENT_BINARY_DIR}/netcdf/
    )

    # Hide NetCDF variables from CMake GUI
    mark_as_advanced(FORCE
        BUILDNAME BUILDNAME_PREFIX BUILDNAME_SUFFIX BUILD_TESTING BUILD_UTILITIES
        CHUNK_CACHE_NELEMS CHUNK_CACHE_PREEMPTION CHUNK_CACHE_SIZE
        DEFAULT_CHUNKS_IN_CACHE DEFAULT_CHUNK_SIZE ENABLE_CONVERSION_WARNINGS
        ENABLE_DAP ENABLE_DAP_GROUPS ENABLE_DISKLESS ENABLE_DOXYGEN ENABLE_EXAMPLES
        ENABLE_EXAMPLE_TESTS ENABLE_EXTRA_TESTS ENABLE_EXTREME_NUMBERS ENABLE_FFIO
        ENABLE_FSYNC ENABLE_HDF4 ENABLE_JNA ENABLE_LARGE_FILE_SUPPORT
        ENABLE_LARGE_FILE_TESTS ENABLE_LOGGING ENABLE_MMAP ENABLE_NETCDF_4
        ENABLE_PARALLEL4 ENABLE_PARALLEL_TESTS ENABLE_PNETCDF
        ENABLE_REMOTE_FORTRAN_BOOTSTRAP ENABLE_RPC ENABLE_STDIO ENABLE_TESTS
        ENABLE_V2_API HAVE_LIBM MAKEDIST_CMAKEFLAGS MAKEDIST_TARFLAGS
        MAX_DEFAULT_CACHE_SIZE NCIO_MINBLOCKSIZE NC_EXTRA_DEPS NC_FIND_SHARED_LIBS
        NC_M4 NETCDF_LIB_NAME PACKAGE TEMP_LARGE UNAME
    )

    set(NETCDF_INCLUDES
        ${CMAKE_CURRENT_BINARY_DIR}/netcdf/include
        ${CMAKE_CURRENT_BINARY_DIR}/netcdf
    )
    set(NETCDF_OBJECTS
        $<TARGET_OBJECTS:netcdf3-chemfiles>
        $<TARGET_OBJECTS:netcdf3>
        $<TARGET_OBJECTS:dispatch>
        PARENT_SCOPE
    )
    set(NETCDF_LIBRARIES "" PARENT_SCOPE)
endif()

list(APPEND EXTERNAL_INCLUDES ${NETCDF_INCLUDES})

# ==========
# TNG io: https://github.com/chemfiles/tng
# ==========
external_library(tng)
list(APPEND EXTERNAL_INCLUDES ${CMAKE_CURRENT_BINARY_DIR}/tng/include)

# Hide TNG variables from CMake GUI
mark_as_advanced(FORCE
    TNG_BUILD_COMPRESSION_TESTS
    TNG_BUILD_EXAMPLES
    TNG_BUILD_FORTRAN
    TNG_BUILD_OWN_ZLIB
    TNG_BUILD_TEST
)

# ==========
# VMD molfile: https://github.com/chemfiles/molfiles
# ==========
set(VMD_MOLFILE_PLUGINS
    dcdplugin gromacsplugin lammpsplugin
)

external_library(molfiles)
list(APPEND EXTERNAL_INCLUDES ${CMAKE_CURRENT_BINARY_DIR}/molfiles/include)

# ==========
# Plugins not yet enabled but that should compile without issue:
# ==========

#     abinitplugin avsplugin babelplugin basissetplugin bgfplugin binposplugin
#     biomoccaplugin brixplugin carplugin ccp4plugin cifplugin corplugin
#     cpmdplugin crdplugin cubeplugin dlpolyplugin dsn6plugin dtrplugin dxplugin
#     edmplugin fs4plugin gamessplugin graspplugin grdplugin gridplugin jsplugin
#     maeffplugin mapplugin mdfplugin mol2plugin moldenplugin molemeshplugin
#     msmsplugin namdbinplugin netcdfplugin offplugin parm7plugin parmplugin
#     pbeqplugin phiplugin pltplugin pqrplugin psfplugin raster3dplugin
#     rst7plugin situsplugin spiderplugin stlplugin tinkerplugin uhbdplugin
#     vasp5xdatcarplugin vaspchgcarplugin vaspoutcarplugin vaspparchgplugin
#     vaspposcarplugin vaspxdatcarplugin vaspxmlplugin vtfplugin xbgfplugin
#     xsfplugin


# ==========
# Plugins with a chemfiles specific implementation
# ==========
#     xyzplugin pdbplugin tngplugin

# ==========
# These plugins do not compile or link
# ==========
# error: no member named 'have_esp' in 'molfile_qm_metadata_t'
#       cpmdlogplugin
# Needs to link the sqlite3 library
#       dmsplugin
# error: no member named 'have_esp' in 'molfile_qm_metadata_t'
#       gaussianplugin
# Needs to link to some XML parser
#       hoomdplugin
# The code is not yet written ...
#       mmcif
# Needs to link to TCL interpreter
#       webpdbplugin

set(EXTERNAL_INCLUDES ${EXTERNAL_INCLUDES} PARENT_SCOPE)
