function(target_use_clang_tidy _target_)
    if (${CHFL_USE_CLANG_TIDY})
        set_target_properties(${_target_} PROPERTIES CXX_CLANG_TIDY ${CHEMFILES_CLANG_TIDY})
    endif()
endfunction()

if (NOT ${CHFL_USE_CLANG_TIDY})
    return()
endif()

set(CLANG_TIDY_CHECKS "\
    clang-*,\
    modernize-*,\
    bugprone-*,\
    misc-*,\
    performance-*,\
    readability-*,\
        -readability-else-after-return,\
        -readability-implicit-bool-conversion,\
    cppcoreguidelines-*,\
        -cppcoreguidelines-pro-type-union-access,\
        -cppcoreguidelines-pro-bounds-pointer-arithmetic,\
        -cppcoreguidelines-pro-type-reinterpret-cast,\
        -cppcoreguidelines-pro-bounds-constant-array-index,\
        -cppcoreguidelines-pro-bounds-array-to-pointer-decay,\
        -cppcoreguidelines-pro-type-vararg,\
        -cppcoreguidelines-owning-memory,\
")

if (NOT ${CMAKE_GENERATOR} MATCHES "Unix Makefiles|Ninja" )
    message(WARNING "clang-tidy integration requires 'Unix Makefiles' or 'Ninja' generator")
    return()
endif()

if (${CMAKE_VERSION} VERSION_LESS "3.6" )
    message(WARNING "clang-tidy integration requires cmake >= 3.6")
    return()
endif()
find_program(CLANG_TIDY_EXE NAMES "clang-tidy" PATHS /usr/local/opt/llvm/bin)

if (CLANG_TIDY_EXE)
    set(CHEMFILES_CLANG_TIDY "${CLANG_TIDY_EXE}")
    list(APPEND CHEMFILES_CLANG_TIDY "-checks=${CLANG_TIDY_CHECKS}")
    list(APPEND CHEMFILES_CLANG_TIDY "-header-filter='${CMAKE_SOURCE_DIR}/*'")
else()
    message(WARNING
        "CHFL_USE_CLANG_TIDY is enabled but clang-tidy is not found. "
        "Set CLANG_TIDY_EXE to clang-tidy path."
    )
endif()

mark_as_advanced(CLANG_TIDY_EXE CHEMFILES_CLANG_TIDY)
