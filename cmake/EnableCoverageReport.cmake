# - Creates a special coverage build type and target on GCC.
#
# Defines a function ENABLE_COVERAGE_REPORT which generates the coverage target
# for selected targets. Optional arguments to this function are used to filter
# unwanted results using globbing expressions. Moreover targets with tests for
# the source code can be specified to trigger regenerating the report if the
# test has changed
#
# ENABLE_COVERAGE_REPORT(TARGETS target... [FILTER filter...] [TESTS test targets...])
#
# To generate a coverage report first build the project with
# CMAKE_BUILD_TYPE=coverage, then call make test and afterwards make coverage.
#
# The coverage report is based on gcov. Depending on the availability of lcov
# a HTML report will be generated and/or an XML report of gcovr is found.
# The generated coverage target executes all found solutions. Special targets
# exist to create e.g. only the xml report: coverage-xml.
#
# Copyright (C) 2010 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
#
# This file may be licensed under the terms of the
# GNU Lesser General Public License Version 3 (the ``LGPL''),
# or (at your option) any later version.
#
# Software distributed under the License is distributed
# on an ``AS IS'' basis, WITHOUT WARRANTY OF ANY KIND, either
# express or implied. See the LGPL for the specific language
# governing rights and limitations.
#
# You should have received a copy of the LGPL along with this
# program. If not, go to http://www.gnu.org/licenses/lgpl.html
# or write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#
# The development of this software was supported by:
#   CoR-Lab, Research Institute for Cognition and Robotics
#     Bielefeld University

INCLUDE(ParseArguments)

FIND_PACKAGE(Lcov QUIET)
FIND_PACKAGE(gcovr QUIET)

FUNCTION(ENABLE_COVERAGE_REPORT)

    # argument parsing
    PARSE_ARGUMENTS(ARG "FILTER;TARGETS;TESTS" "" ${ARGN})

    SET(COVERAGE_RAW_FILE "${CMAKE_BINARY_DIR}/coverage.raw.info")
    SET(COVERAGE_FILTERED_FILE "${CMAKE_BINARY_DIR}/coverage.info")
    SET(COVERAGE_REPORT_DIR "${CMAKE_BINARY_DIR}/coveragereport")
    SET(COVERAGE_XML_FILE "${CMAKE_BINARY_DIR}/coverage.xml")
    SET(COVERAGE_XML_COMMAND_FILE "${CMAKE_BINARY_DIR}/coverage-xml.cmake")

    # decide if there is any tool to create coverage data
    SET(TOOL_FOUND FALSE)
    IF(LCOV_FOUND OR GCOVR_FOUND)
        SET(TOOL_FOUND TRUE)
    ENDIF()
    IF(NOT TOOL_FOUND)
        MESSAGE(STATUS "Cannot enable coverage targets because neither lcov nor gcovr are found.")
    ENDIF()

    STRING(TOLOWER "${CMAKE_BUILD_TYPE}" COVERAGE_BUILD_TYPE)

    IF((CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
        AND TOOL_FOUND AND "${COVERAGE_BUILD_TYPE}" MATCHES "coverage")

        MESSAGE(STATUS "Coverage support enabled for targets: ${ARG_TARGETS}")

        # create coverage build type
        SET(CMAKE_CXX_FLAGS_COVERAGE ${CMAKE_CXX_FLAGS_DEBUG} PARENT_SCOPE)
        SET(CMAKE_C_FLAGS_COVERAGE ${CMAKE_C_FLAGS_DEBUG} PARENT_SCOPE)
        SET(CMAKE_CONFIGURATION_TYPES ${CMAKE_CONFIGURATION_TYPES} coverage PARENT_SCOPE)

        # instrument targets
        SET_TARGET_PROPERTIES(${ARG_TARGETS} PROPERTIES COMPILE_FLAGS --coverage
                                                        LINK_FLAGS --coverage)

        # html report
        IF (LCOV_FOUND)

            MESSAGE(STATUS "Enabling HTML coverage report")

            # set up coverage target

            ADD_CUSTOM_COMMAND(OUTPUT ${COVERAGE_RAW_FILE}
                               COMMAND ${LCOV_EXECUTABLE} -c -d ${CMAKE_BINARY_DIR} -o ${COVERAGE_RAW_FILE}
                               WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                               COMMENT "Collecting coverage data"
                               DEPENDS ${ARG_TARGETS} ${ARG_TESTS}
                               VERBATIM)

            # filter unwanted stuff
            LIST(LENGTH ARG_FILTER FILTER_LENGTH)
            IF(${FILTER_LENGTH} GREATER 0)
                SET(FILTER COMMAND ${LCOV_EXECUTABLE})
                FOREACH(F ${ARG_FILTER})
                    SET(FILTER ${FILTER} -r ${COVERAGE_FILTERED_FILE} ${F})
                ENDFOREACH()
                SET(FILTER ${FILTER} -o ${COVERAGE_FILTERED_FILE})
            ELSE()
                SET(FILTER "")
            ENDIF()

            ADD_CUSTOM_COMMAND(OUTPUT ${COVERAGE_FILTERED_FILE}
                               COMMAND ${LCOV_EXECUTABLE} -e ${COVERAGE_RAW_FILE} "${CMAKE_SOURCE_DIR}*"  -o ${COVERAGE_FILTERED_FILE}
                               ${FILTER}
                               DEPENDS ${COVERAGE_RAW_FILE}
                               COMMENT "Filtering recorded coverage data for project-relevant entries"
                               VERBATIM)
            ADD_CUSTOM_COMMAND(OUTPUT ${COVERAGE_REPORT_DIR}
                               COMMAND ${CMAKE_COMMAND} -E make_directory ${COVERAGE_REPORT_DIR}
                               COMMAND ${GENHTML_EXECUTABLE} --legend --show-details -t "${PROJECT_NAME} test coverage" -o ${COVERAGE_REPORT_DIR} ${COVERAGE_FILTERED_FILE}
                               DEPENDS ${COVERAGE_FILTERED_FILE}
                               COMMENT "Generating HTML coverage report in ${COVERAGE_REPORT_DIR}"
                               VERBATIM)

            ADD_CUSTOM_TARGET(coverage-html
                              DEPENDS ${COVERAGE_REPORT_DIR})

        ENDIF()

        # xml coverage report
        IF(GCOVR_FOUND)

            MESSAGE(STATUS "Enabling XML coverage report")

            # gcovr cannot write directly to a file so the execution needs to
            # be wrapped in a cmake file that generates the file output
            FILE(WRITE ${COVERAGE_XML_COMMAND_FILE}
                 "SET(ENV{LANG} en)\n")
            FILE(APPEND ${COVERAGE_XML_COMMAND_FILE}
                 "EXECUTE_PROCESS(COMMAND \"${GCOVR_EXECUTABLE}\" -x -r \"${CMAKE_SOURCE_DIR}\" OUTPUT_FILE \"${COVERAGE_XML_FILE}\" WORKING_DIRECTORY \"${CMAKE_BINARY_DIR}\")\n")

            ADD_CUSTOM_COMMAND(OUTPUT ${COVERAGE_XML_FILE}
                               COMMAND ${CMAKE_COMMAND} ARGS -P ${COVERAGE_XML_COMMAND_FILE}
                               COMMENT "Generating coverage XML report"
                               VERBATIM)

            ADD_CUSTOM_TARGET(coverage-xml
                              DEPENDS ${COVERAGE_XML_FILE})

        ENDIF()

        # provide a global coverage target executing both steps if available
        SET(GLOBAL_DEPENDS "")
        IF(LCOV_FOUND)
            LIST(APPEND GLOBAL_DEPENDS ${COVERAGE_REPORT_DIR})
        ENDIF()
        IF(GCOVR_FOUND)
            LIST(APPEND GLOBAL_DEPENDS ${COVERAGE_XML_FILE})
        ENDIF()
        IF(LCOV_FOUND OR GCOVR_FOUND)
            ADD_CUSTOM_TARGET(coverage
                              DEPENDS ${GLOBAL_DEPENDS})
        ENDIF()

    ENDIF()

ENDFUNCTION()
