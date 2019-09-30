function(target_use_include_what_you_use _target_)
    if (${CHFL_USE_INCLUDE_WHAT_YOU_USE})
        set_target_properties(${_target_} PROPERTIES
            CXX_INCLUDE_WHAT_YOU_USE "${CHEMFILES_IWYU}"
            C_INCLUDE_WHAT_YOU_USE "${CHEMFILES_IWYU}"
        )
    endif()
endfunction()

if (NOT ${CHFL_USE_INCLUDE_WHAT_YOU_USE})
    return()
endif()

if (NOT ${CMAKE_GENERATOR} MATCHES "Unix Makefiles|Ninja" )
    message(WARNING "include-what-you-use integration requires 'Unix Makefiles' or 'Ninja' generator")
    return()
endif()

if (${CMAKE_VERSION} VERSION_LESS "3.3" )
    message(WARNING "include-what-you-use integration requires cmake >= 3.3")
    return()
endif()

find_program(INCLUDE_WHAT_YOU_USE_EXE NAMES "include-what-you-use" "iwyu")
if (INCLUDE_WHAT_YOU_USE_EXE)

    # Check version of include-what-you-use
    execute_process(
        COMMAND ${INCLUDE_WHAT_YOU_USE_EXE} --version
        OUTPUT_VARIABLE IWYU_FULL_VERSION
    )
    string(REGEX REPLACE "include-what-you-use ([0-9.]+) (.*)" "\\1" IWYU_VERSION "${IWYU_FULL_VERSION}")
    if("${IWYU_VERSION}" VERSION_LESS "0.12")
        message(FATAL_ERROR "Unsupported include-what-you-use (version ${IWYU_VERSION}): a newer version (at least 0.12) is required")
    endif()

    set(CHEMFILES_IWYU ${INCLUDE_WHAT_YOU_USE_EXE}
        -Xiwyu --no_comments # do not add '// for XXX' comments
        -Xiwyu --mapping_file=${PROJECT_SOURCE_DIR}/cmake/chemfiles-iwyu.imp
        -Xiwyu --check_also=${PROJECT_SOURCE_DIR}/include/chemfiles/*.hpp
        -Xiwyu --check_also=${PROJECT_SOURCE_DIR}/include/chemfiles/capi/*.h
        -Xiwyu --check_also=${PROJECT_SOURCE_DIR}/include/chemfiles/files/*.hpp
        -Xiwyu --check_also=${PROJECT_SOURCE_DIR}/include/chemfiles/formats/*.hpp
        -Xiwyu --check_also=${PROJECT_SOURCE_DIR}/include/chemfiles/selections/*.hpp
        -Xiwyu --verbose=3
    )
else()
    message(WARNING
        "CHFL_USE_INCLUDE_WHAT_YOU_USE is enabled but include-what-you-use is not found. "
        "Set INCLUDE_WHAT_YOU_USE_EXE to include-what-you-use path."
    )
endif()

mark_as_advanced(INCLUDE_WHAT_YOU_USE_EXE CHEMFILES_IWYU)
