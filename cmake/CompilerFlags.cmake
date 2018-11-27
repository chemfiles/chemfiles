include(CheckCXXCompilerFlag)
include(CheckCCompilerFlag)

set(CMAKE_REQUIRED_QUIET YES)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Manually check for some flags, as some versions of CMake do not support
# `CMAKE_CXX_STANDARD`
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(${COMPILER_SUPPORTS_CXX11})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(${COMPILER_SUPPORTS_CXXOX})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
elseif(MSVC)
    if(${MSVC_VERSION} LESS 1900)
        message(SEND_ERROR "MSVC < 14.0 is not supported. Please update your compiler or use mingw")
    endif()
else()
    message(SEND_ERROR "The ${CMAKE_CXX_COMPILER} compiler lacks C++11 support. Use another compiler.")
endif()

CHECK_C_COMPILER_FLAG("-std=c99" COMPILER_SUPPORTS_C99)
if(${COMPILER_SUPPORTS_C99})
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
endif()

if(MSVC)
    add_definitions("/D NOMINMAX")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
    set(CMAKE_SHARED_LINKER_FLAGS "/SUBSYSTEM:CONSOLE")
endif()

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Intel")
    # Some version of intel compiler (icc 14 at least) have only partial support
    # for C++11
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMSGPACK_USE_CPP03")
endif()

if(${EMSCRIPTEN})
    if(${BUILD_SHARED_LIBS})
        # Shared libs where not tested and a lot of changes to the build system
        # for emscripten support disable things that are needed for shared libs
        # on Windows.
        message(WARNING "Shared libs are not supported with emscripten")
    endif()

    set(EMCC_FLAGS "")
    set(EMCC_FLAGS "${EMCC_FLAGS} -s DISABLE_EXCEPTION_CATCHING=0")
    set(EMCC_FLAGS "${EMCC_FLAGS} -s ERROR_ON_UNDEFINED_SYMBOLS=1")
    set(EMCC_FLAGS "${EMCC_FLAGS} -s ALLOW_MEMORY_GROWTH=1")
    set(EMCC_FLAGS "${EMCC_FLAGS} -s ASSERTIONS=1")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EMCC_FLAGS}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EMCC_FLAGS}")
endif()

if(${CMAKE_CXX_COMPILER_ID} MATCHES "PGI")
    if(${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 18)
        message(WARNING "Only PGI >= 18 was tested to be able to compile chemfiles")
    endif()
endif()

macro(add_warning_flag _flag_)
    CHECK_CXX_COMPILER_FLAG("${_flag_}" CXX_SUPPORTS${_flag_})
    CHECK_C_COMPILER_FLAG("${_flag_}" CC_SUPPORTS${_flag_})
    if(CXX_SUPPORTS${_flag_})
        set(CHEMFILES_CXX_WARNINGS "${CHEMFILES_CXX_WARNINGS} ${_flag_}")
    endif()

    if(CC_SUPPORTS${_flag_})
        set(CHEMFILES_C_WARNINGS "${CHEMFILES_C_WARNINGS} ${_flag_}")
    endif()
endmacro()

set(CHEMFILES_CXX_WARNINGS "")
set(CHEMFILES_C_WARNINGS "")

macro(remove_msvc_warning _warn_)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd${_warn_}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd${_warn_}")
endmacro()

macro(remove_intel_warning _warn_)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -diag-disable ${_warn_}")
endmacro()

if(MSVC)
    if(${CMAKE_CXX_FLAGS} MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/Wall" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall")
    endif()

    if(${CMAKE_C_FLAGS} MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/Wall" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    else()
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /Wall")
    endif()

    # Disable other warnings
    remove_msvc_warning(4061) # enumerator in switch of enum is not explicitly handled by a case label
    remove_msvc_warning(4127) # conditional expression is constant
    remove_msvc_warning(4275) # non-dll-export interface as base class of dll-export class
    remove_msvc_warning(4251) # class <> needs to be dll-export
    remove_msvc_warning(4503) # mangled name too long was truncated
    remove_msvc_warning(4514) # unreferenced inline function has been removed
    remove_msvc_warning(4582) # constructor is not implicitly called
    remove_msvc_warning(4583) # destructor is not implicitly called
    remove_msvc_warning(4623) # default constructor was implicitly defined as deleted
    remove_msvc_warning(4625) # copy constructor was implicitly defined as deleted
    remove_msvc_warning(4626) # assignment operator was implicitly defined as deleted
    remove_msvc_warning(4668) # not defined preprocessor macro, replacing with '0' for '#if/#elif'
    remove_msvc_warning(4627) # move assignment operator was implicitly defined as deleted
    remove_msvc_warning(4710) # function not inlined
    remove_msvc_warning(4711) # function selected for automatic inlining
    remove_msvc_warning(4820) # padding added
    remove_msvc_warning(5026) # move constructor was implicitly defined as deleted
    remove_msvc_warning(5027) # move assignment operator was implicitly defined as deleted
else()
    # Add some warnings in debug mode
    # Basic set of warnings
    add_warning_flag("-Wall")
    add_warning_flag("-Wextra")
    # Initialization and convertion values
    add_warning_flag("-Wuninitialized")
    add_warning_flag("-Wconversion")
    add_warning_flag("-Wsign-conversion")
    add_warning_flag("-Wsign-promo")
    # C++11 functionalities
    add_warning_flag("-Wsuggest-override")
    add_warning_flag("-Wsuggest-final-types")
    # C++ standard conformance
    add_warning_flag("-Wpedantic")
    add_warning_flag("-pedantic")
    # The compiler is your friend
    add_warning_flag("-Wdocumentation")
    add_warning_flag("-Wdeprecated")
    add_warning_flag("-Wextra-semi")
    add_warning_flag("-Wnon-virtual-dtor")
    add_warning_flag("-Wold-style-cast")
    add_warning_flag("-Wcast-align")
    add_warning_flag("-Wunused")
    add_warning_flag("-Woverloaded-virtual")
    add_warning_flag("-Wundefined-func-template")
    add_warning_flag("-Wmissing-prototypes")
    add_warning_flag("-Wmissing-variable-declarations")

    # Disable some strong warning that are OK here
    add_warning_flag("-Wno-unknown-pragmas")
    add_warning_flag("-Wno-overlength-strings")

    # Disable some warning implied by -Weverything
    # -Weverything is not activated by default, but adding these flags help
    # when adding it manually to check for new warnings 
    add_warning_flag("-Wno-c++98-compat")
    add_warning_flag("-Wno-c++98-compat-pedantic")
    add_warning_flag("-Wno-weak-vtables")
    add_warning_flag("-Wno-weak-template-vtables")
    add_warning_flag("-Wno-padded")
    add_warning_flag("-Wno-float-equal")
    add_warning_flag("-Wno-switch-enum")
    add_warning_flag("-Wno-documentation-unknown-command")
    add_warning_flag("-Wno-exit-time-destructors")
    add_warning_flag("-Wno-global-constructors")
    # This warning trigger when compiling for 64-bit,
    # but the code is relevant for 32-bit
    add_warning_flag("-Wno-tautological-type-limit-compare")
    # Not everyone is as smart as clang for code reachability
    add_warning_flag("-Wno-covered-switch-default")
    add_warning_flag("-Wno-unreachable-code-break")

    # Warnings for PGI compiler
    add_warning_flag("-Minform=warn")

    if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Intel")
        # external function definition with no prior declaration
        remove_intel_warning(1418)
        # Intel compiler is too strict in errors about 'explicit' keyword
        remove_intel_warning(2304)
        remove_intel_warning(2305)
        # parameter "args" was never referenced in variadic templates
        remove_intel_warning(869)
        # exception specification for implicitly declared virtual function ...
        # This is an issue with compiler generated destructors and noexcept
        remove_intel_warning(811)
        remove_intel_warning(809)
    endif()
endif()

set(CHFL_SANITIZER "none" CACHE STRING "Sanitizer (clang and gcc only) to use in the build")
set_property(CACHE CHFL_SANITIZER PROPERTY STRINGS none address memory undefined thread)

set(CHEMFILES_SANITIZERS "")
set(CHEMFILES_SANITIZERS "")

if(${CHFL_SANITIZER} STREQUAL "none")
    # Nothing to do
elseif(${CHFL_SANITIZER} STREQUAL "undefined")
    set(CHEMFILES_SANITIZERS "${CHEMFILES_SANITIZERS} -fsanitize=undefined")
    set(CHEMFILES_SANITIZERS "${CHEMFILES_SANITIZERS} -fsanitize=integer")
else()
    set(CHEMFILES_SANITIZERS "${CHEMFILES_SANITIZERS} -fsanitize=${CHFL_SANITIZER}")
endif()

if(NOT ${CHFL_SANITIZER} STREQUAL "none")
    set(CHEMFILES_SANITIZERS "${CHEMFILES_SANITIZERS} -fno-omit-frame-pointer")
    set(CHEMFILES_SANITIZERS "${CHEMFILES_SANITIZERS} -g")
endif()

try_compile(CHFL_HAS_THREAD_LOCAL
    ${PROJECT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/cmake/thread_local.cpp
)

if(${EMSCRIPTEN})
    # emscripten manages to compile the code, but fails at runtime with
    # 'missing function: __cxa_thread_atexit'
    set(CHFL_HAS_THREAD_LOCAL FALSE)
endif()

# Transform from ON/OFF to 0/1
if(${CHFL_HAS_THREAD_LOCAL})
    set(CHFL_HAS_THREAD_LOCAL 1)
else()
    set(CHFL_HAS_THREAD_LOCAL 0)
endif()
