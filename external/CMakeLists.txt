include(ExternalProject)

if(${ENABLE_NETCDF})
    include_directories(${NETCDF_INCLUDES})
    ExternalProject_add(netcdfcxx
        PREFIX netcdfcxx
        URL file://${CMAKE_CURRENT_SOURCE_DIR}/netcdfcxx.tar.gz
        URL_MD5 a50ba13ff6593c3c9293fd205addf288
        CMAKE_ARGS  -DBUILD_SHARED_LIBS=OFF
                    -DCMAKE_POSITION_INDEPENDENT_CODE=ON
                    -DBUILD_TESTING=OFF
                    -DNCXX_ENABLE_TESTS=OFF
                    -DCMAKE_BUILD_TYPE=release
                    # Setting compilers
                    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                    -DCMAKE_Fortran_COMPILER=${CMAKE_Fortran_COMPILER}
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

ExternalProject_Add(molfiles
    PREFIX molfiles
    URL file://${CMAKE_CURRENT_SOURCE_DIR}/molfiles.tar.gz
    URL_HASH SHA1=266b3e1b95ca422428f6a723aab1fcab0ca7b04d
    CMAKE_ARGS  -DCMAKE_BUILD_TYPE=release
                # Setting compilers
                -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                -DCMAKE_Fortran_COMPILER=${CMAKE_Fortran_COMPILER}
                # Setting installation directory
                -DCMAKE_INSTALL_PREFIX=${PROJECT_BINARY_DIR}
                # Support cross-compilation
                -DCMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}
                -DCMAKE_FIND_ROOT_PATH=${CMAKE_FIND_ROOT_PATH}
                -DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=${CMAKE_FIND_ROOT_PATH_MODE_LIBRARY}
                -DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=${CMAKE_FIND_ROOT_PATH_MODE_INCLUDE}
                -DCMAKE_FIND_ROOT_PATH_MODE_PROGRAM=${CMAKE_FIND_ROOT_PATH_MODE_PROGRAM}
)
