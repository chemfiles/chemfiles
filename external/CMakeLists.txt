# Do not set C standard for external code, as this make some
# symbols (such as strdup) undefined.
string(REPLACE "-std=c99" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

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

function(unpack_library _name_)
    set(_archive_ ${CMAKE_CURRENT_SOURCE_DIR}/${_name_}.tar.gz)
    file(SHA256 ${_archive_} _shasum_)
    if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/${_name_}/${_shasum_}")
        message(STATUS "Unpacking ${_name_} sources")
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E remove_directory ${_name_}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E tar xf ${_archive_}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E touch ${_name_}/${_shasum_}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )
    endif()
endfunction()

function(external_library _name_)
    unpack_library(${_name_})
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

# Hide FMT variables from CMake GUI
mark_as_advanced(FORCE
    FMT_CPPFORMAT
    FMT_DOC
    FMT_INSTALL
    FMT_PEDANTIC
    FMT_USE_CPP11
    FMT_TEST
)

# ==========
# toml11
# ==========
unpack_library(toml11)
list(APPEND EXTERNAL_INCLUDES ${CMAKE_CURRENT_BINARY_DIR}/toml11)

# ==========
# netcdf: https://github.com/chemfiles/netcdf-c
# ==========
if(CHFL_SYSTEM_NETCDF)
    find_package(NetCDF REQUIRED)
    set(NETCDF_OBJECTS "" PARENT_SCOPE)
else()
    if(EMSCRIPTEN)
        # Detection of _filelengthi64 does not seems to work with emscripten
        set(HAVE_FILE_LENGTH_I64 OFF)
    endif()
    external_library(netcdf)

    if(WINDOWS)
        target_compile_definitions(netcdf PRIVATE "-DDLL_NETCDF -DDLL_EXPORT")
    endif()

    set(NETCDF_INCLUDES
        ${CMAKE_CURRENT_BINARY_DIR}/netcdf/include
        ${CMAKE_CURRENT_BINARY_DIR}/netcdf
    )
    set(NETCDF_OBJECTS $<TARGET_OBJECTS:netcdf> PARENT_SCOPE)
    set(NETCDF_LIBRARIES "" PARENT_SCOPE)
    if(NOT EMSCRIPTEN)
        set(NETCDF_OBJECTS_TESTS ${NETCDF_OBJECTS} PARENT_SCOPE)
    else()
        set(NETCDF_OBJECTS_TESTS "" PARENT_SCOPE)
    endif()
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
