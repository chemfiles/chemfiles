set(EXTERNAL_FLAGS "")
# Disable warnings about external code
if(MSVC)
    string(REGEX REPLACE "/Wall" "/w" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    string(REGEX REPLACE "/Wall" "/w" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    set(EXTERNAL_FLAGS "${EXTERNAL_FLAGS} /EHsc")
else()
    set(EXTERNAL_FLAGS "${EXTERNAL_FLAGS} -w")
endif()

if(${BUILD_SHARED_LIBS})
    set(EXTERNAL_FLAGS "${EXTERNAL_FLAGS} -fPIC")
endif()

if(WIN32)
    set(EXTERNAL_FLAGS "${EXTERNAL_FLAGS}
set(WIN32")
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTERNAL_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTERNAL_FLAGS}")

function(unpack_external_library _name_)
    if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/${_name_}/CMakeLists.txt")
        message(STATUS "Unpacking ${_name_} sources")
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/${_name_}.tar.gz
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )
    endif()
endfunction()

# ============================================================================ #
unpack_external_library(fmt)
add_subdirectory(
    ${CMAKE_CURRENT_BINARY_DIR}/fmt
    ${CMAKE_CURRENT_BINARY_DIR}/fmt
)

# ============================================================================ #

if(CHFL_SYSTEM_NETCDF)
    find_package(NetCDF REQUIRED)
    set(NETCDF_OBJECTS "")
else()
    # Build internal NetCDF library
    if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/netcdf/CMakeLists.txt")
        message(STATUS "Unpacking NetCDF sources")
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/netcdf3-4.4.1.1.tar.gz
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )
    endif()

    add_subdirectory(
        ${CMAKE_CURRENT_BINARY_DIR}/netcdf
        ${CMAKE_CURRENT_BINARY_DIR}/netcdf
    )

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
        ${CMAKE_CURRENT_BINARY_DIR}/netcdf/
        PARENT_SCOPE
    )
    set(NETCDF_OBJECTS
        $<TARGET_OBJECTS:netcdf3-chemfiles>
        $<TARGET_OBJECTS:netcdf3>
        $<TARGET_OBJECTS:dispatch>
        PARENT_SCOPE
    )
    set(NETCDF_LIBRARIES "")
endif()

# ============================================================================ #

# TNG library
if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/tng/CMakeLists.txt")
    message(STATUS "Unpacking TNG sources")
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/tng.tar.gz
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endif()

add_subdirectory(
    ${CMAKE_CURRENT_BINARY_DIR}/tng
    ${CMAKE_CURRENT_BINARY_DIR}/tng
)

# Hide TNG variables from CMake GUI
mark_as_advanced(FORCE
    TNG_BUILD_COMPRESSION_TESTS
    TNG_BUILD_EXAMPLES
    TNG_BUILD_FORTRAN
    TNG_BUILD_OWN_ZLIB
    TNG_BUILD_TEST
)

# ============================================================================ #

set(VMD_MOLFILE_PLUGINS
    dcdplugin gromacsplugin
    pdbplugin lammpsplugin
)

if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/molfiles/CMakeLists.txt")
    message(STATUS "Unpacking molfiles sources")
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/molfiles.tar.gz
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endif()

add_subdirectory(
    ${CMAKE_CURRENT_BINARY_DIR}/molfiles
    ${CMAKE_CURRENT_BINARY_DIR}/molfiles
)

# ==========
# Plugins not yet enabled but that should compile without issue:
# ==========
#     abinitplugin avsplugin babelplugin basissetplugin bgfplugin binposplugin
#     biomoccaplugin brixplugin carplugin ccp4plugin cifplugin corplugin
#     cpmdplugin crdplugin cubeplugin dlpolyplugin dsn6plugin dtrplugin dxplugin
#     edmplugin fs4plugin gamessplugin graspplugin grdplugin gridplugin jsplugin
#     maeffplugin mapplugin mdfplugin mol2plugin moldenplugin molemeshplugin
#     msmsplugin namdbinplugin netcdfplugin offplugin parm7plugin parmplugin
#     pbeqplugin phiplugin pltplugin pqrplugin psfplugin raster3dplugin rst7plugin
#     situsplugin spiderplugin stlplugin tinkerplugin uhbdplugin
#     vasp5xdatcarplugin vaspchgcarplugin vaspoutcarplugin vaspparchgplugin
#     vaspposcarplugin vaspxdatcarplugin vaspxmlplugin vtfplugin xbgfplugin
#     xsfplugin xyzplugin

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
# Needs to link the TNGio library
#       tngplugin
# Needs to link to TCL interpreter
#       webpdbplugin
