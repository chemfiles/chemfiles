include(ExternalProject)

if(${ENABLE_NETCDF})
    include_directories(${NETCDF_INCLUDES})
    ExternalProject_add(netcdfcxx
        PREFIX netcdfcxx
        URL file://${CMAKE_CURRENT_SOURCE_DIR}/netcdfcxx.tar.gz
        URL_MD5 445dda3fa385b1454446ed584a3ec73f
        CMAKE_ARGS  -DBUILD_SHARED_LIBS=OFF
                    -DCMAKE_POSITION_INDEPENDENT_CODE=ON
                    -DBUILD_TESTING=OFF
                    -DNCXX_ENABLE_TESTS=OFF
                    -DCMAKE_BUILD_TYPE=release
                    # Setting compilers
                    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                    -DCMAKE_Fortran_COMPILER=${CMAKE_Fortran_COMPILER}
                    -DCMAKE_RC_COMPILER=${CMAKE_RC_COMPILER}
                    # Setting installation directory
                    -DCMAKE_INSTALL_PREFIX=${PROJECT_BINARY_DIR}
                    # Support cross-compilation
                    -DCMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}
                    -DCMAKE_FIND_ROOT_PATH=${CMAKE_FIND_ROOT_PATH}
                    -DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=${CMAKE_FIND_ROOT_PATH_MODE_LIBRARY}
                    -DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=${CMAKE_FIND_ROOT_PATH_MODE_INCLUDE}
                    -DCMAKE_FIND_ROOT_PATH_MODE_PROGRAM=${CMAKE_FIND_ROOT_PATH_MODE_PROGRAM}
    )
    add_library(netcdf_cxx4 STATIC IMPORTED GLOBAL)
    set_property(
        TARGET netcdf_cxx4
        PROPERTY IMPORTED_LOCATION ${PROJECT_BINARY_DIR}/lib/libnetcdf-cxx4${CMAKE_STATIC_LIBRARY_SUFFIX}
    )
endif()

#----------------------------------------------------------------------------------------#
if(MSVC)
    # Do not issue warning for insecure code in Molfiles
    set(MOLFILE_FLAGS "${MOLFILE_FLAGS} -D_CRT_SECURE_OVERLOAD_STANDARD_NAMES=1")
    # strcasecmp and strncasecmp are POSIX functions
    set(MOLFILE_FLAGS "${MOLFILE_FLAGS} -Dstrcasecmp=_stricmp -Dstrncasecmp=_strnicmp")
else()
    # Disable warnings about this code
    set(MOLFILE_FLAGS "${MOLFILE_FLAGS} -w")
endif()

if(WIN32)
    set(MOLFILE_FLAGS "${MOLFILE_FLAGS} -DWIN32")
endif()

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/molfiles/LICENSE")
    message(STATUS "Unpacking molfiles sources")
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/molfiles.tar.gz
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()

set(VMD_MOLFILE_PLUGINS_SRC
    molfiles/src/abinitplugin.c
    molfiles/src/avsplugin.cxx
    molfiles/src/babelplugin.c
    molfiles/src/basissetplugin.c
    molfiles/src/bgfplugin.cxx
    molfiles/src/binposplugin.c
    molfiles/src/biomoccaplugin.cxx
    molfiles/src/brixplugin.cxx
    molfiles/src/carplugin.c
    molfiles/src/ccp4plugin.cxx
    molfiles/src/cifplugin.cxx
    molfiles/src/corplugin.c
    # error: no member named 'have_esp' in 'molfile_qm_metadata_t'
    # molfiles/src/cpmdlogplugin.c
    molfiles/src/cpmdplugin.c
    molfiles/src/crdplugin.c
    molfiles/src/cubeplugin.cxx
    molfiles/src/dcdplugin.c
    molfiles/src/dlpolyplugin.c
    # Needs to link the sqlite3 library
    # molfiles/src/dmsplugin.cxx
    molfiles/src/dsn6plugin.cxx
    molfiles/src/dtrplugin.cxx
    molfiles/src/dxplugin.cxx
    molfiles/src/edmplugin.cxx
    molfiles/src/fs4plugin.cxx
    molfiles/src/gamessplugin.c
    # error: no member named 'have_esp' in 'molfile_qm_metadata_t'
    # molfiles/src/gaussianplugin.c
    molfiles/src/graspplugin.cxx
    molfiles/src/grdplugin.cxx
    molfiles/src/gridplugin.cxx
    molfiles/src/gromacsplugin.cxx
    # Needs to link to some XML parser
    # molfiles/src/hoomdplugin.c
    molfiles/src/jsplugin.c
    molfiles/src/lammpsplugin.c
    molfiles/src/maeffplugin.cxx
    molfiles/src/mapplugin.cxx
    molfiles/src/mdfplugin.cxx
    # The code is not yet written ...
    # molfiles/src/mmcif.cxx
    molfiles/src/mol2plugin.cxx
    molfiles/src/moldenplugin.c
    molfiles/src/molemeshplugin.cxx
    molfiles/src/msmsplugin.cxx
    molfiles/src/namdbinplugin.c
    molfiles/src/netcdfplugin.c
    molfiles/src/offplugin.cxx
    molfiles/src/parm7plugin.cxx
    molfiles/src/parmplugin.cxx
    molfiles/src/pbeqplugin.cxx
    molfiles/src/pdbplugin.c
    molfiles/src/phiplugin.cxx
    molfiles/src/pltplugin.cxx
    molfiles/src/pqrplugin.c
    molfiles/src/psfplugin.c
    molfiles/src/raster3dplugin.cxx
    molfiles/src/rst7plugin.c
    molfiles/src/situsplugin.cxx
    molfiles/src/spiderplugin.cxx
    molfiles/src/stlplugin.cxx
    molfiles/src/tinkerplugin.c
    # Needs to link the TNGio library
    # molfiles/src/tngplugin.cxx
    molfiles/src/uhbdplugin.cxx
    molfiles/src/vasp5xdatcarplugin.c
    molfiles/src/vaspchgcarplugin.c
    molfiles/src/vaspoutcarplugin.c
    molfiles/src/vaspparchgplugin.c
    molfiles/src/vaspposcarplugin.c
    molfiles/src/vaspxdatcarplugin.c
    molfiles/src/vaspxmlplugin.c
    molfiles/src/vtfplugin.c
    # Needs to link to TCL interpreter
    # molfiles/src/webpdbplugin.c
    molfiles/src/xbgfplugin.cxx
    molfiles/src/xsfplugin.cxx
    molfiles/src/xyzplugin.c
)

foreach(file IN ITEMS ${VMD_MOLFILE_PLUGINS_SRC})
    get_filename_component(plugin ${file} NAME_WE)
    set_source_files_properties(${file} PROPERTIES COMPILE_FLAGS "-DVMDPLUGIN=${plugin} ${MOLFILE_FLAGS}")
endforeach()

include_directories(molfiles/include)
add_library(molfiles OBJECT ${VMD_MOLFILE_PLUGINS_SRC})
