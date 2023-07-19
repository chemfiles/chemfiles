if (CMAKE_CROSSCOMPILING)
    message(FATAL_ERROR "can not build documentation when cross-compiling")
endif()

find_package(Doxygen REQUIRED)

execute_process(
    COMMAND ${DOXYGEN_EXECUTABLE} --version
    OUTPUT_VARIABLE DOXYGEN_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if ("${DOXYGEN_VERSION}" VERSION_LESS 1.8.20)
    message(WARNING
        "Your version of doxygen is too old (${DOXYGEN_VERSION}).\n"
        "Please upgrade to doxygen >= 1.8.20 for optimal output"
    )
endif()

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
    ${PROJECT_BINARY_DIR}/Doxyfile
    @ONLY
)
configure_file(
    ${PROJECT_SOURCE_DIR}/doc/conf.py.in
    ${PROJECT_BINARY_DIR}/doc/conf.py
    @ONLY
)

file(GLOB_RECURSE HEADERS ${PROJECT_SOURCE_DIR}/include/**.h*)
file(GLOB_RECURSE C_EXAMPLES ${PROJECT_SOURCE_DIR}/examples/c/doc/**.c)
file(GLOB_RECURSE CXX_EXAMPLES ${PROJECT_SOURCE_DIR}/examples/cpp/doc/**.cpp)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/doxygen/xml/index.xml
    COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile
    DEPENDS ${HEADERS} ${C_EXAMPLES} ${CXX_EXAMPLES} ${PROJECT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen"
)

add_custom_target(generate-doxygen-xml ALL
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/doxygen/xml/index.xml
)

add_executable(extract-formats extract-formats-doc.cpp)
target_link_libraries(extract-formats chemfiles)

add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/formats-overview.csv
    COMMAND extract-formats
    DEPENDS extract-formats
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/
    COMMENT "Extracting format informations"
)

add_custom_target(formats-overview-csv ALL
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/formats-overview.csv
)


find_package(Sphinx REQUIRED)

add_custom_target(doc_html ALL
    COMMAND ${SPHINX_EXECUTABLE} -b html -c ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_BINARY_DIR}/html
    DEPENDS generate-doxygen-xml formats-overview-csv
    COMMENT "Generating sphinx documentation"
)
