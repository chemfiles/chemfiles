find_package(Threads)
include(ExternalProject)

set(GTEST_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/gtest")
ExternalProject_Add(gtest-external
    URL https://googletest.googlecode.com/files/gtest-1.7.0.zip
    URL_HASH SHA1=f85f6d2481e2c6c4a18539e391aa4ea8ab0394af
    PREFIX "${GTEST_PREFIX}"
    INSTALL_COMMAND "")

set(LIBPREFIX "${CMAKE_STATIC_LIBRARY_PREFIX}")
set(LIBSUFFIX "${CMAKE_STATIC_LIBRARY_SUFFIX}")
set(GTEST_LOCATION "${GTEST_PREFIX}/src/gtest-external-build")
set(GTEST_LIBRARY  "${GTEST_LOCATION}/${LIBPREFIX}gtest${LIBSUFFIX}")
set(GTEST_MAINLIB  "${GTEST_LOCATION}/${LIBPREFIX}gtest_main${LIBSUFFIX}")

add_library(gtest IMPORTED STATIC GLOBAL)
set_target_properties(gtest PROPERTIES
    "IMPORTED_LOCATION" "${GTEST_LIBRARY}"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}")

add_library(gtestmain IMPORTED STATIC GLOBAL)
set_target_properties(gtestmain PROPERTIES
    "IMPORTED_LOCATION" "${GTEST_MAINLIB}"
    "IMPORTED_LINK_INTERFACE_LIBRARIES"
        "${GTEST_LIBRARY};${CMAKE_THREAD_LIBS_INIT}")

add_dependencies(gtest gtestmain)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/gtest/src/gtest-external/include/)
